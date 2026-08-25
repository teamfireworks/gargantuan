#include <lua.h>
#include <lualib.h>

void test() {
  auto L = luaL_newstate();

  lua_pushcclosure(L, [](lua_State *L) -> int {}, "debugname", 0);
}
