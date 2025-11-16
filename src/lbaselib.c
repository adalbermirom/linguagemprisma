/*
 * BETO MODIFICATION Sat Nov 16 2025
 * 
** $Id: lbaselib.c,v 1.276.1.1 2013/04/12 18:48:47 roberto Exp $
** Basic library
** See Copyright Notice in lua.h
*/



#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lbaselib_c
#define LUA_LIB

#include "lua.h"
#include "prisma.h"  /*Beto modification 11/2023*/

#include "lauxlib.h"
#include "lualib.h"

/*#include "lclass.h"*/

static int luaB_print (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  lua_getglobal(L, "convstring");
  for (i=1; i<=n; i++) {
    const char *s;
    size_t l;
    lua_pushvalue(L, -1);  /* function to be called */
    lua_pushvalue(L, i);   /* value to print */
    lua_call(L, 1, 1);
    s = lua_tolstring(L, -1, &l);  /* get result */
    if (s == NULL)
      return luaL_error(L,
         LUA_QL("convstring") " deve retornar uma string para " LUA_QL("imprima"));
    if (i>1) luai_writestring("\t", 1);
    luai_writestring(s, l);
    lua_pop(L, 1);  /* pop result */
  }
  luai_writeline();
  return 0;
}


#define SPACECHARS	" \f\n\r\t\v"

static int luaB_tonumber (lua_State *L) {
  if (lua_isnoneornil(L, 2)) {  /* standard conversion */
    int isnum;
    lua_Number n = lua_tonumberx(L, 1, &isnum);
    if (isnum) {
      lua_pushnumber(L, n);
      return 1;
    }  /* else not a number; must be something */
    luaL_checkany(L, 1);
  }
  else {
    size_t l;
    const char *s = luaL_checklstring(L, 1, &l);
    const char *e = s + l;  /* end point for 's' */
    int base = luaL_checkint(L, 2);
    int neg = 0;
    luaL_argcheck(L, 2 <= base && base <= 36, 2, "base fora do comprimento");
    s += strspn(s, SPACECHARS);  /* skip initial spaces */
    if (*s == '-') { s++; neg = 1; }  /* handle signal */
    else if (*s == '+') s++;
    if (isalnum((unsigned char)*s)) {
      lua_Number n = 0;
      do {
        int digit = (isdigit((unsigned char)*s)) ? *s - '0'
                       : toupper((unsigned char)*s) - 'A' + 10;
        if (digit >= base) break;  /* invalid numeral; force a fail */
        n = n * (lua_Number)base + (lua_Number)digit;
        s++;
      } while (isalnum((unsigned char)*s));
      s += strspn(s, SPACECHARS);  /* skip trailing spaces */
      if (s == e) {  /* no invalid trailing characters? */
        lua_pushnumber(L, (neg) ? -n : n);
        return 1;
      }  /* else not a number */
    }  /* else not a number */
  }
  lua_pushnil(L);  /* not a number */
  return 1;
}


static int luaB_error (lua_State *L) {
  int level = luaL_optint(L, 2, 1);
  lua_settop(L, 1);
  if (lua_isstring(L, 1) && level > 0) {  /* add extra information? */
    luaL_where(L, level);
    lua_pushvalue(L, 1);
    lua_concat(L, 2);
  }
  return lua_error(L);
}


static int luaB_getmetatable (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_getmetatable(L, 1)) {
    lua_pushnil(L);
    return 1;  /* no metatable */
  }
  luaL_getmetafield(L, 1, "__metatabela");
  return 1;  /* returns either __metatable field (if present) or metatable */
}


static int luaB_setmetatable (lua_State *L) {
  int t = lua_type(L, 2);
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_argcheck(L, t == LUA_TNIL || t == LUA_TTABLE, 2,
                    "espera-se: nulo ou tabela");
  if (luaL_getmetafield(L, 1, "__metatabela"))
    return luaL_error(L, "nao foi possivel modificar uma metatabela protegida");
  lua_settop(L, 2);
  lua_setmetatable(L, 1);
  return 1;
}


static int luaB_rawequal (lua_State *L) {
  luaL_checkany(L, 1);
  luaL_checkany(L, 2);
  lua_pushboolean(L, lua_rawequal(L, 1, 2));
  return 1;
}


static int luaB_rawlen (lua_State *L) {
  int t = lua_type(L, 1);
  luaL_argcheck(L, t == LUA_TTABLE || t == LUA_TSTRING, 1,
                   "espera-se: tabela ou string");
  lua_pushinteger(L, lua_rawlen(L, 1));
  return 1;
}


static int luaB_rawget (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checkany(L, 2);
  lua_settop(L, 2);
  lua_rawget(L, 1);
  return 1;
}

static int luaB_rawset (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checkany(L, 2);
  luaL_checkany(L, 3);
  lua_settop(L, 3);
  lua_rawset(L, 1);
  return 1;
}


