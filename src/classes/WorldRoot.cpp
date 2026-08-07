#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/Part.hpp"
#include "gargantuan/classes/WeldConstraint.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/physics/Conversions.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL_log.h>
#include <box3d/box3d.h>
#include <box3d/collision.h>
#include <box3d/id.h>
#include <box3d/math_functions.h>
#include <box3d/types.h>
#include <cstddef>
#include <float.h>
#include <memory>
#include <unordered_map>

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(WorldRoot);
	constexpr int MAX_STEPS_PER_FRAME = 4;
	constexpr float TimeStep = 1.0f / 60.0f;
	constexpr int SubStepCount = 4;
	float Accumulator = 0.0f;

	WorldRoot::WorldRoot() {
		b3WorldDef worldDef = b3DefaultWorldDef();
		// put stuff to change the worlddef (probably based on project.config.json)
		// stuff like gravity and etc
		worldDef.gravity = b3Vec3{0.0f, -10.0f, 0.0f};
		b3WorldId worldId = b3CreateWorld(&worldDef);
		World = worldId;

		auto checkChildAdded = [this](Instance::Pointer instance) {
			if (instance->IsA("BasePart")) {
				std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
				this->Parts.push_back(part);

				b3BodyDef partBodyDef = b3DefaultBodyDef();
				partBodyDef.position = ToBox3(part->CFrame.Position);
				partBodyDef.rotation = ToBox3(part->CFrame.ToQuaternion());
				b3ShapeDef partShapeDef = b3DefaultShapeDef();
				if (part->Anchored == true) {
					partBodyDef.type = (b3BodyType)b3_staticBody;
				} else {
					partBodyDef.type = (b3BodyType)b3_dynamicBody;
					partShapeDef.density = 0.7f;
				};

				if (part->CanCollide == false) {
					partShapeDef.isSensor = true;
					partShapeDef.enableSensorEvents = true; // CanTouch
				}

				partBodyDef.userData = part.get();
				b3BodyId partId = b3CreateBody(World, &partBodyDef);
				// hull
				if (const Part *partNotBasePart = part->Cast<Part>()) {
					switch (partNotBasePart->Shape) {
					case Enums::PartType::Block: {
						b3BoxHull partBox = b3MakeBoxHull(
							part->Size.x * 0.5f, part->Size.y * 0.5f, part->Size.z * 0.5f
						);
						b3CreateHullShape(partId, &partShapeDef, &partBox.base);
						break;
					}
					case Enums::PartType::Wedge: {
						const glm::vec3 h = part->Size * 0.5f;
						const b3Vec3 points[6] = {
							{-h.x, -h.y, -h.z},
							{h.x, -h.y, -h.z},
							{h.x, -h.y, h.z},
							{-h.x, -h.y, h.z},
							{h.x, h.y, -h.z},
							{-h.x, h.y, -h.z},
						};
						b3HullData *hull = b3CreateHull(points, 6, 6);
						b3CreateHullShape(partId, &partShapeDef, hull);
						b3DestroyHull(hull);
						break;
					}
					case Enums::PartType::CornerWedge: {
						const glm::vec3 h = part->Size * 0.5f;
						const b3Vec3 points[5] = {
							{-h.x, -h.y, -h.z},
							{h.x, -h.y, -h.z},
							{h.x, -h.y, h.z},
							{-h.x, -h.y, h.z},
							{-h.x, h.y, -h.z},
						};
						b3HullData *hull = b3CreateHull(points, 5, 5);
						b3CreateHullShape(partId, &partShapeDef, hull);
						b3DestroyHull(hull);
						break;
					}
					case Enums::PartType::Ball: {
						b3Sphere partSphere = b3Sphere{
							.center = {0, 0, 0}, .radius = fmin(fmin(part->Size.x, part->Size.y), part->Size.z) * 0.5f
						};
						b3CreateSphereShape(partId, &partShapeDef, &partSphere);
						break;
					}
					case Enums::PartType::Cylinder: {
						b3HullData *cylinderHullData = b3CreateCylinder(
							part->Size.y, fmin(part->Size.x * 0.5f, part->Size.z * 0.5f), 0, 20
						); // idk 20 sides seems fine
						b3CreateHullShape(partId, &partShapeDef, cylinderHullData);
						b3DestroyHull(cylinderHullData);
						break;
					}
					};
				} else {
					b3BoxHull partBox = b3MakeBoxHull(part->Size.x * 0.5f, part->Size.y * 0.5f, part->Size.z * 0.5f);
					b3CreateHullShape(partId, &partShapeDef, &partBox.base);
				}

				this->PartBodies[part.get()] = partId;
			} else if (instance->IsA("WeldConstraint")) { // specific to weldconstraints, cuz it has specific properties
														  // for the welded parts and doesn't just use the attachments'
														  // parents
				std::shared_ptr<WeldConstraint> weldconst = std::static_pointer_cast<WeldConstraint>(instance);

				b3WeldJointDef welddef = b3DefaultWeldJointDef();
				welddef.base.bodyIdA = PartBodies.at(weldconst->Part0.Cast<BasePart>());
				welddef.base.bodyIdB = PartBodies.at(weldconst->Part1.Cast<BasePart>());
				welddef.base.collideConnected = true; // i mean i guess bro
				b3JointId jointid = b3CreateWeldJoint(World, &welddef);

				this->Constraints[weldconst.get()] = jointid;
			}
		};

		auto checkChildRemoved = [this](Instance::Pointer instance) {
			if (instance->IsA("BasePart")) {
				std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
				erase(Parts, part);
				b3DestroyBody(this->PartBodies[part.get()]);
				this->PartBodies.erase(part.get());
			} else if (instance->IsA("Constraint")) {
				std::shared_ptr<Constraint> constraint = std::static_pointer_cast<Constraint>(instance);
				b3DestroyJoint(this->Constraints[constraint.get()], true);
				this->Constraints.erase(constraint.get());
			}
		};

		DescendantAdded->Connect(checkChildAdded);
		DescendantRemoved->Connect(checkChildRemoved);
	};

	void WorldRoot::StepPhys(float deltaTime) {
		Accumulator += deltaTime;

		int steps = 0;
		while (Accumulator >= TimeStep && steps < MAX_STEPS_PER_FRAME) {
			b3World_Step(World, TimeStep, SubStepCount);
			b3BodyEvents events = b3World_GetBodyEvents(World);
			b3ContactEvents touchevents = b3World_GetContactEvents(World);
			for (int i = 0; i < events.moveCount; ++i) {
				const b3BodyMoveEvent &move = events.moveEvents[i];
				BasePart *part = static_cast<BasePart *>(move.userData);
				if (part == nullptr) continue;
				part->CFrame = gargantuan::CFrame(
					FromBox3(move.transform.p), glm::mat3_cast(FromBox3(move.transform.q))
				);
			}
			for (int i = 0; i < touchevents.beginCount; ++i) {
				const b3ContactBeginTouchEvent &begin = touchevents.beginEvents[i];
				BasePart *partA = static_cast<BasePart *>(b3Body_GetUserData(b3Shape_GetBody(begin.shapeIdA)));
				BasePart *partB = static_cast<BasePart *>(b3Body_GetUserData(b3Shape_GetBody(begin.shapeIdB)));
				if (partA == nullptr) continue;
				if (partB == nullptr) continue;
				partA->Touched->Fire(partB);
				partB->Touched->Fire(partA);
			}
			for (int i = 0; i < touchevents.endCount; ++i) {
				const b3ContactEndTouchEvent &end = touchevents.endEvents[i];
				BasePart *partA = static_cast<BasePart *>(b3Body_GetUserData(b3Shape_GetBody(end.shapeIdA)));
				BasePart *partB = static_cast<BasePart *>(b3Body_GetUserData(b3Shape_GetBody(end.shapeIdB)));
				if (partA == nullptr) continue;
				if (partB == nullptr) continue;
				partA->TouchEnded->Fire(partB);
				partB->TouchEnded->Fire(partA);
			}
			Accumulator -= TimeStep;
			++steps;
		}

		if (steps == MAX_STEPS_PER_FRAME) Accumulator = 0.0f;
	}

	void WorldRoot::ApplyImpulse(BasePart *part, glm::vec3 force) {
		auto it = PartBodies.find(part);
		if (it == PartBodies.end()) return;
		b3Body_ApplyLinearImpulseToCenter(it->second, ToBox3(force), true);
	}

	void WorldRoot::KillWorld() {
		b3DestroyWorld(World);
	}
} // namespace gargantuan
