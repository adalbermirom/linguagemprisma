/*
** $Id: ltablib.c,v 1.65.1.1 2013/04/12 18:48:47 roberto Exp $
** Library for Table Manipulation
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define ltablib_c
#define LUA_LIB

#include "lua.h"
#include "prisma.h"

#include "lauxlib.h"
#include "lualib.h"


#define aux_getn(L,n)	(luaL_checktype(L, n, LUA_TTABLE), luaL_len(L, n))



#if defined(LUA_COMPAT_MAXN)
static int maxn (lua_State *L) {
  lua_Number max = 0;
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_pushnil(L);  /* first key */
  while (lua_next(L, 1)) {
    lua_pop(L, 1);  /* remove value */
    if (lua_type(L, -1) == LUA_TNUMBER) {
      lua_Number v = lua_tonumber(L, -1);
      if (v > max) max = v;
    }
  }
  lua_pushnumber(L, max);
  return 1;
}
#endif


static int tinsert (lua_State *L) {
  int e = aux_getn(L, 1) + 1;  /* first empty element */
  int pos;  /* where to insert new element */
  switch (lua_gettop(L)) {
    case 2: {  /* called with only 2 arguments */
      pos = e;  /* insert new element at the end */
      break;
    }
    case 3: {
      int i;
      pos = luaL_checkint(L, 2);  /* 2nd argument is the position */
      luaL_argcheck(L, 1 <= pos && pos <= e, 2, "posicao fora dos limites");
      for (i = e; i > pos; i--) {  /* move up elements */
        lua_rawgeti(L, 1, i-1);
        lua_rawseti(L, 1, i);  /* t[i] = t[i-1] */
      }
      break;
    }
    default: {
      return luaL_error(L, "numero incorreto de argumentos para " LUA_QL("insira"));
    }
  }
  lua_rawseti(L, 1, pos);  /* t[pos] = v */
  return 0;
}


static int tremove (lua_State *L) {
  int size = aux_getn(L, 1);
  int pos = luaL_optint(L, 2, size);
  if (pos != size)  /* validate 'pos' if given */
    luaL_argcheck(L, 1 <= pos && pos <= size + 1, 1, "posicao fora dos limites");
  lua_rawgeti(L, 1, pos);  /* result = t[pos] */
  for ( ; pos < size; pos++) {
    lua_rawgeti(L, 1, pos+1);
    lua_rawseti(L, 1, pos);  /* t[pos] = t[pos+1] */
  }
  lua_pushnil(L);
  lua_rawseti(L, 1, pos);  /* t[pos] = nil */
  return 1;
}


static void addfield (lua_State *L, luaL_Buffer *b, int i) {
  lua_rawgeti(L, 1, i);
  if (!lua_isstring(L, -1))
    luaL_error(L, "valor invalido (%s) no indice %d na tabela para "
                  LUA_QL("concat"), luaL_typename(L, -1), i);
  luaL_addvalue(b);
}


static int tconcat (lua_State *L) {
  luaL_Buffer b;
  size_t lsep;
  int i, last;
  const char *sep = luaL_optlstring(L, 2, "", &lsep);
  luaL_checktype(L, 1, LUA_TTABLE);
  i = luaL_optint(L, 3, 1);
  last = luaL_opt(L, luaL_checkint, 4, luaL_len(L, 1)); 
  luaL_buffinit(L, &b);
  for (; i < last; i++) {
    addfield(L, &b, i);
    luaL_addlstring(&b, sep, lsep);
  }
  if (i == last)  /* add last value (if interval was not empty) */
    addfield(L, &b, i);
  luaL_pushresult(&b);
  return 1;
}


/*
** {======================================================
** Pack/unpack
** =======================================================
*/

static int pack (lua_State *L) {
  int n = lua_gettop(L);  /* number of elements to pack */
  lua_createtable(L, n, 1);  /* create result table */
  lua_pushinteger(L, n);
  lua_setfield(L, -2, "n");  /* t.n = number of elements */
  if (n > 0) {  /* at least one element? */
    int i;
    lua_pushvalue(L, 1);
    lua_rawseti(L, -2, 1);  /* insert first element */
    lua_replace(L, 1);  /* move table into index 1 */
    for (i = n; i >= 2; i--)  /* assign other elements */
      lua_rawseti(L, 1, i);
  }
  return 1;  /* return table */
}


static int unpack (lua_State *L) {
  int i, e, n;
  luaL_checktype(L, 1, LUA_TTABLE);
  i = luaL_optint(L, 2, 1);
  e = luaL_opt(L, luaL_checkint, 3, luaL_len(L, 1));
  if (i > e) return 0;  /* empty range */
  n = e - i + 1;  /* number of elements */
  if (n <= 0 || !lua_checkstack(L, n))  /* n <= 0 means arith. overflow */
    return luaL_error(L, "muitos resultados para desempacotar");
  lua_rawgeti(L, 1, i);  /* push arg[i] (avoiding overflow problems) */
  while (i++ < e)  /* push arg[i + 1...e] */
    lua_rawgeti(L, 1, i);
  return n;
}