static int luaB_collectgarbage (lua_State *L) {
  static const char *const opts[] = /*{"stop", "restart", "collect",\
    "cont", "step", "setpause", "setstepmul",\
    "setmajorinc", "isrunning", "generational", "incremental", NULL}; */
    {"parar", "reiniciar", "coletar",
    "cont","estapa","pausar","setstepmul",
    "setmajorinc","executando","generational","incremental", NULL};
  static const int optsnum[] = {LUA_GCSTOP, LUA_GCRESTART, LUA_GCCOLLECT,
    LUA_GCCOUNT, LUA_GCSTEP, LUA_GCSETPAUSE, LUA_GCSETSTEPMUL,
    LUA_GCSETMAJORINC, LUA_GCISRUNNING, LUA_GCGEN, LUA_GCINC};
  int o = optsnum[luaL_checkoption(L, 1, "coletar", opts)];
  int ex = luaL_optint(L, 2, 0);
  int res = lua_gc(L, o, ex);
  switch (o) {
    case LUA_GCCOUNT: {
      int b = lua_gc(L, LUA_GCCOUNTB, 0);
      lua_pushnumber(L, res + ((lua_Number)b/1024));
      lua_pushinteger(L, b);
      return 2;
    }
    case LUA_GCSTEP: case LUA_GCISRUNNING: {
      lua_pushboolean(L, res);
      return 1;
    }
    default: {
      lua_pushinteger(L, res);
      return 1;
    }
  }
}


static int luaB_type (lua_State *L) {
  luaL_checkany(L, 1);
  if (!luaL_callmeta(L, 1, "__tipo"))
    lua_pushstring(L, luaL_typename(L, 1));
  return 1;
}


static int pairsmeta (lua_State *L, const char *method, int iszero,
                      lua_CFunction iter) {
  if (!luaL_getmetafield(L, 1, method)) {  /* no metamethod? */
    luaL_checktype(L, 1, LUA_TTABLE);  /* argument must be a table */
    lua_pushcfunction(L, iter);  /* will return generator, */
    lua_pushvalue(L, 1);  /* state, */
    if (iszero) lua_pushinteger(L, 0);  /* and initial value */
    else lua_pushnil(L);
  }
  else {
    lua_pushvalue(L, 1);  /* argument 'self' to metamethod */
    lua_call(L, 1, 3);  /* get 3 values from metamethod */
  }
  return 3;
}


static int luaB_next (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_settop(L, 2);  /* create a 2nd argument if there isn't one */
  if (lua_next(L, 1))
    return 2;
  else {
    lua_pushnil(L);
    return 1;
  }
}


static int luaB_pairs (lua_State *L) {
  return pairsmeta(L, "__pares", 0, luaB_next);
}


static int ipairsaux (lua_State *L) {
  int i = luaL_checkint(L, 2);
  luaL_checktype(L, 1, LUA_TTABLE);
  i++;  /* next value */
  lua_pushinteger(L, i);
  lua_rawgeti(L, 1, i);
  return (lua_isnil(L, -1)) ? 1 : 2;
}


static int luaB_ipairs (lua_State *L) {
  return pairsmeta(L, "ipares", 1, ipairsaux);
}


static int load_aux (lua_State *L, int status, int envidx) {
  if (status == LUA_OK) {
    if (envidx != 0) {  /* 'env' parameter? */
      lua_pushvalue(L, envidx);  /* environment for loaded function */
      if (!lua_setupvalue(L, -2, 1))  /* set it as 1st upvalue */
        lua_pop(L, 1);  /* remove 'env' if not used by previous call */
    }
    return 1;
  }
  else {  /* error (message is on top of the stack) */
    lua_pushnil(L);
    lua_insert(L, -2);  /* put before error message */
    return 2;  /* return nil plus error message */
  }
}


static int luaB_loadfile (lua_State *L) {
  const char *fname = luaL_optstring(L, 1, NULL);
  const char *mode = luaL_optstring(L, 2, NULL);
  int env = (!lua_isnone(L, 3) ? 3 : 0);  /* 'env' index or 0 if no 'env' */
  int status = luaL_loadfilex(L, fname, mode);
  return load_aux(L, status, env);
}


/*
** {======================================================
** Generic Read function
** =======================================================
*/


/*
** reserved slot, above all arguments, to hold a copy of the returned
** string to avoid it being collected while parsed. 'load' has four
** optional arguments (chunk, source name, mode, and environment).
*/
#define RESERVEDSLOT	5


