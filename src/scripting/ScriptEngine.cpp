#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/Script.hpp"
#include "gargantuan/datatypes/Axes.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Enum.hpp"
#include "gargantuan/datatypes/Random.hpp"
#include "gargantuan/datatypes/Rect.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/datatypes/UDim.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/scripting/ThreadEngine.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <Luau/Common.h>
#include <Luau/Compiler.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <cstring>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <stdexcept>
#include <vector>

namespace gargantuan {

	struct Lib {
		std::string Label;
		std::function<void(lua_State *L)> Open = nullptr;
		std::function<void(lua_State *L)> CreateUserdataMetatable = nullptr;
	};

	static const Lib SCRIPT_LIBS[] = {
		{"Axes", OpenLibAxes, Axes::CreateUserdataMetatable},
		{"CFrame", OpenLibCFrame, CFrame::CreateUserdataMetatable},
		{"Color3", OpenLibColor3, Color3::CreateUserdataMetatable},
		// {"ColorSequence", OpenLibColorSequence, ColorSequence::CreateUserdataMetatable},
		// {"ColorSequenceKeypoint", OpenLibColorSequenceKeypoint, ColorSequenceKeypoint::CreateUserdataMetatable},
		{"Enum", OpenLibEnum, Enum::CreateUserdataMetatable},
		{"EnumItem", nullptr, EnumItem::CreateUserdataMetatable},
		// {"Faces", OpenLibFaces, Faces::CreateUserdataMetatable},
		// {"Font", OpenLibFont, Font::CreateUserdataMetatable},
		{"Instance", OpenLibInstance, Instance::CreateUserdataMetatable},
		// {"NumberRange", OpenLibNumberRange, NumberRange::CreateUserdataMetatable},
		// {"NumberSequence", OpenLibNumberSequence, NumberSequence::CreateUserdataMetatable},
		// {"NumberSequenceKeypoint", OpenLibNumberSequenceKeypoint, NumberSequenceKeypoint::CreateUserdataMetatable},
		{"Random", OpenLibRandom, Random::CreateUserdataMetatable},
		// {"RaycastParams", OpenLibRaycastParams},
		{"Rect", nullptr, Rect::CreateUserdataMetatable},
		// {"Region3", OpenLibRegion3, Region3::CreateUserdataMetatable},
		{"UDim", OpenLibUDim, UDim::CreateUserdataMetatable},
		// {"UDim2", OpenLibUDim2, UDim2::CreateUserdataMetatable},
		{"Signal", OpenLibSignal, BaseSignal::CreateUserdataMetatable},
		{"SignalConnection", nullptr, SignalConnection::CreateUserdataMetatable},
		{"TweenInfo", OpenLibTweenInfo, TweenInfo::CreateUserdataMetatable},
		{"Vector2", OpenLibVector2, Vector2::CreateUserdataMetatable},
		{"Vector3", OpenLibVector3},

		{"Base", OpenLibBase},
		{"Require", OpenLibRequire},
		{"Task", OpenLibTask},
	};

	static thread_local lua_State *CurrentState = nullptr;

	ScriptEngine::ScriptEngine(std::shared_ptr<gargantuan::DataModel> game)
		: L(luaL_newstate()), Threads(L), DataModel(game) {
		if (L == nullptr) {
			throw std::runtime_error("Failed to instantiate Luau VM");
		}

		CurrentState = L;
		Luau::assertHandler() = [](const char *expression, const char *file, int line, const char *function) -> int {
			LOG_CRITICAL(Lua, "Internal assertion failed: %s", expression);
			LOG_CRITICAL(Lua, "In: %s:%d in %s", file, line, function);
			if (CurrentState) ScriptEngine::DumpStack(CurrentState);
			assert(false);
			return 1;
		};

		lua_pushstring(L, "gargantuan::ScriptEngine");
		lua_pushlightuserdata(L, this);
		lua_settable(L, LUA_REGISTRYINDEX);

		luaL_openlibs(L);
		LOG_INFO(App, "ScriptEngine started opening libraries");
		for (const auto &[name, open, metatable] : SCRIPT_LIBS) {
			LOG_DEBUG(App, "Opening library %s", name.c_str());
			if (metatable) metatable(L);
			if (open) open(L);
		}
		LOG_INFO(App, "ScriptEngine finished opening libraries");

		CompileOptions = lua_CompileOptions{
			.vectorLib = "Vector3",
			.vectorCtor = "new",
			.vectorType = "Vector3",
		};
	}

