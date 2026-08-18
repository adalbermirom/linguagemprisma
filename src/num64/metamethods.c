/* =================================================== */
/* CORPO DOS MÉTODOS E METAMÉTODOS            */
/* =================================================== */

/* Métodos da classe int64_t */
static int p_int64_t___convstring(lua_State *L) {
            int64_t *val = (int64_t *)checkuserdata(L, 1, USERDATA_INT64);
            if(!val){
                lua_pushstring(L, "nil");
                return 1;
            }
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%" PRId64, *val);
            lua_pushstring(L, buffer);
            return 1;
}

static int p_int64_t___add(lua_State *L) {
            OP(+);   
}

static int p_int64_t___sub(lua_State *L) {
            OP(-);   
}

static int p_int64_t___mul(lua_State *L) {
            OP(*);   
}

static int p_int64_t___div(lua_State *L) {
            GET_AB(val_a, val_b);
            if(!val_a || !val_b || *val_b == 0) {
                return luaL_error(L, "tentativa de divisao por zero em int64");
            }
            int64_t *ret = raw_int64_new(*val_a / *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_INT64);
            return 1;
}

static int p_int64_t___mod(lua_State *L) {
            GET_AB(val_a, val_b);
            if(!val_a || !val_b || *val_b == 0) {
                return luaL_error(L, "tentativa de modulo por zero em int64");
            }
            int64_t *ret = raw_int64_new(*val_a % *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_INT64);
            return 1;
}

static int p_int64_t___gc(lua_State *L) {
            userdata *ud = lua_touserdata(L, 1);
            if(!ud || !ud->u){
                return 0;
            }
            free(ud->u);
            ud->u = NULL;
            return 0;
}

static int p_int64_t___bor(lua_State *L) {
            OP(|);
}

static int p_int64_t___band(lua_State *L) {
            OP(&);
}

static int p_int64_t___bxor(lua_State *L) {
            OP(^);
}

static int p_int64_t___bshl(lua_State *L) {
            GET_AB(val_a, val_b);
            if (!val_a || !val_b) { lua_pushnil(L); return 1; }
            if (*val_b < 0 || *val_b >= 64) {
                int64_t *ret = raw_int64_new(0);
                pushuserdata(L, ret, USERDATA_INT64);
                return 1;
            }
            int64_t *ret = raw_int64_new(*val_a << *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_INT64);
            return 1;
}

static int p_int64_t___bshr(lua_State *L) {
            GET_AB(val_a, val_b);
            if (!val_a || !val_b) { lua_pushnil(L); return 1; }
            if (*val_b < 0 || *val_b >= 64) {
                int64_t *ret = raw_int64_new(0);
                pushuserdata(L, ret, USERDATA_INT64);
                return 1;
            }
            int64_t *ret = raw_int64_new(*val_a >> *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_INT64);
            return 1;
}

static int p_int64_t___bnot(lua_State *L) {
            GET_A(a);
            if(!a) { lua_pushnil(L); return 1; }
            int64_t *ret = raw_int64_new( ~(*a) );
            if(!ret){
                lua_pushnil(L);
                return 1;
            }
            pushuserdata(L, ret, USERDATA_INT64);
            return 1;
}

static int p_int64_t___eq(lua_State *L) {
            GET_AB(a, b);
            if (!a || !b) {
                lua_pushboolean(L, 0);
                return 1;
            }
            lua_pushboolean(L, *a == *b);
            return 1;
}

static int p_int64_t___lt(lua_State *L) {
            GET_AB(a, b);
            if (!a || !b) {
                lua_pushboolean(L, 0);
                return 1;
            }
            lua_pushboolean(L, *a < *b);
            return 1;
}

static int p_int64_t___tipo(lua_State *L) {
            lua_pushstring(L, USERDATA_INT64);
            return 1;
}

/* Métodos da classe uint64_t */
static int p_uint64_t___convstring(lua_State *L) {
            uint64_t *val = (uint64_t *)checkuserdata(L, 1, USERDATA_UINT64);
            if(!val){
                lua_pushstring(L, "nil");
                return 1;
            }
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%" PRIu64, *val);
            lua_pushstring(L, buffer);
            return 1;
}

static int p_uint64_t___tipo(lua_State *L) {
            lua_pushstring(L, USERDATA_UINT64);
            return 1;
}

static int p_uint64_t___add(lua_State *L) {
            OPU(+);   
}

static int p_uint64_t___sub(lua_State *L) {
            OPU(-);   
}

static int p_uint64_t___mul(lua_State *L) {
            OPU(*);   
}

static int p_uint64_t___div(lua_State *L) {
            GETU_AB(val_a, val_b);
            if(!val_a || !val_b || *val_b == 0) {
                return luaL_error(L, "tentativa de divisao por zero em uint64");
            }
            uint64_t *ret = raw_uint64_new(*val_a / *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_UINT64);
            return 1;
}

static int p_uint64_t___mod(lua_State *L) {
            GETU_AB(val_a, val_b);
            if(!val_a || !val_b || *val_b == 0) {
                return luaL_error(L, "tentativa de modulo por zero em uint64");
            }
            uint64_t *ret = raw_uint64_new(*val_a % *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_UINT64);
            return 1;
}

static int p_uint64_t___gc(lua_State *L) {
            userdata *ud = lua_touserdata(L, 1);
            if(!ud || !ud->u){
                return 0;
            }
            free(ud->u);
            ud->u = NULL;
            return 0;
}

static int p_uint64_t___bor(lua_State *L) {
            OPU(|);
}

static int p_uint64_t___band(lua_State *L) {
            OPU(&);
}

static int p_uint64_t___bxor(lua_State *L) {
            OPU(^);
}

static int p_uint64_t___bshl(lua_State *L) {
            GETU_AB(val_a, val_b);
            if (!val_a || !val_b) { lua_pushnil(L); return 1; }
            if (*val_b >= 64) {
                uint64_t *ret = raw_uint64_new(0);
                pushuserdata(L, ret, USERDATA_UINT64);
                return 1;
            }
            uint64_t *ret = raw_uint64_new(*val_a << *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_UINT64);
            return 1;
}

static int p_uint64_t___bshr(lua_State *L) {
            GETU_AB(val_a, val_b);
            if (!val_a || !val_b) { lua_pushnil(L); return 1; }
            if (*val_b >= 64) {
                uint64_t *ret = raw_uint64_new(0);
                pushuserdata(L, ret, USERDATA_UINT64);
                return 1;
            }
            uint64_t *ret = raw_uint64_new(*val_a >> *val_b);
            if(!ret) { lua_pushnil(L); return 1; }
            pushuserdata(L, ret, USERDATA_UINT64);
            return 1;
}

static int p_uint64_t___bnot(lua_State *L) {
            GETU_A(a);
            if(!a) { lua_pushnil(L); return 1; }
            uint64_t *ret = raw_uint64_new( ~(*a) );
            if(!ret){
                lua_pushnil(L);
                return 1;
            }
            pushuserdata(L, ret, USERDATA_UINT64);
            return 1;
}

static int p_uint64_t___eq(lua_State *L) {
            GETU_AB(a, b);
            if (!a || !b) {
                lua_pushboolean(L, 0);
                return 1;
            }
            lua_pushboolean(L, *a == *b);
            return 1;
}

static int p_uint64_t___lt(lua_State *L) {
            GETU_AB(a, b);
            if (!a || !b) {
                lua_pushboolean(L, 0);
                return 1;
            }
            lua_pushboolean(L, *a < *b);
            return 1;
}

