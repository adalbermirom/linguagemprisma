/*
** $Id: loslib.c,v 1.40.1.1 2013/04/12 18:48:47 roberto Exp $
** Standard Operating System library
** See Copyright Notice in lua.h
*/


#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h> /* file size Prisma: sis.tamanho(arquivo);*/

#define loslib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "osname/osname.h"
#include "osname/archname.h"
/* Prisma funciona em Linux, Windows e Navegadores modernos(Web-assembly), para outras plataformas pode ser necessário adaptações.*/

/*/========================================= Adalberto*/

#ifdef __linux
 #define _HOME_PATH "HOME"
 #define _USER "USER"
#endif


/*#ifdef _WIN32*/

#if defined (_os_win)
 #define __win__
 #include<windows.h>
 #define _HOME_PATH "HomePath"
 #define _USER "USERNAME"
  /*setenv wrap for windows*/
 #ifndef _WIN_XP_

int setenv(const char *name, const char *value, int overwrite)
{
	
    int errcode = 0;
    if(!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if(errcode || envsize) return errcode;
    }
    return _putenv_s(name, value);
}
 #else /*xp*/
int setenv(const char *name, const char *value, int overwrite)
{
    if(!overwrite) {
        const char *val = getenv(name);
        if(val==NULL) return -1;
    }
    return SetEnvironmentVariable(name, value);
}
 #endif
#endif


#define FALSE 0
#define TRUE 1
/*/===================================== fim Adalberto*/

/*
** list of valid conversion specifiers for the 'strftime' function
*/
#if !defined(LUA_STRFTIMEOPTIONS)

#if !defined(LUA_USE_POSIX)
#define LUA_STRFTIMEOPTIONS	{ "aAbBcdHIjmMpSUwWxXyYz%", "" }
#else
#define LUA_STRFTIMEOPTIONS \
	{ "aAbBcCdDeFgGhHIjmMnprRStTuUVwWxXyYzZ%", "" \
	  "", "E", "cCxXyY",  \
	  "O", "deHImMSuUVwWy" }
#endif

#endif



/*
** By default, Lua uses tmpnam except when POSIX is available, where it
** uses mkstemp.
*/
#if defined(LUA_USE_MKSTEMP)
#include <unistd.h>
#define LUA_TMPNAMBUFSIZE	32
#define lua_tmpnam(b,e) { \
        strcpy(b, "/tmp/pri_XXXXXX"); \
        e = mkstemp(b); \
        if (e != -1) close(e); \
        e = (e == -1); }

#elif !defined(lua_tmpnam)

#define LUA_TMPNAMBUFSIZE	L_tmpnam
#define lua_tmpnam(b,e)		{ e = (tmpnam(b) == NULL); }

#endif


/*
** By default, Lua uses gmtime/localtime, except when POSIX is available,
** where it uses gmtime_r/localtime_r
*/
#if defined(LUA_USE_GMTIME_R)

#define l_gmtime(t,r)		gmtime_r(t,r)
#define l_localtime(t,r)	localtime_r(t,r)

#elif !defined(l_gmtime)

#define l_gmtime(t,r)		((void)r, gmtime(t))
#define l_localtime(t,r)  	((void)r, localtime(t))

#endif



static int os_execute (lua_State *L) {
  const char *cmd = luaL_optstring(L, 1, NULL);
  int stat = system(cmd);
  if (cmd != NULL)
    return luaL_execresult(L, stat);
  else {
    lua_pushboolean(L, stat);  /* true if there is a shell */
    return 1;
  }
}


static int os_remove (lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  return luaL_fileresult(L, remove(filename) == 0, filename);
}


static int os_rename (lua_State *L) {
  const char *fromname = luaL_checkstring(L, 1);
  const char *toname = luaL_checkstring(L, 2);
  return luaL_fileresult(L, rename(fromname, toname) == 0, NULL);
}


