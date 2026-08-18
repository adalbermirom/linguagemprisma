/* ========== C RAW CODE BLOCKS =======*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#define USERDATA_INT64 "userdata<int64_t>"
static int64_t *raw_int64_new(int64_t val){
    int64_t *new = (int64_t*) calloc(1, sizeof(int64_t));
    if(!new) return NULL;
    *new = val;
    return new;
}
static int p_raw_int64_new(lua_State* L){
    int64_t val = 0;
    if(lua_isstring(L, 1)) {
        const char *str = lua_tostring(L, 1);
        char* endptr;
        errno = 0;
        val = strtoll(str, &endptr, 10);
        if (endptr == str || errno == ERANGE) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        val = (int64_t)luaL_checknumber(L, 1);
    }
    int64_t *new = raw_int64_new(val);
    if (!new) {
        lua_pushnil(L);
        return 1;
    }
    pushuserdata(L, new, USERDATA_INT64);
    return 1;
}
#define GET_AB(a, b) \
    int64_t *a = checkuserdata(L, 1, USERDATA_INT64);\
    int64_t *b = checkuserdata(L, 2, USERDATA_INT64);
#define GET_A(a) \
    int64_t *a = checkuserdata(L, 1, USERDATA_INT64);
#define OP(operator) \
    GET_AB(val_a, val_b);\
    if (!val_a || !val_b) { lua_pushnil(L); return 1; }\
    int64_t *ret = raw_int64_new(*val_a operator *val_b);\
    if (!ret) { lua_pushnil(L); return 1; }\
    pushuserdata(L, ret, USERDATA_INT64);\
    return 1;
#define USERDATA_UINT64 "userdata<uint64_t>"
static uint64_t *raw_uint64_new(uint64_t val){
    uint64_t *new = (uint64_t*) calloc(1, sizeof(uint64_t));
    if(!new) return NULL;
    *new = val;
    return new;
}
static int p_raw_uint64_new(lua_State* L){
    uint64_t val = 0;
    if(lua_isstring(L, 1)) {
        const char *str = lua_tostring(L, 1);
        char* endptr;
        errno = 0;
        val = strtoull(str, &endptr, 10);
        if (endptr == str || errno == ERANGE) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        val = (uint64_t)luaL_checknumber(L, 1);
    }
    uint64_t *new = raw_uint64_new(val);
    if (!new) {
        lua_pushnil(L);
        return 1;
    }
    pushuserdata(L, new, USERDATA_UINT64);
    return 1;
}
#define GETU_AB(a, b) \
    uint64_t *a = checkuserdata(L, 1, USERDATA_UINT64);\
    uint64_t *b = checkuserdata(L, 2, USERDATA_UINT64);
#define GETU_A(a) \
    uint64_t *a = checkuserdata(L, 1, USERDATA_UINT64);
#define OPU(operator) \
    GETU_AB(val_a, val_b);\
    if (!val_a || !val_b) { lua_pushnil(L); return 1; }\
    uint64_t *ret = raw_uint64_new(*val_a operator *val_b);\
    if (!ret) { lua_pushnil(L); return 1; }\
    pushuserdata(L, ret, USERDATA_UINT64);\
    return 1;
static int p_raw_toint64(lua_State* L) {
    int64_t val = 0;
    uint64_t *uval = (uint64_t*)checkuserdata(L, 1, USERDATA_UINT64);
    if (uval) {
        val = (int64_t)(*uval);
    } else {
        return p_raw_int64_new(L);
    }
    int64_t *new = raw_int64_new(val);
    if (!new) { lua_pushnil(L); return 1; }
    pushuserdata(L, new, USERDATA_INT64);
    return 1;
}
static int p_raw_touint64(lua_State* L) {
    uint64_t val = 0;
    int64_t *sval = (int64_t*)checkuserdata(L, 1, USERDATA_INT64);
    if (sval) {
        val = (uint64_t)(*sval);
    } else {
        return p_raw_uint64_new(L);
    }
    uint64_t *new = raw_uint64_new(val);
    if (!new) { lua_pushnil(L); return 1; }
    pushuserdata(L, new, USERDATA_UINT64);
    return 1;
}
/* ====================================*/