/* }====================================================== */



/*
** {======================================================
** Quicksort
** (based on `Algorithms in MODULA-3', Robert Sedgewick;
**  Addison-Wesley, 1993.)
** =======================================================
*/


static void set2 (lua_State *L, int i, int j) {
  lua_rawseti(L, 1, i);
  lua_rawseti(L, 1, j);
}

static int sort_comp (lua_State *L, int a, int b) {
  if (!lua_isnil(L, 2)) {  /* function? */
    int res;
    lua_pushvalue(L, 2);
    lua_pushvalue(L, a-1);  /* -1 to compensate function */
    lua_pushvalue(L, b-2);  /* -2 to compensate function and `a' */
    lua_call(L, 2, 1);
    res = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return res;
  }
  else  /* a < b? */
    return lua_compare(L, a, b, LUA_OPLT);
}

static void auxsort (lua_State *L, int l, int u) {
  while (l < u) {  /* for tail recursion */
    int i, j;
    /* sort elements a[l], a[(l+u)/2] and a[u] */
    lua_rawgeti(L, 1, l);
    lua_rawgeti(L, 1, u);
    if (sort_comp(L, -1, -2))  /* a[u] < a[l]? */
      set2(L, l, u);  /* swap a[l] - a[u] */
    else
      lua_pop(L, 2);
    if (u-l == 1) break;  /* only 2 elements */
    i = (l+u)/2;
    lua_rawgeti(L, 1, i);
    lua_rawgeti(L, 1, l);
    if (sort_comp(L, -2, -1))  /* a[i]<a[l]? */
      set2(L, i, l);
    else {
      lua_pop(L, 1);  /* remove a[l] */
      lua_rawgeti(L, 1, u);
      if (sort_comp(L, -1, -2))  /* a[u]<a[i]? */
        set2(L, i, u);
      else
        lua_pop(L, 2);
    }
    if (u-l == 2) break;  /* only 3 elements */
    lua_rawgeti(L, 1, i);  /* Pivot */
    lua_pushvalue(L, -1);
    lua_rawgeti(L, 1, u-1);
    set2(L, i, u-1);
    /* a[l] <= P == a[u-1] <= a[u], only need to sort from l+1 to u-2 */
    i = l; j = u-1;
    for (;;) {  /* invariant: a[l..i] <= P <= a[j..u] */
      /* repeat ++i until a[i] >= P */
      while (lua_rawgeti(L, 1, ++i), sort_comp(L, -1, -2)) {
        if (i>=u) luaL_error(L, "funcao organizadora invalida para ordenar");
        lua_pop(L, 1);  /* remove a[i] */
      }
      /* repeat --j until a[j] <= P */
      while (lua_rawgeti(L, 1, --j), sort_comp(L, -3, -1)) {
        if (j<=l) luaL_error(L, "funcao organizadora invalida para ordenar");
        lua_pop(L, 1);  /* remove a[j] */
      }
      if (j<i) {
        lua_pop(L, 3);  /* pop pivot, a[i], a[j] */
        break;
      }
      set2(L, i, j);
    }
    lua_rawgeti(L, 1, u-1);
    lua_rawgeti(L, 1, i);
    set2(L, u-1, i);  /* swap pivot (a[u-1]) with a[i] */
    /* a[l..i-1] <= a[i] == P <= a[i+1..u] */
    /* adjust so that smaller half is in [j..i] and larger one in [l..u] */
    if (i-l < u-i) {
      j=l; i=i-1; l=i+2;
    }
    else {
      j=i+1; i=u; u=j-2;
    }
    auxsort(L, j, i);  /* call recursively the smaller one */
  }  /* repeat the routine for the larger one */
}

static int sort (lua_State *L) {
  int n = aux_getn(L, 1);
  luaL_checkstack(L, 40, "");  /* assume array is smaller than 2^40 */
  if (!lua_isnoneornil(L, 2))  /* is there a 2nd argument? */
    luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_settop(L, 2);  /* make sure there is two arguments */
  auxsort(L, 1, n);
  return 0;
}

/* }====================================================== */

#define ARRAY_META_NAME "METATABLE::ARRAY::LIST::FUNCS"
/*
** {======================================================
** Reduce function
** BETO MODIFICATION
** Adalberto Amorim Felipe 2024.)
** =======================================================
*/
static int reduce(lua_State *L);
static int map(lua_State *L);
static int filter(lua_State *L);
static int copy(lua_State *L);
static int push(lua_State *L);
static int merge(lua_State *L);


