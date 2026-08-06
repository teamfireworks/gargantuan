#pragma once

#include "gargantuan/datatypes/instances/InstanceClassDefinition.hpp"
#include "gargantuan/datatypes/instances/InstanceProperty.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#define I_DECL(classType)                                                                                              \
  public:                                                                                                              \
	static const InstanceClassDefinition CLASS_DEFINITION;

#define I_PROP(propertyName, propertyType, ...)                                                                        \
	inline static InstanceProperty<propertyType> propertyName = ([]() {                                                \
		static auto property = CreateInstanceProperty<propertyType>({                                                  \
			.Name = #propertyName,                                                                                     \
			__VA_ARGS__,                                                                                               \
		});                                                                                                            \
		return property;                                                                                               \
	})();

namespace gargantuan {
	G_SHARED_USERDATA_DECL(
		Instance,

		I_DECL(Instance);

		Instance *ParentPointer = nullptr;
		void SetParent(std::shared_ptr<Instance> newParent);
		std::vector<std::shared_ptr<Instance>> Children;

		I_PROP(Archivable, bool, .Preset = InstancePropertyPreset::Value, .Default = true);
		I_PROP(Name, std::string, .Preset = InstancePropertyPreset::Value, .Default = CLASS_DEFINITION.ClassName);
		I_PROP(
			Destroyed,
			bool,
			.Preset = InstancePropertyPreset::Value,
			.Default = false,
			.WritePermission = Enums::Permission::Engine
		);
		I_PROP(ClassName, std::string, .Preset = InstancePropertyPreset::Unimplemented);
		I_PROP(Parent, std::optional<std::shared_ptr<Instance>>, .Preset = InstancePropertyPreset::Unimplemented);

		typedef std::tuple<std::shared_ptr<Instance>, std::shared_ptr<Instance>> AncestryChangedArguments;
		G_SIGNAL(ChildAdded, std::shared_ptr<Instance>);
		G_SIGNAL(ChildRemoved, std::shared_ptr<Instance>);
		G_SIGNAL(DescendantAdded, std::shared_ptr<Instance>);
		G_SIGNAL(DescendantRemoved, std::shared_ptr<Instance>);
		G_SIGNAL(AncestryChanged, AncestryChangedArguments);
		G_SIGNAL(Destroying, std::monostate);

		InstanceProperty<std::any> * FindProperty(std::string_view name);
		const Self::Method *FindMethod(std::string_view name);

		static int LIndex(lua_State *L, Instance *instance);
		static int LNewIndex(lua_State *L, Instance *instance);
		static int LNamecall(lua_State *L, Instance *instance);

		std::string GetFullName();
		bool IsA(std::string_view className);
		std::vector<std::shared_ptr<Instance>> & GetChildren();
		std::vector<std::shared_ptr<Instance>> GetDescendants();
		void ClearAllChildren();
		std::shared_ptr<Instance> FindFirstChild(std::string_view name, bool recursive = false);
		std::shared_ptr<Instance> FindFirstChildOfClass(std::string_view className);
		std::shared_ptr<Instance> FindFirstChildWhichIsA(std::string_view className);
		std::shared_ptr<Instance> FindFirstDescendant(std::string_view name);
		std::shared_ptr<Instance> FindFirstDescendantOfClass(std::string_view className);
		std::shared_ptr<Instance> FindFirstDescendantWhichIsA(std::string_view className);
		void Destroy();

		template <typename T>
		bool IsClass() const { return dynamic_cast<const T *>(this) != nullptr; }

		template <typename T>
		T *Cast() const { return dynamic_cast<const T *>(this); }

		template <typename T>
		T *Cast() { return dynamic_cast<T *>(this); }

		template <typename T>
		const T *Cast() const { return dynamic_cast<const T *>(this); }

		private : void CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants);
		void FireAncestryChanged(std::shared_ptr<Instance> child, std::shared_ptr<Instance> parent);
		void AssertNotDestroyed();
	);
}
