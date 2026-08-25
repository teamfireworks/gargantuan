#include "datatypes/gg_Color3.hpp"

#include <lua.h>

// this would be codegen but i needa rough out the output code
void RegisterColor3(lua_State *L) {
  lua_newtable(L);

  lua_pushliteral(L, "Color3");
  lua_setfield(L, -2, "__type");

  lua_CFunction luaColor3__tostring = [](lua_State *L) -> int {
    auto value = (gg_Color3 *)lua_touserdatatagged(L, 1, 1);
    lua_pushfstring(L, "%f, %f, %f", value->R, value->G, value->B);
    return 1;
  };
  lua_pushcclosurek(L, luaColor3__tostring, "Color3.__tostring", 0, nullptr);
  lua_setfield(L, -2, "__tostring");

  lua_setreadonly(L, -1, true);

  lua_pushvalue(L, -1);
  lua_setglobal(L, "Color3");
  lua_setuserdatatag(L, -1, 1);
}