static int os_tmpname (lua_State *L) {
  char buff[LUA_TMPNAMBUFSIZE];
  int err;
  lua_tmpnam(buff, err);
  if (err)
    return luaL_error(L, "incapaz de gerar um nome unico");
  lua_pushstring(L, buff);
  return 1;
}


static int os_getenv (lua_State *L) {
  lua_pushstring(L, getenv(luaL_checkstring(L, 1)));  /* if NULL push nil */
  return 1;
}

/*/============================ Adalberto*/


static int os_setenv(lua_State *L) {
    const char *envname = luaL_checkstring(L,1);
    const char *envval = luaL_checkstring(L,2);
    int overwrite = lua_toboolean(L,3); /*/ 0 nao modifica caso haja um valor já atribuido*/
/*/overwrite deve ser diferente de zero para sobreescrever*/

    int ret = setenv(envname,envval,overwrite);
    if ( ret == 0 ) lua_pushboolean(L,TRUE);
    else {
        /**lua_pushboolean(L,FALSE);
        if(ret == EINVAL) lua_pushliteral(L,"argumentos invalidos");
        if(ret == ENOMEN) lua_pushliteral(L, "memoria insuficiente para nova variavel de ambiente");
        return 2; ///retorna falso + msg de erro.
        */
        lua_pushboolean(L,FALSE);
        lua_pushinteger(L, ret);
        return 2;
     }
    return 1; /*/retorna verdadeiro.*/
}

/* =======================================================
 * CÓDIGO LIMPO (SEM CARACTERES INVISÍVEIS)
 * ======================================================= */

#if !defined(_os_win)
/*deleta var ambiente*/
static int os_unsetenv (lua_State *L) {
  const char *name = luaL_checkstring(L,1);
  int ret = unsetenv(name);
  lua_pushinteger(L, ret);
  return 1;
}
#endif


/* --- Pausa o programa (sleep) --- */
#if defined(_os_win)
    #include <windows.h>
#else
    #include <time.h> /* Para nanosleep*/
#endif

void sleep_ms(int milliseconds) /* cross-platform sleep function*/
{
#if defined(_os_win)
    Sleep(milliseconds);
#else
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}


static int os_sleep(lua_State *L){
  int ms = luaL_checkinteger(L, 1);
  sleep_ms(ms);
  return 0;
}
/*/============================ fim Adalberto*/


static int os_clock (lua_State *L) {
  lua_pushnumber(L, ((lua_Number)clock())/(lua_Number)CLOCKS_PER_SEC);
  return 1;
}


/*
** {======================================================
** Time/Date operations
** { year=%Y, month=%m, day=%d, hour=%H, min=%M, sec=%S,
**   wday=%w+1, yday=%j, isdst=? }
** =======================================================
*/

static void setfield (lua_State *L, const char *key, int value) {
  lua_pushinteger(L, value);
  lua_setfield(L, -2, key);
}

static void setboolfield (lua_State *L, const char *key, int value) {
  if (value < 0)  /* undefined? */
    return;  /* does not set field */
  lua_pushboolean(L, value);
  lua_setfield(L, -2, key);
}

static int getboolfield (lua_State *L, const char *key) {
  int res;
  lua_getfield(L, -1, key);
  res = lua_isnil(L, -1) ? -1 : lua_toboolean(L, -1);
  lua_pop(L, 1);
  return res;
}


static int getfield (lua_State *L, const char *key, int d) {
  int res, isnum;
  lua_getfield(L, -1, key);
  res = (int)lua_tointegerx(L, -1, &isnum);
  if (!isnum) {
    if (d < 0)
      return luaL_error(L, "campo " LUA_QS " faltando na tabela de data", key);
    res = d;
  }
  lua_pop(L, 1);
  return res;
}


