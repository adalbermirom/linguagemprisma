static const struct luaL_Reg int64_t_methods[] = {
    {"__convstring", p_int64_t___convstring},
    {"__add", p_int64_t___add},
    {"__sub", p_int64_t___sub},
    {"__mul", p_int64_t___mul},
    {"__div", p_int64_t___div},
    {"__mod", p_int64_t___mod},
    {"__gc", p_int64_t___gc},
    {"__bor", p_int64_t___bor},
    {"__band", p_int64_t___band},
    {"__bxor", p_int64_t___bxor},
    {"__bshl", p_int64_t___bshl},
    {"__bshr", p_int64_t___bshr},
    {"__bnot", p_int64_t___bnot},
    {"__eq", p_int64_t___eq},
    {"__lt", p_int64_t___lt},
    {"__tipo", p_int64_t___tipo},
    {NULL, NULL}
};

void registrar_meta_int64_t(lua_State *L, int mod_idx) {
    int top_orig = lua_gettop(L);
    luaL_newmetatable(L, "userdata<int64_t>"); int meta_idx = lua_gettop(L);
    /* ==== Unbound Instance Method access (e.g., gtk4.GtkWindow.set_title(win, "my title")) ==== */
    /* criar uma tabela definir a meta_idx como metatabela dessa nova tabela.*/
    lua_newtable(L);int static_meta = lua_gettop(L);
    lua_pushvalue(L, meta_idx);
    lua_setfield(L, static_meta, "__meta");
    lua_pushvalue(L, meta_idx);
    lua_setmetatable(L, static_meta);
    lua_setfield(L, mod_idx, "int64_t");
    luaL_setfuncs(L, int64_t_methods, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, meta_idx , "__index");

    lua_settop(L, top_orig);
}

static const struct luaL_Reg uint64_t_methods[] = {
    {"__convstring", p_uint64_t___convstring},
    {"__tipo", p_uint64_t___tipo},
    {"__add", p_uint64_t___add},
    {"__sub", p_uint64_t___sub},
    {"__mul", p_uint64_t___mul},
    {"__div", p_uint64_t___div},
    {"__mod", p_uint64_t___mod},
    {"__gc", p_uint64_t___gc},
    {"__bor", p_uint64_t___bor},
    {"__band", p_uint64_t___band},
    {"__bxor", p_uint64_t___bxor},
    {"__bshl", p_uint64_t___bshl},
    {"__bshr", p_uint64_t___bshr},
    {"__bnot", p_uint64_t___bnot},
    {"__eq", p_uint64_t___eq},
    {"__lt", p_uint64_t___lt},
    {NULL, NULL}
};

void registrar_meta_uint64_t(lua_State *L, int mod_idx) {
    int top_orig = lua_gettop(L);
    luaL_newmetatable(L, "userdata<uint64_t>"); int meta_idx = lua_gettop(L);
    /* ==== Unbound Instance Method access (e.g., gtk4.GtkWindow.set_title(win, "my title")) ==== */
    /* criar uma tabela definir a meta_idx como metatabela dessa nova tabela.*/
    lua_newtable(L);int static_meta = lua_gettop(L);
    lua_pushvalue(L, meta_idx);
    lua_setfield(L, static_meta, "__meta");
    lua_pushvalue(L, meta_idx);
    lua_setmetatable(L, static_meta);
    lua_setfield(L, mod_idx, "uint64_t");
    luaL_setfuncs(L, uint64_t_methods, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, meta_idx , "__index");

    lua_settop(L, top_orig);
}

