/* ====================================================================
* Module: num64
* Version: 0.1
* Author: Adalberto Amorim Felipe
* Descrição: Lib num: int64 e uint64 for Prisma lang, v1
* ==================================================================== */
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define P_MODULE "num64"

/* ========== C INCLUDES ==============*/
/* ====================================*/

#include "runtime.c"
#include "c_raw.c"
#include "raw_funcs.c"

#include "metamethods.c"
#include "metatables.c"

#include "regfuncs.c"

static int luaopen_num64(lua_State *L){
    luaL_newlib(L, num64_funcs);
    int mod_idx = lua_gettop(L);
    registrar_meta_int64_t(L, mod_idx);
    registrar_meta_uint64_t(L, mod_idx);
    lua_pushliteral(L, "0.1");
    lua_setfield(L, -2, "VER");
    lua_pushliteral(L, "Adalberto Amorim Felipe");
    lua_setfield(L, -2, "AUTOR");
    lua_pushliteral(L, "Lib num: int64 e uint64 for Prisma lang, v1");
    lua_setfield(L, -2, "INFO");
    return 1;
}