static const char *checkoption (lua_State *L, const char *conv, char *buff) {
  static const char *const options[] = LUA_STRFTIMEOPTIONS;
  size_t i;
  for (i = 0; i < sizeof(options)/sizeof(options[0]); i += 2) {
    if (*conv != '\0' && strchr(options[i], *conv) != NULL) {
      buff[1] = *conv;
      if (*options[i + 1] == '\0') {  /* one-char conversion specifier? */
        buff[2] = '\0';  /* end buffer */
        return conv + 1;
      }
      else if (*(conv + 1) != '\0' &&
               strchr(options[i + 1], *(conv + 1)) != NULL) {
        buff[2] = *(conv + 1);  /* valid two-char conversion specifier */
        buff[3] = '\0';  /* end buffer */
        return conv + 2;
      }
    }
  }
  luaL_argerror(L, 1,
    lua_pushfstring(L, "especificador de conversao invalido '%%%s'", conv));
  return conv;  /* to avoid warnings */
}


static int os_date (lua_State *L) {
  const char *s = luaL_optstring(L, 1, "%c");
  time_t t = luaL_opt(L, (time_t)luaL_checknumber, 2, time(NULL));
  struct tm tmr, *stm;
  if (*s == '!') {  /* UTC? */
    stm = l_gmtime(&t, &tmr);
    s++;  /* skip `!' */
  }
  else
    stm = l_localtime(&t, &tmr);
  if (stm == NULL)  /* invalid date? */
    lua_pushnil(L);
  else if (strcmp(s, "*t") == 0 || strcmp(s, "*tab") == 0) {
    lua_createtable(L, 0, 9);  /* 9 = number of fields */
    setfield(L, "segundo", stm->tm_sec);
    setfield(L, "minuto", stm->tm_min);
    setfield(L, "hora", stm->tm_hour);
    setfield(L, "dia", stm->tm_mday);
    setfield(L, "mes", stm->tm_mon+1);
    setfield(L, "ano", stm->tm_year+1900);
    setfield(L, "diasemana", stm->tm_wday+1);
    setfield(L, "diaano", stm->tm_yday+1);
    setboolfield(L, "horariodeverao", stm->tm_isdst);
  }else {
    char cc[4];
    luaL_Buffer b;
    cc[0] = '%';
    luaL_buffinit(L, &b);
    while (*s) {
      if (*s != '%')  /* no conversion specifier? */
        luaL_addchar(&b, *s++);
      else {
        size_t reslen;
        char buff[200];  /* should be big enough for any conversion result */
        s = checkoption(L, s + 1, cc);
        reslen = strftime(buff, sizeof(buff), cc, stm);
        luaL_addlstring(&b, buff, reslen);
      }
    }
    luaL_pushresult(&b);
  }
  return 1;
}


static int os_time (lua_State *L) {
  time_t t;
  if (lua_isnoneornil(L, 1))  /* called without args? */
    t = time(NULL);  /* get current time */
  else {
    struct tm ts;
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1);  /* make sure table is at the top */
    ts.tm_sec = getfield(L, "segundo", 0);
    ts.tm_min = getfield(L, "minuto", 0);
    ts.tm_hour = getfield(L, "hora", 12);
    ts.tm_mday = getfield(L, "dia", -1);
    ts.tm_mon = getfield(L, "mes", -1) - 1;
    ts.tm_year = getfield(L, "ano", -1) - 1900;
    ts.tm_isdst = getboolfield(L, "horariodeverao");
   /* t = mktime(&ts);*/
    t = mktime(&ts);
  }
  if (t == (time_t)(-1))
    lua_pushnil(L);
  else
    lua_pushnumber(L, (lua_Number)t);
  return 1;
}


static int os_difftime (lua_State *L) {
  lua_pushnumber(L, difftime((time_t)(luaL_checknumber(L, 1)),
                             (time_t)(luaL_optnumber(L, 2, 0))));
  return 1;
}

/* }====================================================== */


