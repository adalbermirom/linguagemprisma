/*
** $Id: lmathlib.c,v 1.83.1.1 2013/04/12 18:48:47 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/


#include <stdlib.h>
#include <math.h>

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#undef PI
#define PI	((lua_Number)(3.1415926535897932384626433832795))
#define RADIANS_PER_DEGREE	((lua_Number)(PI/180.0))



static int math_abs (lua_State *L) {
  lua_pushnumber(L, l_mathop(fabs)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sin (lua_State *L) {
  lua_pushnumber(L, l_mathop(sin)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sinh (lua_State *L) {
  lua_pushnumber(L, l_mathop(sinh)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_cos (lua_State *L) {
  lua_pushnumber(L, l_mathop(cos)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_cosh (lua_State *L) {
  lua_pushnumber(L, l_mathop(cosh)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tan (lua_State *L) {
  lua_pushnumber(L, l_mathop(tan)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tanh (lua_State *L) {
  lua_pushnumber(L, l_mathop(tanh)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_asin (lua_State *L) {
  lua_pushnumber(L, l_mathop(asin)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_acos (lua_State *L) {
  lua_pushnumber(L, l_mathop(acos)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_atan (lua_State *L) {
  lua_pushnumber(L, l_mathop(atan)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_atan2 (lua_State *L) {
  lua_pushnumber(L, l_mathop(atan2)(luaL_checknumber(L, 1),
                                luaL_checknumber(L, 2)));
  return 1;
}

static int math_ceil (lua_State *L) {
  lua_pushnumber(L, l_mathop(ceil)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_floor (lua_State *L) {
  lua_pushnumber(L, l_mathop(floor)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_fmod (lua_State *L) {
  lua_pushnumber(L, l_mathop(fmod)(luaL_checknumber(L, 1),
                               luaL_checknumber(L, 2)));
  return 1;
}

static int math_modf (lua_State *L) {
  lua_Number ip;
  lua_Number fp = l_mathop(modf)(luaL_checknumber(L, 1), &ip);
  lua_pushnumber(L, ip);
  lua_pushnumber(L, fp);
  return 2;
}

static int math_sqrt (lua_State *L) {
  lua_pushnumber(L, l_mathop(sqrt)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_pow (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  lua_Number y = luaL_checknumber(L, 2);
  lua_pushnumber(L, l_mathop(pow)(x, y));
  return 1;
}

static int math_root (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  lua_Number y = luaL_checknumber(L, 2);
  lua_pushnumber(L, l_mathop(pow)(x, 1/y));
  return 1;
}

static int math_log (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  lua_Number res;
  if (lua_isnoneornil(L, 2))
    res = l_mathop(log)(x);
  else {
    lua_Number base = luaL_checknumber(L, 2);
    if (base == (lua_Number)10.0) res = l_mathop(log10)(x);
    else res = l_mathop(log)(x)/l_mathop(log)(base);
  }
  lua_pushnumber(L, res);
  return 1;
}

#if defined(LUA_COMPAT_LOG10)
static int math_log10 (lua_State *L) {
  lua_pushnumber(L, l_mathop(log10)(luaL_checknumber(L, 1)));
  return 1;
}
#endif

static int math_exp (lua_State *L) {
  lua_pushnumber(L, l_mathop(exp)(luaL_checknumber(L, 1)));
  return 1;
}

static int math_deg (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)/RADIANS_PER_DEGREE);
  return 1;
}

static int math_rad (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)*RADIANS_PER_DEGREE);
  return 1;
}

static int math_frexp (lua_State *L) {
  int e;
  lua_pushnumber(L, l_mathop(frexp)(luaL_checknumber(L, 1), &e));
  lua_pushinteger(L, e);
  return 2;
}

static int math_ldexp (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  int ep = luaL_checkint(L, 2);
  lua_pushnumber(L, l_mathop(ldexp)(x, ep));
  return 1;
}



static int math_min (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmin = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d < dmin)
      dmin = d;
  }
  lua_pushnumber(L, dmin);
  return 1;
}


static int math_max (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmax = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d > dmax)
      dmax = d;
  }
  lua_pushnumber(L, dmax);
  return 1;
}


static int math_random (lua_State *L) {
  /* the `%' avoids the (rare) case of r==1, and is needed also because on
     some systems (SunOS!) `rand()' may return a value larger than RAND_MAX */
  lua_Number r = (lua_Number)(rand()%RAND_MAX) / (lua_Number)RAND_MAX;
  switch (lua_gettop(L)) {  /* check number of arguments */
    case 0: {  /* no arguments */
      lua_pushnumber(L, r);  /* Number between 0 and 1 */
      break;
    }
    case 1: {  /* only upper limit */
      lua_Number u = luaL_checknumber(L, 1);
      luaL_argcheck(L, (lua_Number)1.0 <= u, 1, "intervalo esta vazio");
      lua_pushnumber(L, l_mathop(floor)(r*u) + (lua_Number)(1.0));  /* [1, u] */
      break;
    }
    case 2: {  /* lower and upper limits */
      lua_Number l = luaL_checknumber(L, 1);
      lua_Number u = luaL_checknumber(L, 2);
      luaL_argcheck(L, l <= u, 2, "intervalo esta vazio");
      lua_pushnumber(L, l_mathop(floor)(r*(u-l+1)) + l);  /* [l, u] */
      break;
    }
    default: return luaL_error(L, "numero incorreto de argumentos");
  }
  return 1;
}


static int math_randomseed (lua_State *L) {
  srand(luaL_checkunsigned(L, 1));
  (void)rand(); /* discard first value to avoid undesirable correlations */
  return 0;
}