/*
** Reader for generic `load' function: `lua_load' uses the
** stack for internal stuff, so the reader cannot change the
** stack top. Instead, it keeps its resulting string in a
** reserved slot inside the stack.
*/
static const char *generic_reader (lua_State *L, void *ud, size_t *size) {
  (void)(ud);  /* not used */
  luaL_checkstack(L, 2, "muitas funcoes aninhadas");
  lua_pushvalue(L, 1);  /* get function */
  lua_call(L, 0, 1);  /* call it */
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);  /* pop result */
    *size = 0;
    return NULL;
  }
  else if (!lua_isstring(L, -1))
    luaL_error(L, "funcao de leitura deve retornar uma string");
  lua_replace(L, RESERVEDSLOT);  /* save string in reserved slot */
  return lua_tolstring(L, RESERVEDSLOT, size);
}


static int luaB_load (lua_State *L) {
  int status;
  size_t l;
  const char *s = lua_tolstring(L, 1, &l);
  const char *mode = luaL_optstring(L, 3, "bt");
  int env = (!lua_isnone(L, 4) ? 4 : 0);  /* 'env' index or 0 if no 'env' */
  if (s != NULL) {  /* loading a string? */
    const char *chunkname = luaL_optstring(L, 2, s);
    status = luaL_loadbufferx(L, s, l, chunkname, mode);
  }
  else {  /* loading from a reader function */
    const char *chunkname = luaL_optstring(L, 2, "=(carregue)");
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, RESERVEDSLOT);  /* create reserved slot */
    status = lua_load(L, generic_reader, NULL, chunkname, mode);
  }
  return load_aux(L, status, env);
}

/* }====================================================== */


static int dofilecont (lua_State *L) {
  return lua_gettop(L) - 1;
}


static int luaB_dofile (lua_State *L) {
  const char *fname = luaL_optstring(L, 1, NULL);
  lua_settop(L, 1);
  if (luaL_loadfile(L, fname) != LUA_OK)
    return lua_error(L);
  lua_callk(L, 0, LUA_MULTRET, 0, dofilecont);
  return dofilecont(L);
}


static int luaB_assert (lua_State *L) {
  if (!lua_toboolean(L, 1))
    return luaL_error(L, "%s", luaL_optstring(L, 2, "tentativa falhou, (assertion)"));
  return lua_gettop(L);
}


/*Beto ---> validando argumentos de funções (tipagem adaptada): numero, string, tabela, funcao, boolean;*/


/**
 * Função-auxiliar que faz o trabalho:
 * Ela não é registrada em Prisma, é só para uso interno (static).
 */
static int luaB_check_all_args(lua_State *L, int tipo_constante, const char *tipo_nome) {
    int top = lua_gettop(L), i=1;
        
    for(i=1; i<=top; i++){
        if (lua_type(L, i) != tipo_constante) { 
            const char* tipo_obj;
            if (luaL_callmeta(L, i, "__tipo")){
                tipo_obj = luaL_checkstring(L,-1);
                lua_pop(L,1);
            } else {
                tipo_obj = luaL_typename(L, i);
            }
            /* Use o 'tipo_nome' que recebemos como argumento*/
            luaL_error(L, "\n     |\n     |Erro: valor incorreto, espera-se `%s' em vez de: `%s' em (arg#%d)%s ", 
                tipo_nome, tipo_obj, i, "\n     |____________________" );
        }
    }
    return top;
}

static int luaB_check_number(lua_State*L){
	return luaB_check_all_args(L, LUA_TNUMBER, "numero");
}
static int luaB_check_string(lua_State *L){
    return luaB_check_all_args(L, LUA_TSTRING, "string");
}

static int luaB_check_table(lua_State *L){
    return luaB_check_all_args(L, LUA_TTABLE, "tabela");
}

static int luaB_check_function(lua_State *L){
    return luaB_check_all_args(L, LUA_TFUNCTION, "funcao");
}

static int luaB_check_boolean(lua_State *L){
    return luaB_check_all_args(L, LUA_TBOOLEAN, "booleano");
}

static int luaB_check_thread(lua_State *L){
    return luaB_check_all_args(L, LUA_TTHREAD, "thread");
}


/*LUA_TNIL, LUA_TNUMBER, LUA_TBOOLEAN, LUA_TSTRING, LUA_TTABLE, */
/*LUA_TFUNCTION, LUA_TUSERDATA, LUA_TTHREAD, and LUA_TLIGHTUSERDATA.*/


/*Beto check userdata*/
static int luaB_check_userdata(lua_State *L){
        int top = lua_gettop(L), i=1;
        const char*tipo_obj;
        const char*tipo_checado = "userdata";
        
        for(i=1; i<=top; i++){	        
	        if(lua_type(L,i)!= LUA_TUSERDATA && lua_type(L,i)!= LUA_TLIGHTUSERDATA){
                /*const char* tipo_obj;*/
                if (luaL_callmeta(L, i, "__tipo")){
                    tipo_obj = luaL_checkstring(L,-1);
                    lua_pop(L,1);
                } else {
                    tipo_obj = luaL_typename(L, i);
                }

                /*erro:*/
               luaL_error(L, "\n     |\n     |Erro: valor incorreto, espera-se `%s' em vez de: `%s' em (arg#%d)%s ", 
                tipo_checado, tipo_obj, i, "\n     |____________________" );
            
            /* (luaL_error nunca retorna)*/
           }
	            
		}
            
	return top;
}