	void ScriptEngine::DumpStack(lua_State *L) {
		int stackSize = lua_gettop(L);

		SDL_Log("Dumping Lua stack of %d values", stackSize);

		for (int i = stackSize; i >= 1; --i) {
			int type = lua_type(L, i);

			switch (type) {
			case LUA_TNIL:
				SDL_Log("[%d] Nil", i);
				break;
			case LUA_TBOOLEAN:
				SDL_Log("[%d] Boolean: %s", i, lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				SDL_Log("[%d] Number: %g", i, lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				SDL_Log("[%d] String: %s", i, lua_tostring(L, i));
				break;
			case LUA_TTABLE:
				SDL_Log("[%d] Table", i);
				break;
			case LUA_TFUNCTION:
				SDL_Log("[%d] Function", i);
				break;
			case LUA_TUSERDATA: {
				auto tag = lua_userdatatag(L, i);
				if (tag != -1 || tag != (int)UserdataTag::Invalid) {
					auto tagName = magic_enum::enum_name((UserdataTag)tag);
					SDL_Log("[%d] Userdata (%d)%s", i, tag, tagName.data());
				} else {
					SDL_Log("[%d] Userdata (Not tagged)", i);
				}
				break;
			}
			case LUA_TTHREAD:
				SDL_Log("[%d] Thread", i);
				break;
			case LUA_TLIGHTUSERDATA:
				SDL_Log("[%d] Light Userdata", i);
				break;
			default:
				SDL_Log("[%d] Unknown", i);
				break;
			}
		}
	}

	ScriptEngine::~ScriptEngine() {
		if (L) {
			lua_close(L);
			L = nullptr;
			CurrentState = nullptr;
		}
	}

	ScriptEngine *ScriptEngine::Get(lua_State *L) {
		lua_pushstring(L, "gargantuan::ScriptEngine");
		lua_gettable(L, LUA_REGISTRYINDEX);

		auto *engine = static_cast<ScriptEngine *>(lua_tolightuserdata(L, -1));
		lua_pop(L, 1);

		if (!engine) luaL_error(L, "Missing gargantuan::ScriptEngine (Internal error)");
		return engine;
	}

	std::shared_ptr<Instance> ScriptEngine::FindRequiredInstanceByPath(const char *rawPath) {
		LOG_INFO(App, "Attempting to find required instance %s", rawPath);

		if (!rawPath || !DataModel || std::strcmp(rawPath, "\0") == 0) return nullptr;

		std::string path(rawPath);
		std::vector<const char *> segments;
		std::string currentSegment;
		for (char &currentCharacter : path) {
			if (currentCharacter == '/' || currentCharacter == '\\') {
				segments.emplace_back(currentSegment.c_str());
				currentSegment.clear();
			} else {
				currentSegment += currentCharacter;
			}
		}

		std::shared_ptr<Instance> currentInstance;
		return nullptr;
	}

	void ScriptEngine::Step() {
		Threads.Step();

		for (auto it = ScriptQueue.begin(); it != ScriptQueue.end();) {
			auto script = *it;
			auto status = script->Step(L);

			switch (status) {
			case ScriptStatus::Error:
				LOG_CRITICAL(App, "%s", script->ErrorMessage.c_str());
				[[fallthrough]];

			case ScriptStatus::Finished:
			case ScriptStatus::Disabled:
			case ScriptStatus::Yielded:
			case ScriptStatus::Running:
				it = ScriptQueue.erase(it);
				break;

			default:
				++it;
				break;
			}
		}
	}
} // namespace gargantuan
