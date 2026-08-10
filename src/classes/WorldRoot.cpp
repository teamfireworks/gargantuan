#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/Part.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/physics/Conversions.hpp"

#include <SDL3/SDL.h>

#include <box3d/box3d.h>
#include <box3d/collision.h>
#include <box3d/math_functions.h>
#include <box3d/types.h>
#include <cstddef>
#include <memory>
#include <unordered_map>

namespace gargantuan {
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

		auto checkChildAdded = [this](std::shared_ptr<Instance> instance) {
			if (auto part = std::dynamic_pointer_cast<BasePart>(instance)) {
				this->Parts.push_back(part);

				b3BodyDef partBodyDef = b3DefaultBodyDef();
				partBodyDef.position = ToBox3(part->GetCFrame().Position);
				partBodyDef.rotation = ToBox3(part->GetCFrame().ToQuaternion());
				b3ShapeDef partShapeDef = b3DefaultShapeDef();
				if (part->GetAnchored() == true) {
					partBodyDef.type = (b3BodyType)b3_staticBody;
				} else {
					partBodyDef.type = (b3BodyType)b3_dynamicBody;
					partShapeDef.density = 0.7f;
				};

				if (part->GetCanCollide() == false) {
					partShapeDef.isSensor = true;
					partShapeDef.enableSensorEvents = true; // CanTouch
				}

				partBodyDef.userData = part.get();
				b3BodyId partId = b3CreateBody(World, &partBodyDef);
				// hull
				if (const Part *partNotBasePart = dynamic_cast<Part *>(part.get())) {
					switch (partNotBasePart->GetShape()) {
					case Enums::PartType::Block: {
						b3BoxHull partBox = b3MakeBoxHull(
							part->GetSize().x * 0.5f, part->GetSize().y * 0.5f, part->GetSize().z * 0.5f
						);
						b3CreateHullShape(partId, &partShapeDef, &partBox.base);
						break;
					}
					case Enums::PartType::Wedge: {
						const glm::vec3 h = part->GetSize() * 0.5f;
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
						const glm::vec3 h = part->GetSize() * 0.5f;
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
							.center = {0, 0, 0},
							.radius = fmin(fmin(part->GetSize().x, part->GetSize().y), part->GetSize().z) * 0.5f
						};
						b3CreateSphereShape(partId, &partShapeDef, &partSphere);
						break;
					}
					case Enums::PartType::Cylinder: {
						b3HullData *cylinderHullData = b3CreateCylinder(
							part->GetSize().y, fmin(part->GetSize().x * 0.5f, part->GetSize().z * 0.5f), 0, 20
						); // idk 20 sides seems fine
						b3CreateHullShape(partId, &partShapeDef, cylinderHullData);
						b3DestroyHull(cylinderHullData);
						break;
					}
					};
				} else {
					b3BoxHull partBox = b3MakeBoxHull(
						part->GetSize().x * 0.5f, part->GetSize().y * 0.5f, part->GetSize().z * 0.5f
					);
					b3CreateHullShape(partId, &partShapeDef, &partBox.base);
				}

				this->PartBodies[part.get()] = partId;
			}
		};

		auto checkChildRemoved = [this](std::shared_ptr<Instance> instance) {
			if (auto part = std::static_pointer_cast<BasePart>(instance)) {
				erase(Parts, part);
				b3DestroyBody(this->PartBodies[part.get()]);
				this->PartBodies.erase(part.get());
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
			for (int i = 0; i < events.moveCount; ++i) {
				const b3BodyMoveEvent &move = events.moveEvents[i];
				BasePart *part = static_cast<BasePart *>(move.userData);
				if (part == nullptr) continue;
				part->GetCFrame() = gargantuan::CFrame(
					FromBox3(move.transform.p), glm::mat3_cast(FromBox3(move.transform.q))
				);
			}
			Accumulator -= TimeStep;
			++steps;
		}

		if (steps == MAX_STEPS_PER_FRAME) Accumulator = 0.0f;
	}

	void WorldRoot::KillWorld() {
		b3DestroyWorld(World);
	}
}