/*Beto check userdata                 ---->      Val('type', va1,v2,v3,v4...);*/
static int luaB_check_var(lua_State *L){
        int top = lua_gettop(L), i=2;
      /*  const char*tipo_obj;*/
        
        const char*tipo_checado = luaL_checkstring(L,1);
        
        for(i=2; i<=top; i++){            
            pris_checktypebyname(L,i, tipo_checado); /*em lauxilib.c*/
		}
		       
	return top - 1; /*não retorna a string 'type', somente os valores do 2 arg em diante.*/
}



/*------------------ FIM BETO CHECK ARGS FUNCS.....*/

static int luaB_select (lua_State *L) {
  int n = lua_gettop(L);
  if (lua_type(L, 1) == LUA_TSTRING && *lua_tostring(L, 1) == '#') {
    lua_pushinteger(L, n-1);
    return 1;
  }
  else {
    int i = luaL_checkint(L, 1);
    if (i < 0) i = n + i;
    else if (i > n) i = n;
    luaL_argcheck(L, 1 <= i, 1, "indice fora do comprimento");
    return n - i;
  }
}


static int finishpcall (lua_State *L, int status) {
  if (!lua_checkstack(L, 1)) {  /* no space for extra boolean? */
    lua_settop(L, 0);  /* create space for return values */
    lua_pushboolean(L, 0);
    lua_pushstring(L, "estouro da pilha");
    return 2;  /* return false, msg */
  }
  lua_pushboolean(L, status);  /* first result (status) */
  lua_replace(L, 1);  /* put first result in first slot */
  return lua_gettop(L);
}


static int pcallcont (lua_State *L) {
  int status = lua_getctx(L, NULL);
  return finishpcall(L, (status == LUA_YIELD));
}


static int luaB_pcall (lua_State *L) {
  int status;
  luaL_checkany(L, 1);
  lua_pushnil(L);
  lua_insert(L, 1);  /* create space for status result */
  status = lua_pcallk(L, lua_gettop(L) - 2, LUA_MULTRET, 0, 0, pcallcont);
  return finishpcall(L, (status == LUA_OK));
}


static int luaB_xpcall (lua_State *L) {
  int status;
  int n = lua_gettop(L);
  luaL_argcheck(L, n >= 2, 2, "espera-se algum valor");
  lua_pushvalue(L, 1);  /* exchange function... */
  lua_copy(L, 2, 1);  /* ...and error handler */
  lua_replace(L, 2);
  status = lua_pcallk(L, n - 2, LUA_MULTRET, 1, 0, pcallcont);
  return finishpcall(L, (status == LUA_OK));
}


static int luaB_tostring (lua_State *L) {
  luaL_checkany(L, 1);
  luaL_tolstring(L, 1, NULL);
  return 1;
}


static int luaB_dostring(lua_State *L){
int x = luaL_dostring(L, luaL_checkstring(L,1) );
lua_pushinteger(L,x);
return 1;
}


/************* BETO MODIFICATION ****/
static int luaB_puts(lua_State* P) {
    int top = lua_gettop(P);
    
    /* 1. Lida com o caso de 0 argumentos*/
    if (top == 0) {
        putchar('\n');
        return 0;
    }

    luaL_Buffer b;
    luaL_buffinit(P, &b);
    size_t len;
    const char *s;

    for (int i = 1; i <= top; i++) {
        
        s = luaL_tolstring(P, i, &len);
        luaL_addlstring(&b, s, len);
        
        if ( (b.b != b.initb) && (b.b != s) ) { 
            lua_remove(P, -2); /* 'conv_str' estava em -2*/
        } else {
            lua_pop(P, 1);     /* 'conv_str' estava em -1*/
        }

        if (i < top) {
            luaL_addchar(&b, '\t');
        }
    }
    
    /* 6. Adiciona newline*/
    luaL_addchar(&b, '\n');

    fwrite((const char*)b.b, 1, b.n, stdout);
    if (b.b != b.initb) {
        lua_remove(P, -1); /* Limpa o 'buffer_ud'*/
    }
    
    return 0;
}

/************* FIM BETO MODIFICATION ****/




/*
** {======================================================
**         printf  (string.format to printf modification ADALBERTO BETO MODIFICATION
** =======================================================
*/


/* macro to `unsign' a character */
#define uchar(c)	((unsigned char)(c))

#define L_ESC		'%'

