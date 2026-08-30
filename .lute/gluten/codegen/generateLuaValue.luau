-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const function generateLuaValue() end

-- Instances, Signals -> std::shared_ptr
-- Color3s --> gg_Color3

-- how old codebase did it vvv
-- #define G_SHARED_USERDATA_DECL(classType, ...)                                                                         \
-- 	class classType : public std::enable_shared_from_this<classType>,                                                  \
-- 					  public Userdata<classType, std::shared_ptr<classType>> {                                         \
-- 	  public:                                                                                                          \
-- 		static const Definition DEFINITION;                                                                            \
-- 		__VA_ARGS__                                                                                                    \
-- 	};                                                                                                                 \
-- 	template <> struct StackValue<std::shared_ptr<classType>> {                                                        \
-- 		typedef Userdata<classType, std::shared_ptr<classType>> This;                                                  \
-- 		static inline std::string_view ReflectedTypedef() {                                                            \
-- 			return StackValue<This>::ReflectedTypedef();                                                               \
-- 		}                                                                                                              \
-- 		static bool Is(lua_State *L, int idx) {                                                                        \
-- 			return StackValue<This>::Is(L, idx);                                                                       \
-- 		}                                                                                                              \
-- 		static std::shared_ptr<classType> From(lua_State *L, int idx) {                                                \
-- 			return StackValue<This>::From(L, idx);                                                                     \
-- 		}                                                                                                              \
-- 		static int Push(lua_State *L, std::shared_ptr<classType> value) {                                              \
-- 			return StackValue<This>::Push(L, value);                                                                   \
-- 		}                                                                                                              \
-- 	};

return generateLuaValue