/* to use internally */
static void create_list(lua_State *L, int len){
	lua_createtable(L, len, 0); /* table is at top */
	/*table deve estar no topo agora*/
	/*Veja prisma.h para macros como essa: PRIS_*   */
	PRIS_SET_META(L, ARRAY_META_NAME); /*MT criada e adicionada na tabela, MT fora do topo*/
	/*table continua no topo*/
	return;
}



static int reduce(lua_State *L){
	/* 1 arg = function? */
	/* 2 arg = arr table */
	int i = 0, last=0;
	luaL_checktype(L, 1, LUA_TTABLE);
		luaL_checktype(L, 2, LUA_TFUNCTION);
		
	if(lua_gettop(L) >= 3){ /*o valor incial foi passado como arg 3;*/
		i = 1;
		luaL_checkany(L, 3); /*valor inicial já está no topo em 3*/
	}else{
		/* não foi passado valor inicial, logo pegamos o primeiro elemento da tabela.*/
	    lua_rawgeti(L, 1,1); /* idx 3 is t[1]; */
	    if (lua_isnil(L, -1)) {
        // Tabela está vazia e não há valor inicial
            return luaL_error(L, "Redução de array vazia sem valor inicial"); 
        }
	    i = 2;
	}
	
	last = luaL_len(L, 1);

	for(; i <= last; i++){ /* i já está definido acima. 1 se arg3 (inicial) ou 2 se nao arg3  */
		/* printf("idx: %d\n", i); */
		lua_pushvalue(L,2); /* function on top; */
		lua_pushvalue(L,3); /* 1 arg = total */
	    lua_rawgeti(L, 1, i); /* 2 arg = curr index from table;*/
	    lua_pushvalue(L, 1); /* 3 arg = self table; */
	    lua_call(L,3,1); /* call func with index from a table arr; */
	    lua_copy(L, -1, 3); /* copy the result to idx 3 on stack; */
	    lua_pop(L, 1); /* pops the ret */
	}
    /* return is at top now */
	return 1;	
}

/* }====================================================== */


/*
** {======================================================
** Map function
** BETO MODIFICATION
** Adalberto Amorim Felipe 2024.)
* tab.map(arr, function(currentValue, index, arr) )
* 
* create an array, does not change the original table;
** =======================================================
*/
static int map(lua_State *L){
	luaL_checktype(L, 1, LUA_TTABLE);  /* 1 must be a table (array table) */
	luaL_checktype(L, 2, LUA_TFUNCTION); /* 2 must be a function */
	int last = luaL_len(L, 1);
    create_list(L, last); /* 3 is a table-list to return. / create a table to return; */
	int i;
	for(i=1;i<=last;i++){
		lua_pushvalue(L, 2); /* function on top */
		lua_rawgeti(L, 1, i); /* arg 1 index from table. // top = tab[i]; */
		lua_pushinteger(L, i); /* arg 2 i idx of current value from table; */
		lua_pushvalue(L, 1); /*  arg 3 table self */
		lua_call(L, 3, 1); /* call a function with 3 args, retuns 1 */
		lua_rawseti(L, 3, i); /* set ret[i] = topvalue; and popped the ret func; */
	}
	/* table return is on top */
	return 1;
}

/* }====================================================== */



/*
** {======================================================
** filter function
** BETO MODIFICATION
** Adalberto Amorim Felipe 2024.)
* tab.filter(tab, function(current, index, arr) )
** =======================================================
*/

static int filter(lua_State *L){
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	create_list(L, 0); /* 3 is table to be returned */
	
	int last = luaL_len(L, 1); /* length of table (arg1) */
	int i, i2=1;
	for(i=1;i<=last;i++){
		lua_pushvalue(L,2);/* function on top */
		lua_rawgeti(L,1, i); /* arg1 = current element from table*/
		lua_pushinteger(L, i); /* arg2 = current idx */
		lua_pushvalue(L, 1);  /* arg3 = table self */
		lua_call(L, 3, 1); /*call func, pops its args and push value (ret 1) return into top of stack */
		if(lua_toboolean(L,-1)){ /* filter func return true then index item into new table. */
			lua_rawgeti(L, 1, i);/* get the i idx from table in */
			lua_rawseti(L, 3, i2); /* tab_ret[#tab_ret+1] = tab_in[i] */
			i2++;			
		}	
		lua_pop(L, 1);	
	}
	/* table 3 is on top */
	return 1;
}

/* }====================================================== */



/*
** {======================================================
** copy function
** BETO MODIFICATION
** Adalberto Amorim Felipe 2024.)
* tab2 = tabela.copie(tab)
* Copia uma tabela do tipo array para uma nova.
* Atenção: a copia só tem efeito na parte array(idx/value) da tabela, na parte key/value não copia.
** =======================================================
*/