/*
** LUA_INTFRMLEN is the length modifier for integer conversions in
** 'string.format'; LUA_INTFRM_T is the integer type corresponding to
** the previous length
*/
#if !defined(LUA_INTFRMLEN)	/* { */
#if defined(LUA_USE_LONGLONG)

#define LUA_INTFRMLEN		"ll"
#define LUA_INTFRM_T		long long

#else

#define LUA_INTFRMLEN		"l"
#define LUA_INTFRM_T		long

#endif
#endif				/* } */


/*
** LUA_FLTFRMLEN is the length modifier for float conversions in
** 'string.format'; LUA_FLTFRM_T is the float type corresponding to
** the previous length
*/
#if !defined(LUA_FLTFRMLEN)

#define LUA_FLTFRMLEN		""
#define LUA_FLTFRM_T		double

#endif


/* maximum size of each formatted item (> len(format('%99.99f', -1e308))) */
#define MAX_ITEM	512
/* valid flags in a format specification */
#define FLAGS	"-+ #0"
/*
** maximum size of each format specification (such as '%-099.99d')
** (+10 accounts for %99.99x plus margin of error)
*/
#define MAX_FORMAT	(sizeof(FLAGS) + sizeof(LUA_INTFRMLEN) + 10)


static void addquoted (lua_State *L, luaL_Buffer *b, int arg) {
  size_t l;
  const char *s = luaL_checklstring(L, arg, &l);
  luaL_addchar(b, '"');
  while (l--) {
    if (*s == '"' || *s == '\\' || *s == '\n') {
      luaL_addchar(b, '\\');
      luaL_addchar(b, *s);
    }
    else if (*s == '\0' || iscntrl(uchar(*s))) {
      char buff[10];
      if (!isdigit(uchar(*(s+1))))
        sprintf(buff, "\\%d", (int)uchar(*s));
      else
        sprintf(buff, "\\%03d", (int)uchar(*s));
      luaL_addstring(b, buff);
    }
    else
      luaL_addchar(b, *s);
    s++;
  }
  luaL_addchar(b, '"');
}

static const char *scanformat (lua_State *L, const char *strfrmt, char *form) {
  const char *p = strfrmt;
  while (*p != '\0' && strchr(FLAGS, *p) != NULL) p++;  /* skip flags */
  if ((size_t)(p - strfrmt) >= sizeof(FLAGS)/sizeof(char))
    luaL_error(L, "formato invalido (flags repetidos)");
  if (isdigit(uchar(*p))) p++;  /* skip width */
  if (isdigit(uchar(*p))) p++;  /* (2 digits at most) */
  if (*p == '.') {
    p++;
    if (isdigit(uchar(*p))) p++;  /* skip precision */
    if (isdigit(uchar(*p))) p++;  /* (2 digits at most) */
  }
  if (isdigit(uchar(*p)))
    luaL_error(L, "formato invalido (largura ou precisao muito longa)");
  *(form++) = '%';
  memcpy(form, strfrmt, (p - strfrmt + 1) * sizeof(char));
  form += p - strfrmt + 1;
  *form = '\0';
  return p;
}


/*
** add length modifier into formats
*/
static void addlenmod (char *form, const char *lenmod) {
  size_t l = strlen(form);
  size_t lm = strlen(lenmod);
  char spec = form[l - 1];
  strcpy(form + l - 1, lenmod);
  form[l + lm - 1] = spec;
  form[l + lm] = '\0';
}





