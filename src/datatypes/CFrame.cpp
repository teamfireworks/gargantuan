#define GLM_ENABLE_EXPERIMENTAL

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Vector3.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/quaternion.hpp>
#include <lua.h>
#include <lualib.h>

#include <tuple>

namespace gargantuan {
	G_USERDATA_IMPL(
		CFrame,
		.Tag = UserdataTag::CFrame,
		.Type = "CFrame",

		.Properties =
			//
		{
			{"Position", Property::fromReadonlyMember<&CFrame::Position>()},
			{"Rotation", Property::fromRead([](CFrame *self) { return CFrame(glm::vec3(), self->Rotation); })},
			{"X", Property::fromRead([](CFrame *self) { return self->Position.x; })},
			{"Y", Property::fromRead([](CFrame *self) { return self->Position.y; })},
			{"Z", Property::fromRead([](CFrame *self) { return self->Position.z; })},
			{"XVector", Property::fromRead([](CFrame *self) { return self->GetRightVector(); })},
			{"RightVector", Property::fromRead([](CFrame *self) { return self->GetRightVector(); })},
			{"YVector", Property::fromRead([](CFrame *self) { return self->GetUpVector(); })},
			{"UpVector", Property::fromRead([](CFrame *self) { return self->GetUpVector(); })},
			{"ZVector", Property::fromRead([](CFrame *self) { return self->GetLookVector(); })},
			{"LookVector", Property::fromRead([](CFrame *self) { return self->GetLookVector(); })},
		},

		.Methods =
			//
		{
			{"Inverse", Method::fromMember<&CFrame::Inverse>()},
			{"Lerp", Method::fromMember<&CFrame::Lerp>()},
			{"Orthonormalize", Method::fromMember<&CFrame::Orthonormalize>()},
			{"ToWorldSpace", Method::fromMember<&CFrame::ToWorldSpace>()},
			{"ToObjectSpace", Method::fromMember<&CFrame::ToObjectSpace>()},
			{"PointToWorldSpace", Method::fromMember<&CFrame::PointToWorldSpace>()},
			{"PointToObjectSpace", Method::fromMember<&CFrame::PointToObjectSpace>()},
			{"VectorToWorldSpace", Method::fromMember<&CFrame::VectorToWorldSpace>()},
			{"VectorToObjectSpace", Method::fromMember<&CFrame::VectorToObjectSpace>()},
			{"GetComponents", Method::fromMember<&CFrame::GetComponents>()},
			{"ToEulerAngles", Method::fromMember<&CFrame::ToEulerAngles>()},
			{"ToEulerAnglesXYZ", Method::fromMember<&CFrame::ToEulerAnglesXYZ>()},
			{"ToEulerAnglesYXZ", Method::fromMember<&CFrame::ToEulerAnglesYXZ>()},
			{"ToOrientation", Method::fromMember<&CFrame::ToOrientation>()},
			{"ToAxisAngle", Method::fromMember<&CFrame::ToAxisAngle>()},
			{"FuzzyEq", Method::fromMember<&CFrame::FuzzyEq>()},
			{"AngleBetween", Method::fromMember<&CFrame::AngleBetween>()},
			{"__add", Method{CFrame::LAdd}},
			{"__sub", Method{CFrame::LSub}},
			{"__mul", Method{CFrame::LMul}},
			{"__tostring", Method{CFrame::LTostring}},
		}
	);

	CFrame::CFrame() : Position(0.0f, 0.0f, 0.0f), Rotation(CFrame::DEFAULT_ROTATION) {};
	CFrame::CFrame(glm::vec3 position) : Position(position), Rotation(CFrame::DEFAULT_ROTATION) {};
	CFrame::CFrame(float x, float y, float z) : Position(x, y, z), Rotation(CFrame::DEFAULT_ROTATION) {};
	CFrame::CFrame(glm::vec3 position, glm::vec3 target)
		: Position(position), Rotation(BuildLookRotation(position, target)) {};
	CFrame::CFrame(glm::vec3 position, glm::mat3 rotation) : Position(position), Rotation(rotation) {};
	CFrame::CFrame(
		float x,
		float y,
		float z,
		float r00,
		float r01,
		float r02,
		float r10,
		float r11,
		float r12,
		float r20,
		float r21,
		float r22
	)
		: Position(x, y, z), Rotation(r00, r01, r02, r10, r11, r12, r20, r21, r22) {};

	CFrame CFrame::lookAt(glm::vec3 at, glm::vec3 lookAtPos, glm::vec3 up) {
		return CFrame(at, BuildLookRotation(at, lookAtPos, up));
	}

	CFrame CFrame::lookAlong(glm::vec3 at, glm::vec3 direction, glm::vec3 up) {
		if (glm::length2(direction) < CF_EPSILON * CF_EPSILON) {
			return CFrame(at);
		}
		return CFrame::lookAt(at, at + direction, up);
	}