static int os_setlocale (lua_State *L) {
  static const int cat[] = {LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY,
                      LC_NUMERIC, LC_TIME};
  static const char *const catnames[] =  /*{"all", "collate", "ctype", "monetary","numeric", "time", NULL};*/
  {"tudo", "string", "tecla", "moeda", "numerico", "tempo", NULL};

  const char *l = luaL_optstring(L, 1, NULL);
  int op = luaL_checkoption(L, 2, "tudo", catnames);
  lua_pushstring(L, setlocale(cat[op], l));
  return 1;
}


static int os_exit (lua_State *L) {
  int status;
  if (lua_isboolean(L, 1))
    status = (lua_toboolean(L, 1) ? EXIT_SUCCESS : EXIT_FAILURE);
  else
    status = luaL_optint(L, 1, EXIT_SUCCESS);
  if (lua_toboolean(L, 2))
    lua_close(L);
  if (L) exit(status);  /* 'if' to avoid warnings for unreachable 'return' */
  return 0;
}


/* ============================================== Adalberto modification ***/
#define strequal(sa, sb)  (strcmp((sa),(sb))==0)

static int os_size(lua_State *L) {
  const char *s = luaL_checkstring(L,1);
  
  /* Bloco 1: Lógica de Tipo*/
  if(s[0]=='%'){
    
    if(strequal(s,"%i") || strequal(s, "%d")){
        lua_pushunsigned(L, sizeof(int));
        return 1; 
    }else 
    if(strequal(s, "%u")){
        lua_pushunsigned(L, sizeof(unsigned int));
        return 1;
    }else
    if(strequal(s,"%li") || strequal(s,"%ld")){
        lua_pushunsigned(L, sizeof(long int));
        return 1; 
    }else
    if(strequal(s,"%lld") || strequal(s,"%lli")){
        lua_pushunsigned(L, sizeof(long long int));
        return 1; 
    }else
    if(strequal(s,"%lu")){
        lua_pushunsigned(L, sizeof(unsigned long int));
        return 1; 
    }else
    if(strequal(s,"%llu")){
        lua_pushunsigned(L, sizeof(unsigned long long int));
        return 1; 
    }else
    if(strequal(s,"%hi")){
        lua_pushunsigned(L, sizeof(short int));
        return 1; 
    }else
    if(strequal(s,"%hu")){
        lua_pushunsigned(L, sizeof(unsigned short int));
        return 1; 
    }else
    if(strequal(s,"%f")){
        lua_pushunsigned(L, sizeof(float));
        return 1; 
    }else
    if(strequal(s,"%lf")){
        lua_pushunsigned(L, sizeof(double));
        return 1; 
    }else
    if(strequal(s,"%Lf")){
        lua_pushunsigned(L, sizeof(long double));
        return 1; 
    }else
    if(strequal(s,"%p")){
        lua_pushunsigned(L, sizeof(void *));
        return 1; 
    }else
    if(strequal(s,"%c")){
        lua_pushunsigned(L, sizeof(char));
        return 1; 
    }else
    if(strequal(s,"%be")){
        int i = 1;
        if (*((char *)&i) == 1) 
            lua_pushboolean(L,0);
        else lua_pushboolean(L,1);
        return 1; 
    }else
    if(strequal(s,"%le")){
        int i = 1;
        if (*((char *)&i) == 1) 
            lua_pushboolean(L,1);
        else lua_pushboolean(L,0);
        return 1; 
    }
    
    /* fallback*/
    lua_pushboolean(L, FALSE);
    lua_pushliteral(L, "Formato de tipo desconhecido.");
    return 2;

  } 
  
  /* tamanho de arquivo:*/
  else {
    
    FILE *fp=fopen(s,"rb");
    if(fp==NULL) {
        lua_pushboolean(L,FALSE);
        lua_pushliteral(L,"Arquivo nao encontrado.");
        return 2;
    }
    long long file_size = -1; /* Use 'long long' para garantir 64 bits*/

#if defined(_os_win)
    /* --- LÓGICA DO WINDOWS --- */
    /* Obtenha o file descriptor (Windows usa _)*/
    int fd = _fileno(fp); 
    /* Use a estrutura e função de 64 bits*/
    struct _stat64 file_stats;
    if (_fstat64(fd, &file_stats) == 0) { /* 0 é sucesso*/
        file_size = file_stats.st_size;
    }
    
#else
    /* --- LÓGICA POSIX (Linux, macOS, BSD, etc.) --- */
    
    /* Obtenha o file descriptor (POSIX)*/
    int fd = fileno(fp); 
    
    /* Use a estrutura e função padrão*/
    struct stat file_stats;
    if (fstat(fd, &file_stats) == 0) { /* 0 é sucesso*/
        file_size = file_stats.st_size;
    }
#endif

    fclose(fp); /* Feche o arquivo*/

    if (file_size == -1) {
        lua_pushboolean(L, FALSE);
        lua_pushliteral(L, "Erro ao ler o tamanho do arquivo (fstat).");
        return 2;
    }

    /* lua_pushunsigned é bom, mas se seu Lua for antigo,*/
    /* lua_pushnumber(L, (lua_Number)file_size) é mais seguro*/
    /* para números de 64 bits.*/
    lua_pushnumber(L, (lua_Number)file_size);
    return 1;
  }
}