static int luaB_printf (lua_State *L) {
  int top = lua_gettop(L);
  int arg = 1;
  size_t sfl;
  const char *strfrmt = luaL_checklstring(L, arg, &sfl);
  const char *strfrmt_end = strfrmt+sfl;
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while (strfrmt < strfrmt_end) {
    if (*strfrmt != L_ESC)
      luaL_addchar(&b, *strfrmt++);
    else if (*++strfrmt == L_ESC)
      luaL_addchar(&b, *strfrmt++);  /* %% */
    else { /* format item */
      char form[MAX_FORMAT];  /* to store the format (`%...') */
      char *buff = luaL_prepbuffsize(&b, MAX_ITEM);  /* to put formatted item */
      int nb = 0;  /* number of bytes in added item */
      if (++arg > top)
        luaL_argerror(L, arg, "nenhum valor");
      strfrmt = scanformat(L, strfrmt, form);
      switch (*strfrmt++) {
        case 'c': {
          nb = sprintf(buff, form, luaL_checkint(L, arg));
          break;
        }
        case 'd': case 'i': {
          lua_Number n = luaL_checknumber(L, arg);
          LUA_INTFRM_T ni = (LUA_INTFRM_T)n;
          lua_Number diff = n - (lua_Number)ni;
          luaL_argcheck(L, -1 < diff && diff < 1, arg,
                        "nao e um numero em comprimento apropriado");
          addlenmod(form, LUA_INTFRMLEN);
          nb = sprintf(buff, form, ni);
          break;
        }
        case 'o': case 'u': case 'x': case 'X': {
          lua_Number n = luaL_checknumber(L, arg);
          unsigned LUA_INTFRM_T ni = (unsigned LUA_INTFRM_T)n;
          lua_Number diff = n - (lua_Number)ni;
          luaL_argcheck(L, -1 < diff && diff < 1, arg,
                        "nao e um numero nao-negativo em comprimento apropriado");
          addlenmod(form, LUA_INTFRMLEN);
          nb = sprintf(buff, form, ni);
          break;
        }
        case 'e': case 'E': case 'f':
#if defined(LUA_USE_AFORMAT)
        case 'a': case 'A':
#endif
        case 'g': case 'G': {
          addlenmod(form, LUA_FLTFRMLEN);
          nb = sprintf(buff, form, (LUA_FLTFRM_T)luaL_checknumber(L, arg));
          break;
        }
        case 'q': {
          addquoted(L, &b, arg);
          break;
        }
        case 's': {
          size_t l;
          const char *s = luaL_tolstring(L, arg, &l);
          if (!strchr(form, '.') && l >= 100) {
            /* no precision and string is too long to be formatted;
               keep original string */
            luaL_addvalue(&b);
            break;
          }
          else {
            nb = sprintf(buff, form, s);
            lua_pop(L, 1);  /* remove result from 'luaL_tolstring' */
            break;
          }
        }
        default: {  /* also treat cases `pnLlh' */
          return luaL_error(L, "opcao invalida " LUA_QL("%%%c") " para "
                               LUA_QL("formato"), *(strfrmt - 1));
        }
      }
      luaL_addsize(&b, nb);
    }
  }
  
 
  /*luaL_pushresult(&b);*/
  
  /*fwrite para imprimir 'len' bytes*/
  fwrite((const char*)b.b, 1, b.n, stdout); 
  if ((b).b != (b).initb){
    lua_remove(L, -1);  /* remove o buffer alocado se existir um userdata no topo */
  }
  return 0;
}


/* }====================================================== */

/* forward declaration */
static int counter (lua_State *L);
    
int newCounter (lua_State *L) {
      double init=1;
      double inc=1;
      if(lua_isnumber(L,1)) init = luaL_checknumber(L,1); /* initial value;*/
      if(lua_isnumber(L,2)) inc = luaL_checknumber(L,2); /*the increment value +1, +2, -1, -2... etc.*/
      init = init - inc;
      lua_pushnumber(L, init); /* upvalueindex(1);*/
      lua_pushnumber(L, inc);/*upvalueindex(2);*/
      lua_pushcclosure(L, &counter, 2);
      return 1;
}

static int counter (lua_State *L) {
      double val = lua_tonumber(L, lua_upvalueindex(1));
      double inc = lua_tonumber(L, lua_upvalueindex(2));
      val = val + (inc);
     
      lua_pushnumber(L, val);  /* new value */
      lua_pushvalue(L, -1);  /* duplicate it */
      lua_replace(L, lua_upvalueindex(1));  /* update upvalue */
      return 1;  /* return new value */
}

/*  ------------   switch func ------------------------*/
/*funcao switch(s)
    retorne funcao(ss)
        retorne s == ss;
    fim
fim*/


static int switch_implem(lua_State*L){
    int i = 1;
    int top = lua_gettop(L);
    lua_pushvalue(L, lua_upvalueindex(1) );/* put it in top of stack*/
    /* upvalue is on top + 1 now */
    
    for (i=1;i<=top;i++){   
       if(lua_rawequal(L, i, -1)){  /*se pelo menos um valor for igual a condição entao retorna verdadeiro.*/
            lua_pushboolean(L, 1);
            return 1;
	   }
    }
    lua_pushboolean(L,0);
    return 1;
}

static int switch_func(lua_State*L){   /*# Beto modification modificado beto.*/
   luaL_checkany(L,1);
   lua_pushcclosure(L,&switch_implem,1);
   return 1;
} /* End switch func --------------------------------*/


#define MYCLASS_LIB
#ifdef MYCLASS_LIB
/*
 * Implementacao em C da logica de classes do Prisma,
 * baseada no Programming in Lua (Pil 3ed).
 */

/*
 * c:novo(o)
 * 1 = c (self)
 * 2 = o (optional table)
 */