	CFrame CFrame::fromRotationBetweenVectors(glm::vec3 from, glm::vec3 to) {
		glm::vec3 uFrom = SafeUnit(from, glm::vec3(0, 0, -1));
		glm::vec3 uTo = SafeUnit(to, glm::vec3(0, 0, -1));
		glm::quat q = glm::rotation(uFrom, uTo);
		return CFrame(glm::vec3(0.0f), glm::mat3_cast(q));
	}

	CFrame CFrame::fromEulerAngles(float rx, float ry, float rz, Enums::RotationOrder order) {
		switch (order) {
		case Enums::RotationOrder::XYZ:
			return CFrame::fromEulerAnglesXYZ(rx, ry, rz);
		case Enums::RotationOrder::YXZ:
			return CFrame::fromEulerAnglesYXZ(rx, ry, rz);
		default:
			return CFrame::fromEulerAnglesYXZ(rx, ry, rz);
		}
	}

	CFrame CFrame::fromEulerAnglesXYZ(float rx, float ry, float rz) {
		return CFrame(glm::vec3(0.0f), glm::mat3(glm::eulerAngleXYZ(rx, ry, rz)));
	}

	CFrame CFrame::fromEulerAnglesYXZ(float rx, float ry, float rz) {
		return CFrame(glm::vec3(0.0f), glm::mat3(glm::eulerAngleYXZ(ry, rx, rz)));
	}

	CFrame CFrame::Angles(float x, float y, float z) {
		return CFrame(glm::vec3(0, 0, 0), glm::mat3(glm::eulerAngleXYZ(x, y, z)));
	}

	CFrame CFrame::fromAxisAngle(glm::vec3 v, float r) {
		glm::vec3 axis = SafeUnit(v, glm::vec3(0, 1, 0));
		glm::quat q = glm::angleAxis(r, axis);
		return CFrame(glm::vec3(0.0f), glm::mat3_cast(q));
	}

	CFrame CFrame::fromOrientation(float rx, float ry, float rz) {
		return CFrame::fromEulerAnglesYXZ(rx, ry, rz);
	}

	CFrame CFrame::fromMatrix(glm::vec3 position, glm::vec3 x, glm::vec3 y, glm::vec3 z) {
		return CFrame(position, glm::mat3(x, y, z));
	}

	CFrame CFrame::fromQuaternion(float x, float y, float z, float w, glm::vec3 position) {
		glm::quat q = glm::normalize(glm::quat(w, x, y, z));
		return CFrame(position, glm::mat3_cast(q));
	}

	glm::vec3 CFrame::GetRightVector() const {
		return Rotation[0];
	}

	glm::vec3 CFrame::GetUpVector() const {
		return Rotation[1];
	}

	glm::vec3 CFrame::GetLookVector() const {
		return -Rotation[2];
	}

	CFrame CFrame::Inverse() const {
		glm::mat3 invertedRotation = glm::transpose(Rotation);
		return CFrame(-invertedRotation * Position, invertedRotation);
	};

	CFrame CFrame::Lerp(const CFrame &goal, double alpha) const {
		float a = static_cast<float>(alpha);

		glm::vec3 pos = glm::mix(Position, goal.Position, a);

		glm::quat q1 = ToQuaternion();
		glm::quat q2 = goal.ToQuaternion();
		glm::quat qRot = glm::slerp(q1, q2, a);

		return CFrame(pos, glm::mat3_cast(qRot));
	};

	CFrame CFrame::Orthonormalize() const {
		return CFrame(Position, glm::orthonormalize(Rotation));
	}

	CFrame CFrame::ToWorldSpace(const CFrame &cf) const {
		return *this * cf;
	}

	CFrame CFrame::ToObjectSpace(const CFrame &cf) const {
		return this->Inverse() * cf;
	}

	glm::vec3 CFrame::PointToWorldSpace(const glm::vec3 &point) const {
		return *this * point;
	}

	glm::vec3 CFrame::PointToObjectSpace(const glm::vec3 &point) const {
		return Inverse() * point;
	}

	glm::vec3 CFrame::VectorToWorldSpace(const glm::vec3 &vector) const {
		return Rotation * vector;
	}

	glm::vec3 CFrame::VectorToObjectSpace(const glm::vec3 &vector) const {
		return glm::transpose(Rotation) * vector;
	}

	CFrame::Components CFrame::GetComponents() const {
		return Components(
			Position.x,
			Position.y,
			Position.z,
			Rotation[0][0],
			Rotation[0][1],
			Rotation[0][2],
			Rotation[1][0],
			Rotation[1][1],
			Rotation[1][2],
			Rotation[2][0],
			Rotation[2][1],
			Rotation[2][2]
		);
	}