/* FIM ============================================== Adalberto modification ***/

/*============== FILENAME PROCESSOR ==============*/
#ifdef _os_win
 #define _BARRA_ '\\'
#else
 #define _BARRA_ '/'
#endif

#include <string.h>
#include <stddef.h>
/*estou deixando esta função mesmo sem usá-la, pois, primeiro: deu um baita trabalho mental fazê-la*/
/*segundo: pode ser útil futuramente em outras situações!*/
/*o __attribute__((unused)) é para evitar os avisos do compilador de função declarada não usada.*/
static __attribute__((unused)) void os_str_copie(char *dest, size_t dest_size, const char *src, size_t src_len, int inicio, int final) {
    if (!dest || !src || dest_size == 0) return;

    /* Corrige índices: base 1 → base 0*/
    if (inicio < 1) inicio = 1;
    if (final < 0) final = (int)src_len + final + 1; /* -1 → último caractere*/
    if (final > (int)src_len) final = (int)src_len;

    size_t start = (size_t)(inicio - 1);
    size_t end = (size_t)(final);
    if (start >= end || start >= src_len) {
        dest[0] = '\0';
        return;
    }

    size_t len = end - start;
    if (len >= dest_size) len = dest_size - 1;

    memcpy(dest, src + start, len);
    dest[len] = '\0';
}



/*===========  ============*/

static int os_filename(lua_State*L){
  size_t size = 0;
  const char *file = (const char*) luaL_checklstring(L,1,&size);
  if(size==0){ /*string vazia?*/
      return 0;
  }
  int i=0,cont=-1,ponto=-1;
  for(i=size-1;i>=0;i--){
	 if(file[i]=='.' && ponto== -1) ponto=i; /*só pega o primeiro ponto (de tras para frente)*/
     if(file[i]==_BARRA_){
	   cont = i;
	   break;
     }
  }
  /*============ PATH*/
  if(cont!= -1){
    lua_pushlstring(L, file, cont+1);
  }else lua_pushnil(L);
  
  if(ponto== (size - 1)) ponto = -1;/*último caractere não há extensão e o ponto é acrescentado ao nome*/
  if(ponto > 0){
	  if(file[ponto-1] == _BARRA_) ponto = -1;/*caso antes do ponto é barra de endereco entao não há extensao.*/
  }
  /*=========== NAME ==========*/
  if(ponto > 0){/*pegando o nome e ext*/
	 lua_pushlstring(L,(file+cont+1),(ponto-cont-1));
	 /*============ EXT ======= */
	 lua_pushlstring(L, file+ponto+1, (size-ponto-1));  
  }else if(size>(cont+1)){/*============ HÁ NOME, MAS NAO HÁ EXTENSÃO:*/
	 lua_pushlstring(L, file+cont+1, (size-cont-1));
	 lua_pushnil(L);/*e o ultimo é falso; */
  }else{/*não há nome nem extensão*/
	  lua_pushnil(L);/*e o ultimo é falso; */
	  lua_pushnil(L);/*e o ultimo é falso; */
  }
  /*printf("cont: %d\nsize: %d\n", cont, size);*/
	return 3; /*path, name, extension ex.:  C:\\Prisma\\1.0\bin\, prisma, exe*/
}