static int myclass_new(lua_State*L){
    int c = 1;
    int o = 2;
    
    /* o = o or {} */
    if( lua_isnone( L, 2 ) ) { /*se 2 é none ou nil entao 'o' vai para '2';*/
        lua_createtable(L, 0, 0); /* Novo 'o' vai para o topo (indice 2) */
    }else{
        luaL_checktype(L, 2, LUA_TTABLE);
    }
    
    /* Pilha: [c] [o] */
    
    /* setmetatable(o, c) */
    lua_pushvalue(L, c); /* Empurra 'c' (a metatabela) */
    lua_setmetatable(L, o); /* Define a metatabela de 'o' e popa 'c' */
    
    /* Pilha: [c] [o] */
    
    /* Retorna 'o' */
    lua_settop(L, 2); /* Garante que a pilha so tenha [c] [o] */
    return 1; /* Retorna o valor do topo (que e 'o') */
}


/*
 * Metodo __index (procura nos pais)
 * 1 = t (self)
 * 2 = k (key)
 * upvalue(1) = parents
 */
static int myclass__index(lua_State* L) {
    const char *k = lua_tostring(L, 2);
    int parents = lua_upvalueindex(1);
    int len_parents = luaL_len(L, parents);
    int i;

    for ( i = 1; i <= len_parents; i++ ) {
        lua_rawgeti(L, parents, i); /* Empilha parents[i] */
        
        luaL_checktype(L, -1, LUA_TTABLE);/* Testa se é tabela*/

        lua_getfield(L, -1, k);     /* Empilha parents[i][k] (value) */
        
        if(lua_isnoneornil(L, -1) == 0){
            return 1; /* Retorna 'value' */
        }

        lua_pop(L, 2); /* Remove 'value' (nil) e 'parents[i]' */
    }

    /* Nao encontrou em nenhum pai */
    lua_pushnil(L);
    return 1;
}

/*
 * Metodo __call
 * 1 = c (self)
 * 2..top = args
 */
static int myclass__call(lua_State* L) {
    /* obj = self:novo(); */
    lua_pushliteral(L, "novo");
    lua_gettable(L, 1); /* Pega c.novo */
    lua_pushvalue(L, 1); /* 'self' (c) como argumento */
    lua_call(L, 1, 1); /* Chama c:novo(c), retorna obj */
    /* Pilha: [c] [a1] [a2] ... [obj] (obj esta no topo) */

    /* Prepara para chamar obj:construtor(a1, a2, ...) */
    lua_pushvalue(L, -1); /* Copia obj */
    /* Pilha: [c] [a1] [a2] [obj] [obj_copy] */
    lua_insert(L, 2);
    /* Pilha: [c] [obj_copy] [a1] [a2] [obj] */
    
    lua_pushliteral(L, "construtor");
    lua_gettable(L, -2); /* Pega obj.construtor */
    /* Pilha: [c] [obj_copy] [a1] [a2] [obj] [constr_func] */
    
    if(!lua_isfunction(L, -1) ) {
        lua_pushliteral(L, " Metodo construtor() da Classe nao definido ou invalido.\nDefina-o: funcao class:construtor() ... fim");
        lua_error(L);
    }
    
    /* Pilha: [c] [obj_copy] [a1] [a2] [obj] [constr_func] */
    lua_insert(L, 3);
    /* Pilha: [c] [obj_copy] [constr_func] [a1] [a2] [obj] */
    lua_insert(L, 4);
    /* Pilha: [c] [obj_copy] [constr_func] [obj] [a1] [a2] */
    
    /* Chama construtor(obj, a1, a2) */
    /* O numero de args e 'top' - 3 (c, obj_copy, constr_func) */
    lua_call(L, lua_gettop(L) - 3, 0);
    
    /* Pilha: [c] [obj_copy] */
    
    /* Retorna o obj_copy */
    return 1;
}

/*
 * Funcao global Classe(...)
 * ... = P1, P2, ...
 */
    
static int luaB_class(lua_State *L){
    int nargs = lua_gettop(L);
    lua_createtable(L,0,0); /* c = {}; // [top+1] = c*/
    /* Pilha: [P1] [P2] ... [c] */
    /*int c = nargs + 1;*/
    
    /*stack = c*/
    
    lua_createtable(L, nargs, 0); /* parents = {...} */
    /* Pilha: [P1] [P2] ... [c] [parents] */
    int i = 0;
    
    /*int parents = nargs + 2;*/
    /*stack = c, parents.*/
    /* Iteramos pelos argumentos originais (P1, P2, etc.) */
    for( i = 1; i <= nargs; i++ ) {  /* parents = {...} */
        lua_pushvalue(L, i);    /* 1. Empurra o argumento original (ex: ClasseA) */
        /*stack = c, parents, value*/
        lua_rawseti(L, -2, i);  /* 2. Define parents[i] = ClasseA (e popa ClasseA) */
        /*stack = c, parents*/
    }
    
    /* c.novo = myclass_new */
    lua_pushcfunction(L, myclass_new);  /*top + 3 */
    /*stack = c, parents, function myclass_new*/
    lua_setfield(L, -3, "novo"); /* -3 e 'c' */
    /*stack = c, parents*/

    /* mt = {} */
    lua_createtable(L, 0,0); /* mt */
    /*stack = c, parents, mt*/
    /* mt.__index = closure(myclass__index, parents) */
    lua_pushvalue(L, -2); /* Empurra 'parents' (upvalue) */  
    /*stack = c, parents, mt, parents*/
    lua_pushcclosure(L, myclass__index, 1); /*stack = c, parents, mt, parents, myclass__index*/
    lua_setfield(L, -2, "__index"); /* Define mt.__index, pop myclass__index e parents upvalue */
    
    /*stack = c, parents, mt*/
    
    /* mt.__call = myclass__call */
    lua_pushcfunction(L, myclass__call);
    lua_setfield(L, -2, "__call");
    
    /* setmetatable(c, mt) */
    lua_setmetatable(L, -3); /* -3 e 'c' */
    /*stack = c, parents*/
    
    /* Pilha: [P1] [P2] ... [c] [parents] */
    
    lua_remove(L, -1); /* Remove 'parents' */
    
    /* Pilha: [P1] [P2] ... [c] */
    
    /* c.__index = c (para encontrar metodos em 'c' primeiro) */
    lua_pushvalue(L, -1);  /* Duplica 'c' */
    lua_setfield(L, -2, "__index");
    
    /* Retorna 'c' */
    return 1;
}