	std::tuple<double, double, double> CFrame::ToEulerAngles(Enums::RotationOrder order) {
		// TODO: We only implement a subset rn
		switch (order) {
		case Enums::RotationOrder::XYZ:
			return ToEulerAnglesXYZ();
		case Enums::RotationOrder::YXZ:
			return ToEulerAnglesYXZ();
		default:
			return ToEulerAnglesYXZ();
		}
	}

	std::tuple<double, double, double> CFrame::ToEulerAnglesXYZ() const {
		float x, y, z;
		glm::extractEulerAngleXYZ(glm::mat4(Rotation), x, y, z);
		return {x, y, z};
	}

	std::tuple<double, double, double> CFrame::ToEulerAnglesYXZ() const {
		float x, y, z;
		glm::extractEulerAngleYXZ(glm::mat4(Rotation), y, x, z);
		return {x, y, z};
	}

	std::tuple<double, double, double> CFrame::ToOrientation() const {
		return ToEulerAnglesYXZ();
	}

	std::tuple<glm::vec3, double> CFrame::ToAxisAngle() const {
		glm::quat q = ToQuaternion();
		if (std::abs(q.w) > 1.0f) q = glm::normalize(q);

		float angle = glm::angle(q);
		glm::vec3 axis = glm::axis(q);

		return {axis, angle};
	}

	bool CFrame::FuzzyEq(const CFrame &other, double epsilon) const {
		bool isPositionEqual = glm::all(glm::epsilonEqual(Position, other.Position, static_cast<float>(epsilon)));
		if (!isPositionEqual) return false;

		glm::quat q1 = ToQuaternion();
		glm::quat q2 = other.ToQuaternion();
		return glm::abs(glm::dot(q1, q2)) >= (1.0 - epsilon);
	}

	double CFrame::AngleBetween(const CFrame &other) const {
		glm::quat q1 = ToQuaternion();
		glm::quat q2 = other.ToQuaternion();

		float dot = std::abs(glm::dot(q1, q2));
		dot = glm::clamp(dot, -1.0f, 1.0f);

		return 2.0 * std::acos(dot);
	}

	glm::quat CFrame::ToQuaternion() const {
		return glm::quat_cast(Rotation);
	}

	int CFrame::LAdd(lua_State *L, CFrame *self) {
		if (lua_isvector(L, 2)) {
			auto vec = StackValue<glm::vec3>::From(L, 2);
			StackValue<CFrame>::Push(L, CFrame(self->Position + vec, self->Rotation));
			return 1;
		} else {
			luaL_typeerror(L, 2, "Vector3");
			return 0;
		}
	}

	int CFrame::LSub(lua_State *L, CFrame *self) {
		if (lua_isvector(L, 2)) {
			auto vec = StackValue<glm::vec3>::From(L, 2);
			StackValue<CFrame>::Push(L, CFrame(self->Position - vec, self->Rotation));
			return 1;
		} else {
			luaL_typeerror(L, 2, "Vector3");
			return 0;
		}
	}

	int CFrame::LMul(lua_State *L, CFrame *self) {
		if (lua_isvector(L, 2)) {
			auto other = StackValue<glm::vec3>::From(L, 2);
			StackValue<glm::vec3>::Push(L, *self * other);
		} else if (StackValue<CFrame>::Is(L, 2)) {
			auto other = StackValue<CFrame>::From(L, 2);
			StackValue<CFrame>::Push(L, *self * other);
		} else {
			luaL_typeerror(L, 2, "Vector3 or CFrame");
			return 0;
		}

		return 1;
	}

	int CFrame::LTostring(lua_State *L, CFrame *self) {
		lua_pushfstringL(
			L,
			"%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f",
			self->Position.x,
			self->Position.y,
			self->Position.z,
			self->Rotation[0][0],
			self->Rotation[0][1],
			self->Rotation[0][2],
			self->Rotation[1][0],
			self->Rotation[1][1],
			self->Rotation[1][2],
			self->Rotation[2][0],
			self->Rotation[2][1],
			self->Rotation[2][2]
		);
		return 1;
	}

	glm::vec3 CFrame::SafeUnit(glm::vec3 vec, glm::vec3 fallback) {
		float lenSq = glm::length2(vec);
		if (lenSq <= CF_EPSILON * CF_EPSILON) {
			return fallback;
		}
		return glm::normalize(vec);
	}

	glm::mat3 CFrame::BuildLookRotation(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
		glm::vec3 dir = target - position;
		float lenSq = glm::dot(dir, dir);
		if (lenSq < 1e-8f) {
			return DEFAULT_ROTATION;
		}

		glm::vec3 z = -glm::normalize(dir);

		if (glm::abs(glm::dot(up, z)) > 0.999f) {
			up = glm::vec3(0, 0, 1);
		}

		glm::vec3 x = glm::normalize(glm::cross(up, z));
		glm::vec3 y = glm::cross(z, x);

		return glm::mat3(x, y, z);
	}
}
