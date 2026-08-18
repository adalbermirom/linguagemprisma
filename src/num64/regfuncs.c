static const struct luaL_Reg num64_funcs[] = {
    {"int64", p_raw_int64_new},
    {"uint64", p_raw_uint64_new},
    {"toint64", p_raw_toint64},
    {"touint64", p_raw_touint64},
    {"conv_int64", p_raw_toint64},
    {"conv_uint64", p_raw_touint64},
    {NULL, NULL}
};