static int copy(lua_State*L){
	luaL_checktype(L, 1, LUA_TTABLE); /* 1 must be a table */
	int last = luaL_len(L, 1); /* length of the table 1 */
	int start = 1, finish = last;
	int idx_tab_copy = lua_gettop(L)+1;
	
	if( lua_isnumber(L,2)){ start = lua_tointeger(L,2);}
	if( lua_isnumber(L,3)){ finish = lua_tointeger(L,3);}
	
	if(start<0) start = (last + start) + 1;
	if(finish<0) finish = (last + finish) + 1;
	
	// Garante que 'start' nao comece antes de 1.
    if (start < 1) start = 1; 
    // Garante que 'finish' nao vá alem de 'last'.
    if (finish > last) finish = last; 

    // Garante que start nao passe finish (evita loop inverso)
    if (start > finish) {
        create_list(L, 0); // Cria e retorna uma lista vazia
        return 1;
    }
	
	create_list(L, last); /* is new table on top; | creates a new table with len of table 1; */
	
	unsigned int i, cont=1;
	for(i=start; i<=finish;i++){
		lua_rawgeti(L, 1, i); /* get i index from table arg 1, puts it on top */
		lua_rawseti(L, idx_tab_copy, cont++); /* makes a copy from table 1 to table 2; | pops element from lua_rawgeti() */
	}
	/* copies idx table items to new table and returns it , without metamethods */
	return 1;
}

/* }====================================================== */




/*
** {======================================================
** push / append /add function
** BETO MODIFICATION
** Adalberto Amorim Felipe 2024.)
*  anexa um valor no final da tabela na parte array;
*  aceita número variado de parâmetros:  t = tabela.adicione(1,2,3,4,5...);
*  Obs. Esta função não cria uma nova tabela, retorna a mesma passada como arg 1;
** =======================================================
*/

static int push(lua_State*L){
	luaL_checktype(L, 1, LUA_TTABLE); /* Tabela (tab) no índice 1 */
	
	int total = lua_gettop(L);
	int last = luaL_len(L, 1);
	for (int i = 2; i <= total; i++ ){
	    luaL_checkany(L, i); /* Qualquer valor no índice*/
	    lua_pushvalue(L, i);
	    last = last + 1;
	    lua_rawseti(L, 1, last); 
    }	
	return 1; /* Tabela array no topo da pilha */
}

/* }====================================================== */



/*
** {======================================================
** merge function
** BETO MODIFICATION
** Adalberto Amorim Felipe 2024.)
*  Junta dois arrays em um novo array:    
*   t = tabela.junte({1,2,3}, {4,5,6} [, {...} ...]); // --> t = {1,2,3,4,5,6...};
** =======================================================
*/

static int merge(lua_State*L){
	
	int nargs = lua_gettop(L); /* number of arguments */
	int new_table = nargs + 1;/* last is the new table */
	unsigned int total_len=0; /*sum of length of all tables*/	
	unsigned int cur_table, idx_dest=1, idx_current=1, len; /* variables to use in the loops for below */
		
	for(cur_table = 1; cur_table <= nargs; cur_table++){
		luaL_checktype(L, cur_table, LUA_TTABLE); /* stack[i] is table? */
		total_len += luaL_len(L, cur_table); /* then total_len = total_len + current_tab_len */
	}    

	create_list(L, total_len); /* creates a table <array> at the top now! */

    for (cur_table=1; cur_table<=nargs; cur_table++){
	    len = luaL_len(L, cur_table); /* length of the current table */
	    for(idx_current = 1; idx_current <= len; idx_current++){ /*copy each element from current table. */
			lua_rawgeti(L, cur_table, idx_current); /* get the index from current table; */
			lua_rawseti(L, new_table, idx_dest++);
		}
		
	}
	
	return 1; /* return table */
}

/* }====================================================== */


static const luaL_Reg tab_funcs[] = {
  {"concat", tconcat},
#if defined(LUA_COMPAT_MAXN)
  {"maxn", maxn},
#endif
  {"insira", tinsert},
  {"empacote", pack},
  {"desempacote", unpack},
  {"remova", tremove},
  {"ordene", sort},
  {"reduza", reduce}, 
  {"map", map},
  {"filtre", filter},
  {"copie", copy},
  {"adicione", push},
  {"junte", merge},
  {"crie", copy},
  {NULL, NULL}
};


LUAMOD_API int luaopen_table (lua_State *L) {
  luaL_newlib(L, tab_funcs);
/*#if defined(LUA_COMPAT_UNPACK)*/
  /* _G.unpack = table.unpack */
  lua_getfield(L, -1, "desempacote");
  lua_setglobal(L, "desempacote");
  PRIS_REGISTER_META(L, ARRAY_META_NAME, tab_funcs);
  return 1;
}



