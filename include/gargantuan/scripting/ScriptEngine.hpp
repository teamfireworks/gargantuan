#pragma once

#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/scripting/ThreadEngine.hpp"

#include <Luau/Compiler.h>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace gargantuan {
	class Script;
	class ModuleScript;

	int OpenLibBase(lua_State *L);
	int OpenLibRequire(lua_State *L);
	int OpenLibTask(lua_State *L);

	int OpenLibAxes(lua_State *L);
	int OpenLibCFrame(lua_State *L);
	int OpenLibColor3(lua_State *L);
	int OpenLibColorSequence(lua_State *L);
	int OpenLibColorSequenceKeypoint(lua_State *L);
	int OpenLibEnum(lua_State *L);
	int OpenLibFaces(lua_State *L);
	int OpenLibFont(lua_State *L);
	int OpenLibInstance(lua_State *L);
	int OpenLibNumberRange(lua_State *L);
	int OpenLibNumberSequence(lua_State *L);
	int OpenLibNumberSequenceKeypoint(lua_State *L);
	int OpenLibRandom(lua_State *L);
	int OpenLibRaycastParams(lua_State *L);
	int OpenLibRect(lua_State *L);
	int OpenLibRegion3(lua_State *L);
	int OpenLibSignal(lua_State *L);
	int OpenLibTweenInfo(lua_State *L);
	int OpenLibUDim(lua_State *L);
	int OpenLibUDim2(lua_State *L);
	int OpenLibVector2(lua_State *L);
	int OpenLibVector3(lua_State *L);

	class ScriptEngine {
	  public:
		ScriptEngine(std::shared_ptr<DataModel> game);
		~ScriptEngine();

		lua_State *L = nullptr;
		ThreadEngine Threads;
		lua_CompileOptions CompileOptions;
		std::unordered_set<std::shared_ptr<Script>> ScriptQueue;

		std::shared_ptr<gargantuan::DataModel> DataModel;
		std::shared_ptr<Instance> RequireCurrentInstance = nullptr;
		std::unordered_map<std::string, std::shared_ptr<Instance>> RequirePathCache;
		std::shared_ptr<Instance> FindRequiredInstanceByPath(const char *path);

		void Step();

		static ScriptEngine *Get(lua_State *L);
		static void DumpStack(lua_State *L);
	};
}