#else
    #include "lclass.h"
#endif

static const luaL_Reg base_funcs[] = {
#ifdef MYCLASS_LIB
  {"Classe", luaB_class},
#endif
  {"contador", newCounter}, /*----------- inicio BETO beto modification --------*/
  {"escolha", switch_func},
  {"tente", luaB_assert},
  /* -------------- BETO IMPLEMENTATION - funções de checagem de tipo ----------------*/
  /* Caso o valor seja do tipo certo, o retorno é o próprio valor, senão o programa encerra com erro */
  /* aceita parâmetros variados: local n1, n2, n3 = N(1,2,3); */
  {"Th" , luaB_check_thread},/*=='thread'*/
  {"N", luaB_check_number}, /* n = N(n); --> (tipo(n)=='numero')*/
  {"S", luaB_check_string},/*s = S(s); --> tipo(s)=='string'*/
  {"F", luaB_check_function},/*=='funcao'*/
  {"T", luaB_check_table},/*=='tabela'*/
  {"B", luaB_check_boolean},/*=='booleano'*/
  {"U", luaB_check_userdata},/*=='userdata'*/
  {"V", luaB_check_var}, /*=='valor'-->local n = V('string', 1);-->erro espera-se string em vez de número!*/
  /* -------------- FIM BETO IMPLEMENTATION - funções de checagem de tipo ----------------*/
  
  {"coletelixo", luaB_collectgarbage},
  {"executearquivo", luaB_dofile},
  {"erro", luaB_error},
  {"obtmetatabela", luaB_getmetatable},
  {"ipares", luaB_ipairs},
  {"carreguearquivo", luaB_loadfile},
  {"carregue", luaB_load},
  {"executestring",  luaB_dostring},
  {"proximo", luaB_next},
  {"pares", luaB_pairs},
  {"pchame", luaB_pcall},
  {"imprima", luaB_print},
  {"imprimaf", luaB_printf},/*/printf("fmt", ...);*/
  {"poe" , luaB_puts },
  {"igual", luaB_rawequal},/* igual(a,b) é o mesmo que a == b, mas sem evocar metametodos.*/
  {"tamanho", luaB_rawlen},/*retorna o tamanho de string ou tabela.*/
  {"obt", luaB_rawget},/*ret = obt(tab,1) é o mesmo que ret = tab[1],  porém não executa metamétodos.*/
  {"def", luaB_rawset},/*def(tab,1,'val'); ou def(tab,'n',1000 ); ( tab[1]='val'; tab.n = 1000*/
  {"selecione", luaB_select},
  {"defmetatabela", luaB_setmetatable},
  {"convnumero", luaB_tonumber},
  {"convstring", luaB_tostring},
  {"tipo", luaB_type},  /* *11/2023 -- permite a chamada do metamétodo __tipo(); para dados */
                        /*             definidos pelo usuário. */
  {"xpchame", luaB_xpcall}, /*----------- final BETO Beto beto modification --------*/


  {NULL, NULL}
};


LUAMOD_API int luaopen_base (lua_State *L) {
  /* set global _G */
  lua_pushglobaltable(L);
  lua_pushglobaltable(L);
  lua_setfield(L, -2, "_G");
  /* open lib into global table */
  luaL_setfuncs(L, base_funcs, 0);
  #ifndef MYCLASS_LIB
      lcf_classe (L); /* set Global Classe(...)  BETO MODIFICATION -> corrigido o problema com _G, apenas usei um lua_pop(L,1) em lclasse.h*/
  #endif
  return 1;
}

