
#ifndef __Prisma_H__
   #define __Prisma_H__    /*garante que essa lib só será carregada uma vez*/

#include "lua.h"  /*definições para lib Lua*/
#include "lauxlib.h"
#include <string.h>

/* minhas definições para Prisma:*/

#define NOME_PROG "prisma"
#define SHOW_VERSION "1.0.107"
#define SHOW_INFO "Prisma-" SHOW_VERSION "  linguagemprisma@gmail.com (jan/2024)"


/*facilidade para função de biblioteca C*/
#define PFUNC(f) static int f(lua_State *L){
#define FIMPFUNC }

/*facilidade para o retorno de uma função de biblioteca C*/
#define Pfunc static int  

/*prisma objeto*/
typedef lua_State pObj;

/*funções para empurrar itens na pilha*/
#define p_pushinteger lua_pushinteger
#define p_pushunsigned lua_pushunsigned
#define p_pushnil lua_pushnil
#define p_pushnumber lua_pushnumber
#define p_pushstring lua_pushstring
#define p_pushlstring lua_pushlstring /*(L,str,tam);*/
#define p_pushfstring lua_pushfstring /*(L,fmt,...);*/
#define p_pushchar(L, c) lua_pushfstring(L, "%c", c)
#define p_pushliteral lua_pushliteral /*(L,"string");*/
#define p_pushboolean lua_pushboolean
#define p_pushvalue lua_pushvalue
#define p_pushpointer lua_pushlightuserdata
#define p_pushcfunc lua_pushcfunction


#define p_pushfalse(L) lua_pushboolean(L,0);
#define p_pushtrue(L) lua_pushboolean(L,1);

/*funções para pegar itens da pilha*/
#define p_getinteger luaL_checkinteger /*retorna (lua_integer)*/
#define p_getint luaL_checkint  /*retorna (int)*/
#define p_getlong luaL_checklong
#define p_getunsigned luaL_checkunsigned
#define p_getnil luaL_checknil
#define p_getnumber luaL_checknumber
#define p_getstring luaL_checkstring
#define p_getlstring luaL_checklstring /*(L,index,&tam)*/
#define p_getboolean luaL_checkboolean
#define p_getpointer lua_tolightuserdata


/*option*/
#define p_checkoption luaL_checkoption  /* index op = (L, idx,const char*default, const char*str_list_op[]);*/

/*funções para tabela:*/

#define lua_pushpointer lua_pushlightuserdata

/*inserindo valor em campo de tabela:(type = number,string,lightuserdata ou pointer,integer, unsigned,string, value etc.*/
/*a tabela deve estar no topo*/
#define p_setfield(L,type,field,value) lua_push##type(L,value);lua_setfield(L,-2,field);
                                 
/*obtem um campo: tabela deve estar no topo*/
#define p_getfield(L,type,field) lua_getfield(L,-1,field);

#define p_pop lua_pop  /*elimina n elementos da pilha se 1 entao só o último.*/

#define p_geti lua_rawgeti /*lua_rawgeti (lua_State *L, int index, int n); // t[n]*/



#define tab_int(L,field,val) \
  p_pushinteger(L,val);\
  lua_setfield(L,-2,field);

  
#define tab_num(L,field,val) \
  p_pushnumber(L,val);\
  lua_setfield(L,-2,field);

/*Macro para facilitar tabela field ->   tab['field'] = val*/
/* a tabela deve estar no topo da pilha*/
#define tab(field, type, val)\
  lua_push ## type(L,val);\
  lua_setfield(L,-2,field);

/*Macro para facilitar tabela index ->   tab[n] = val*/
/* a tabela deve estar no topo da pilha */
#define tabindex(n, type, val)\
lua_push ## type( L , val);\
lua_rawseti(L, -2, n);

    
  /*macro para facilitar criação de bibliotecas em C prisma:*/
#define PRIS_START_REG_FUNCS(name_of_array_lib_funcs) static const luaL_Reg name_of_array_lib_funcs[] ={


#define PRIS_END_REG_FUNCS ,{NULL,NULL}\
};

#define PRIS_OPEN_MODULO(L, nome_lib) LUA_API int luaopen_ ## nome_lib (lua_State *L) {
  
#define PRIS_CLOSE_MODULO(ret)   return ret;\
}



/*Macro para registrar facilmente uma biblioteca prisma em C:*/
#define PRIS_REGISTER_LIB(L, lib_name, lib_funcs){ \
	luaL_newlib(L, lib_funcs);\
    lua_pushvalue(L,-1);\
    lua_setglobal(L, lib_name);\
}



/*Macro para criar um userdata facilmente em prisma C api:*/
#define PRIS_CREATE_USERDATA(L,mtname, size) lua_newuserdata(L, size);\
        luaL_getmetatable(L, mtname);\
        lua_setmetatable(L, -2);


/*Macro para definir uma metatabela em uma tabela que está no top*/
#define PRIS_SET_META(L, mtname) luaL_getmetatable(L, mtname); lua_setmetatable(L, -2);
      

/*versao macro da funcao acima:*/
#define PRIS_REGISTER_META(L, obj_name, meta_funcs) {  \
	luaL_newmetatable(L, obj_name);/*cria a tabela metatable [MT]*/ \
    lua_pushvalue(L, -1); /*duplica a metatabela [MT, MT]*/ \
    lua_setfield(L, -2, "__index"); /*define MT.__index = MT e tira a cópia do topo*/ \
    luaL_setfuncs(L, meta_funcs, 0);/*adiciona as funções em MT [MT]*/\
    lua_pop(L, 1); /*retira o MT do top, pilha sem MT: []*/\
}




#define pris_erro luaL_error

#define PRIS_FUNC(funcname, L) static int funcname(lua_State*L)
  

/*facilidades para criar bibliotecas prisma*/
#define PRISMA_FUNC(name) static int name(lua_State*L){
#define FIM_PRISMA_FUNC(n) return n;}


/*exemplo:
    PRISMA_FUNC(lib_soma);
        int a = luaL_checknumber(L,1);
        int b = luaL_checknumber(L,1);
        lua_pushinteger(L, a+b);
    FIM_PRISMA_FUNC(1); //1 retorno 

*/

#define PRISMA_REG_FUNCS(name) static luaL_Reg name[] = {
	#define FN(name, cfunction) {name, cfunction},
#define FIM_PRISMA_REG_FUNCS {NULL, NULL}};

/*
    exemplo:
    PRISMA_GER_FUNCS(teste_funcs)
        FN("soma", lib_soma})
        FN("mult", lib_mult)
    FIM_PRISMA_REG_FUNCS
*/

#define PRISMA_MODULO(name) LUA_API int luaopen_##name(lua_State *L){
#define FIM_PRISMA_MODULO(n) return n;}

#define REGISTRE_FUNCS(funcs_array, namespace) luaL_newlib(L, funcs_array);

/*
     exemplo:
     PRISMA_MODULO(pteste)
          REGISTRE_FUNCS(teste_funcs, "pteste"); //namespace = pteste ( pteste.soma(1,2) );
     FIM_PRISMA_MODULO(1) //um retorno;
*/



#endif

                         
                       
   
                                 