#ifdef _os_win

static int os_realpath(lua_State*L){
    DWORD ret = 0;
    const char *path = luaL_checkstring(L, 1);
    char** lppPart = {NULL};

    /* 1. Tente com o buffer da stack (rápido)*/
    char stack_buffer[4096];
    ret = GetFullPathNameA(path, 4096, stack_buffer, lppPart);

    /* 2. Erro total?*/
    if (ret == 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 3. Sucesso na primeira tentativa? (Caso mais comum)*/
    if (ret <= 4096) {
        lua_pushstring(L, stack_buffer);
        return 1;
    }

    /* 4. Buffer da stack foi pequeno. 'ret' agora é o tamanho que precisamos.*/
    /* Agora sim, alocamos dinamicamente (heap).*/
    char *heap_buffer = malloc(ret); /* Aloca o tamanho exato*/
    if (heap_buffer == NULL) {
        lua_pushboolean(L, 0); /* Falha de memória*/
        return 1;
    }

    /* 5. Chame DE NOVO com o buffer do heap*/
    DWORD ret2 = GetFullPathNameA(path, ret, heap_buffer, lppPart);

    if (ret2 == 0 || ret2 > ret) {
        /* Falhou (não deveria acontecer, mas é bom checar)*/
        lua_pushboolean(L, 0);
    } else {
        /* Sucesso!*/
        lua_pushstring(L, heap_buffer);
    }

    /* 6. LIBERE O HEAP! (A correção do memory leak)*/
    free(heap_buffer);
    return 1;
}

#else

static int os_realpath(lua_State*L){
  const char *symlinkpath = luaL_checkstring(L,1);
  char *actualpath;
  actualpath = realpath(symlinkpath, NULL);
  if (actualpath != NULL)
  {
    lua_pushstring(L,actualpath);
    free(actualpath);
  }
  else
  {
    lua_pushboolean(L,0);
  }
 return 1;
}
#endif


/** BETO MODIFICATION */

#include <stdio.h>
#include <stdlib.h> /* Para size_t */

/*
 * Copia um arquivo de 'origem_path' para 'destino_path'.
 * Usa I/O padrao C89, portavel e em modo binario.
 *
 * Retorna:
 * 0 em sucesso.
 * -1 em falha (nao pode abrir/ler/escrever).
 */
static int copiar_arquivo(const char *origem_path, const char *destino_path) {
    FILE *origem, *destino;
    
    /* 1. O buffer. BUFSIZ e definido em <stdio.h> 
     * e e o tamanho de buffer sugerido pelo sistema.
     */
    char buffer[BUFSIZ];
    size_t bytes_lidos;

    /* 2. Abrir origem em MODO BINARIO */
    origem = fopen(origem_path, "rb");
    if (origem == NULL) {
        return -1; /* Nao foi possivel abrir a origem */
    }

    /* 3. Abrir destino em MODO BINARIO */
    destino = fopen(destino_path, "wb");
    if (destino == NULL) {
        fclose(origem);
        return -1; /* Nao foi possivel criar o destino */
    }

    /* 4. O loop de copia (leitura e escrita em "chunks") */
    
    while ((bytes_lidos = fread(buffer, 1, BUFSIZ, origem)) > 0) {
        /* * Escreve o numero exato de bytes que acabamos de ler.
         * Se fwrite nao retornar 'bytes_lidos', e um erro (ex: disco cheio).
         */
        if (fwrite(buffer, 1, bytes_lidos, destino) != bytes_lidos) {
            fclose(origem);
            fclose(destino);
            return -1; /* Erro de escrita */
        }
    }

    /* 5. Checagem final de erro de LEITURA */
    /* fread retorna 0 em 'fim de arquivo' OU 'erro'. 
     * ferror() nos diz se foi um erro. */
    if (ferror(origem)) {
        fclose(origem);
        fclose(destino);
        return -1; /* Erro de leitura */
    }

    /* 6. Sucesso! Limpe tudo. */
    fclose(origem);
    fclose(destino);
    return 0; /* Sucesso */
}
/*
 *local ok, err = es.copie_arquivo(arquivo_origem, arquivo_destino)
 * Retorna (verdadeiro) em sucesso
 * Retorna (nil, "mensagem de erro") em falha
 */
static int os_copie_arquivo(lua_State *L) {
    const char *origem = luaL_checkstring(L, 1);
    const char *destino = luaL_checkstring(L, 2);
    
    int resultado = copiar_arquivo(origem, destino);
    
    if (resultado == 0) {
        /* Sucesso */
        lua_pushboolean(L, 1);
        return 1;
    }
    else {
        /* Falha */
        lua_pushnil(L);
        lua_pushstring(L, "Erro de E/S: Nao foi possivel copiar o arquivo");
        return 2; /* Retorna (nil, msg) */
    }
}

/** FIM BETO MODIFICATION */

static const luaL_Reg syslib[] = {

  {"durma" , os_sleep } ,
  {"relogio",     os_clock},
  {"data",      os_date},
  {"diftempo",  os_difftime},
  {"execute",   os_execute},
  {"saia",      os_exit},
  {"obtvarambiente",    os_getenv}, /*obtem variavel de ambiente*/
  {"defvarambiente",    os_setenv}, /* define variavel de ambiente. //cuidado sobreescrever uma ja existente*/
  /*/by adalberto 2016 jun.*/
  #if !defined(_os_win)
  {"delvarambiente",      os_unsetenv},/*/deleta uma variavel de ambiente.*/
  #endif
  {"caminho_absoluto", os_realpath}, /*pega o path real de um arquivo ou link simbolico*/
  {"remova",    os_remove}, /*apaga um arquivo*/
  {"renomeie",    os_rename}, /*renomeia um arquivo*/
  {"deflocal", os_setlocale}, /*define idioma, codificao etc */
  {"tempo",      os_time},
  {"nometmp",   os_tmpname}, /*nome temporario*/
  /* sis.tamanho('out.txt'); --> tamanho do arquivo out.txt em bytes; */
  /* sis.tamanho('%i'); --> tamanho de um integer em bytes; */
  {"tamanho", os_size},/*retorna o tamanho de um arquivo | o tamanho de um dado em C de acordo com o sistema em que foi compilado Prisma. | verdaeiro ou falso de acordo com o tipo de endianess "%be" big-endian ou "%le" little-endian */
  {"nome_arquivo", os_filename}, /*retorna três valores: pasta, nome_arquivo, extensao = sis.nome_arquivo'pasta/file.ext';*/
  {"copie_arquivo",     os_copie_arquivo},/*sis.copie_arquivo(orig, dest);*/
  {NULL, NULL}
};

/* }====================================================== */



LUAMOD_API int luaopen_os (lua_State *L) {
  luaL_newlib(L, syslib);

  lua_pushstring(L, OS_NAME ); /* OS_NAME in <osname/osname.h>*/
  lua_setfield(L, -2, "nome");/*/linux, win32,win64, outro*/
  
  lua_pushstring(L, ARCH_NAME);
  lua_setfield(L, -2, "arq");

  return 1;
}