/*  fallback para compiladores que não suportam c99 das funções round, hypot e cbrt */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    /*
     * ESTAMOS EM MODO C99 (OU C11, C17, etc.).
     * O compilador SUPORTA C99.
     */
    #define _C99_UP
#endif


#ifndef _C99_UP
  /* Fallback C89 outro que não seja >= C99 para round() */
static double round(double x) {
    /*
     * floor(1.5 + 0.5) -> floor(2.0) -> 2.0
     * floor(1.4 + 0.5) -> floor(1.9) -> 1.0
     * ceil(-1.5 - 0.5) -> ceil(-2.0) -> -2.0
     * ceil(-1.4 - 0.5) -> ceil(-1.9) -> -1.0
     */
    if (x > 0.0)
        return floor(x + 0.5);
    else
        return ceil(x - 0.5);
}


/* Fallback C89 para hypot() */
static double hypot(double x, double y) {
	/*
     * CUIDADO: pode falhar com numeros astronomicamente grandes.
     */
    return sqrt(x*x + y*y);
}

/* Fallback C89 para cbrt() */
static double cbrt(double x) {
    if (x == 0.0)
        return 0.0;
    else if (x < 0.0)
        /* Se for negativo, tira a raiz do positivo e devolve o sinal */
        return -pow(-x, 1.0/3.0);
    else
        /* Se for positivo, e so o pow() normal */
        return pow(x, 1.0/3.0);
}

#endif


static int math_round(lua_State*L){
    double ret = l_mathop(round)(luaL_checknumber(L,1));
    lua_pushnumber(L, ret);  
    return 1;
}

/*Beto prisma modification raiz cúbica de um decimal*/

static int math_cbrt (lua_State*L){ /*[Mathematics]  ∛x = cbrt(x) [In C Programming]*/
    double ret = l_mathop(cbrt)(luaL_checknumber(L,1));
    lua_pushnumber(L,ret);
	return 1;
}

/*Beto prisma modification hipotenusa*/
static int math_hypot(lua_State*L){
    double p, b;
    double hypotenuse;
    p = luaL_checknumber(L,1);
    b = luaL_checknumber(L,2);
                /*#define l_mathop(x)  (x)   em luaconf.h*/
    hypotenuse = l_mathop(hypot)(p, b);
    lua_pushnumber(L, hypotenuse);
    return 1;
}


static const luaL_Reg mathlib[] = {
  {"absoluto",   math_abs}, /*/ (x) absoluto*/
  {"arcocosseno",  math_acos}, /*/ (x) arco co-seno radianos*/
  {"arcoseno",  math_asin}, /*/(x)  arco seno radianos*/
  {"arcotangente2", math_atan2}, /*/ (y/x) arco tangente radianos*/
  {"arcotangente",  math_atan}, /*/ (x) arco tangente radianos*/
  {"arredondeacima",  math_ceil}, /*/ arredonda para mais*/
  {"cossenoh",   math_cosh}, /*/ (x) co-seno hiperpolico*/
  {"cosseno",   math_cos}, /*/ co-seno (radiandos)*/
  {"emGrau",   math_deg}, /*/ de radianos para grau*/
  {"exp",   math_exp},
  {"arredondeabaixo", math_floor}, /*/arredonda por baixo: 4.5 = 4*/
  {"corte", math_floor}, /*/para retro compatibilidade.*/
  {"cmodulo",   math_fmod}, /*/corte modulo  modulo por baixo*/
  {"frexp", math_frexp},
  {"ldexp", math_ldexp},
#if defined(LUA_COMPAT_LOG10)
  {"log10", math_log10},/*log10(x) logaritmo base 10 de x*/
#endif
  {"log",   math_log}, /*log(x) retorna o logaritmo natural de x*/
  {"maximo",   math_max}, /*max(a,b,...) retorna o maior numero entre os argumentos*/
  {"minimo",   math_min}, /*min(a,b,...) o menor dentre os argumentos*/
  {"separe",   math_modf},/* separa a parte inteira da fracionaria de um numero com ponto decimal*/
  {"elevado",   math_pow},/* elevado(a,b) a elevado a b*/
  {"raiz" , math_root } , /* raiz y de x ( x , y )*/
  {"emRadianos",   math_rad}, /*rad(x) converte de graus para radianos*/
  {"randonico",     math_random}, /*(a,b) pseudo randonico entre a e b*/
  {"xrandonico", math_randomseed},/* depois*/
  {"aleatorio",     math_random}, /*aleatorio(a,b) pseudo randonico entre a e b*/
  {"xaleatorio", math_randomseed},/* incrementa uma semente x randomica para a função aleatório.*/
  {"senoh",   math_sinh}, /* seno hiperbolico*/
  {"seno",   math_sin}, /* seno*/
  {"raizquad",  math_sqrt}, /*raiz quadrada*/
  {"tangenteh",   math_tanh}, /*tangente hiperbolica*/
  {"tangente",   math_tan},
  {"arredonde", math_round},
  {"raizcubica", math_cbrt}, /* retorna a raiz cúbica de um número:   local cub = mat.raizcubica(1000); //--> 10;*/
  {"hipot", math_hypot}, /*calcula a hipotenusa dado ca e co:  local h = mat.hipot(2.5, 6.2);*/
  {NULL, NULL}
};


/*
** Open math library
*/
LUAMOD_API int luaopen_math (lua_State *L) {
  luaL_newlib(L, mathlib);
  lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "infinito");
  return 1;
}
