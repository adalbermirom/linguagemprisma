/*
** $Id: lua.c,v 1.206.1.1 2013/04/12 18:48:47 roberto Exp $
** Lua stand-alone interpreter
** See Copyright Notice in lua.h
*/


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _NO_READLINE

#define lua_c

#include "prisma.h" /*algumas definições específicas de Prisma.*/

#include "lauxlib.h"
#include "lualib.h"
#include "osname/osname.h"


static const char *nome_prog = NOME_PROG;
static lua_State *globalL = NULL;
int getprog(char *progdir, size_t nsize);/* final do arquivo*/


static void lstop (lua_State *L, lua_Debug *ar) {
  (void)ar;  /* unused arg. */
  lua_sethook(L, NULL, 0, 0);
  /*t_setforeground(tRed);*/
  luaL_error(L, "interrompido!");
  /*t_setdefault();*/
}


static void laction (int i) {
  signal(i, SIG_DFL); /* if another SIGINT happens before lstop,
                              terminate process (default action) */
  lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}



/*============== PRISMADIR ==============*/
#if defined(_os_win32) || defined(_os_win64) /*#include "osname/osname.h"*/
 #define _BARRA_ '\\'
 #include <string.h>
 #include <windows.h>
 /*#define PATH_MAX MAX_PATH*/
#else
 #define _BARRA_ '/'
 #include <string.h>
 #include <limits.h>
 #include <unistd.h>
#endif

/* define a global prismadir = 'path/completa/do/interpretador/prisma'*/
void prisma_def_prismadir(lua_State*L, char *file){
  unsigned int size = strlen(file);
  int i=0,cont=0;
  for(i=size-1;i>=0;i--){
     if(file[i]==_BARRA_){
	   cont = i;
	   break;
     }
  }
  /*printf("cont: %u\n", cont); //for debug*/
  if(cont>0){
    /*char path[cont + 2];//cont + 2 para garantir o tamanho*/
    lua_pushlstring(L, file,cont+1);
    lua_setglobal(L,"prismadir");
  }  
}
/*===========  FIM PRISMADIR ============*/



void prisma_inicialize(lua_State*L){  
  lua_gc(L, LUA_GCSTOP, 0);  /*Para o coletor de lixo durante a inicialização*/
  lua_pushliteral(L, SHOW_VERSION); /*criando a variável global.*/
  lua_setglobal(L,"_VERSAO");
  
  lua_pushliteral(L, SHOW_INFO);/*INFORMAÇÃO do interpretador*/
  lua_setglobal(L,"_INFO");
  luaL_openlibs(L);  /* abre as bibliotecas padrão */
  lua_gc(L, LUA_GCRESTART, 0); /*reinicia o coletro de lixo*/
}

void prisma_finalize(lua_State*L){
	lua_close(L);
}



void prisma_erro_msg(lua_State*L, const char*msg){	
	if (nome_prog) luai_writestringerror("%s: ", nome_prog);
    luai_writestringerror("%s\n", msg);	
}

static int report (lua_State *L, int status) {
  if (status != LUA_OK && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(erro objeto nao e´ uma string)";
    prisma_erro_msg(L, msg);
    lua_pop(L, 1);
    /*force a complete garbage collection in case of errors*/
    lua_gc(L, LUA_GCCOLLECT, 0);
  }
  return status;
}

static int report_principal (lua_State *L, int status) {
  if (status != LUA_OK && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(erro objeto nao e´ uma string)";
    /*prisma_erro_msg(L, msg);*/
    if (nome_prog) luai_writestringerror("%s: ", nome_prog);
    luai_writestringerror("\n%s `principal(args)'\n", msg);
    lua_pop(L, 1);
    /*force a complete garbage collection in case of errors*/
    lua_gc(L, LUA_GCCOLLECT, 0);
  }
  return status;
}



void prisma_verif_status(lua_State*L, int status){
	if (status != LUA_OK) {
    const char *msg = (lua_type(L, -1) == LUA_TSTRING) ? lua_tostring(L, -1) : NULL;
    if (msg == NULL) msg = "(Erro objeto nao e´ uma string)";
    prisma_erro_msg(L, msg);
    lua_pop(L, 1);
  }	
}

#define use_msg "\n---------------------------\n\n"\
                SHOW_INFO "\n"\
                "\nFaltam argumentos, use:\n\n"\
                "   prisma prog.prisma (executa prog.prisma)\n\n"\
                "   prisma -v (mostra a versão)\n"\
			    "   prisma -e 'imprima\"ola mundo em prisma!\"' (Executa uma string)"\
			    "\n---------------------------\n"\
                



static int traceback (lua_State *L) {

  const char *msg = lua_tostring(L, 1);
  if (msg)
    luaL_traceback(L, L, msg, 1);
  else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
    if (!luaL_callmeta(L, 1, "__convstring"))  /* try its 'tostring' metamethod */
      lua_pushliteral(L, "(nenhuma mensagem de erro)");
  }

  return 1;
}


static int docall (lua_State *L, int narg, int nres) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, traceback);  /* push traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
  globalL = L;  /* to be available to 'laction' */
  signal(SIGINT, laction);
  status = lua_pcall(L, narg, nres, base);
  signal(SIGINT, SIG_DFL);
  lua_remove(L, base);  /* remove traceback function */
  return status;
}


int prisma_execute(lua_State*L,char*arq){
  int status = luaL_loadfile(L, arq);
  if (status == LUA_OK) status = docall(L, 0, 0);
  return report(L, status);
}



static int dostring (lua_State *L, const char *s, const char *name) {
  int status = luaL_loadbuffer(L, s, strlen(s), name);
  if (status == LUA_OK) status = docall(L, 0, 0);
  return report(L, status);
}



int prisma_verif_args(lua_State*L,int argc, char **argv){
   /*tab args.*/
   if(argc<=1) return 0;
   
   if(strcmp( argv[1], "-v")==0){
	   puts(SHOW_VERSION);
	   prisma_finalize(L);	
	   exit(1);   	   
   }else /*prisma -e  'imprima("ola mundo em prisma!")'*/
   if(strcmp(argv[1], "-e")==0){
	   if(argc<=2){ /* -e but not arg string to execute, error*/
		   puts("\nErro - Prisma em linha de comando: opção -e requer uma string depois para ser executada\n");
		   puts("Use:");
		   puts(use_msg);
		   exit(1);		   
	   }
	   const char *chunk = argv[2];
       /*pris_dostring(L, argv[2]);//executa a string passada.*/
       lua_assert(chunk != NULL);
        if (dostring(L, chunk, "=(linha de comando -e:)") != LUA_OK)
            exit(1);
        else
            exit(0);
   }
   int i = 0;
   lua_createtable(L, 0,argc+1);
   for ( i = 0 ; i< argc ; i++ ){
		    lua_pushstring( L , argv[i] ); /*push values into table...*/
	        lua_rawseti(L, -2, i - 1 );
    } 
   lua_setglobal(L,"args");
   
   return 1;
}


int main (int argc, char **argv) {

  long int status, result;
  lua_State *L = luaL_newstate();  /* create state */
  if (L == NULL) {
    prisma_erro_msg(L, "Nao foi possivel carregar o programa: memoria insuficiente");
    return EXIT_FAILURE;
  }
  prisma_inicialize(L);
  
  if(!prisma_verif_args(L,argc,argv)){
	  puts(use_msg);
	  prisma_finalize(L);
	  exit(1);	  
  }
  
 
  
/* ---- prismadir -----------*/
  char filename[PATH_MAX];
  if(getprog(filename, PATH_MAX) )
    prisma_def_prismadir(L,filename);
  else
    prisma_def_prismadir(L, argv[0]);

   
  /*inicializando*/
  status = prisma_execute(L,argv[1]);
  result = lua_tointeger(L,-1);
  report(L, status);

  /* --use estrito.modo_global*/

  /* ----  carrega a função principal(args) em prisma se estiver definida*/
  lua_getglobal(L,"principal");
  if( lua_type(L,-1) == LUA_TFUNCTION ){
	  lua_getglobal(L,"args");
	 /* status = lua_pcall(L,1,1,0);*/
	  status = docall(L, 1, 0);
	  report_principal(L,status);
	  result = lua_tointeger(L, -1);	  
  }/*trata do retorno da função principal se ela existir.*/
  else report(L, status);
  prisma_finalize(L);
  return result; /*retorna o que retornou a função principal(args).*/
}


/* ---- esta função foi retirada de srlua, muito mais completa do que a minha implementação anterior*/
/* --- em domínio público, pode ser encontrada em https://github.com/LuaDist/srlua/blob/master/srlua.c */

int getprog(char *progdir, size_t nsize) {
  char *lb;
  int n = 0;
#if defined(__CYGWIN__)
  char win_buff[_PATH_MAX + 1];
  GetModuleFileNameA(NULL, win_buff, nsize);
  cygwin_conv_path(CCP_WIN_A_TO_POSIX, win_buff, progdir, nsize);
  n = strlen(progdir);
#elif defined(_WIN32)
  n = GetModuleFileNameA(NULL, progdir, nsize);
#elif defined(__linux__)
  n = readlink("/proc/self/exe", progdir, nsize);
  if (n > 0) progdir[n] = 0;
#elif defined(__sun)
  pid_t pid = getpid();
  char linkname[256];
  sprintf(linkname, "/proc/%d/path/a.out", pid);
  n = readlink(linkname, progdir, nsize);
  if (n > 0) progdir[n] = 0;  
#elif defined(__FreeBSD__)
  int mib[4];
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PATHNAME;
  mib[3] = -1;
  size_t cb = nsize;
  sysctl(mib, 4, progdir, &cb, NULL, 0);
  n = cb;
#elif defined(__BSD__)
  n = readlink("/proc/curproc/file", progdir, nsize);
  if (n > 0) progdir[n] = 0;
#elif defined(__APPLE__)
  uint32_t nsize_apple = nsize;
  if (_NSGetExecutablePath(progdir, &nsize_apple) == 0)
    n = strlen(progdir);
#else
  /* FALLBACK*/
  /* Use 'lsof' ... should work on most UNIX systems (incl. OSX)*/
  /* lsof will list open files, this captures the 1st file listed (usually the executable)*/
  int pid;
  FILE* fd;
  char cmd[80];
  pid = getpid();

  sprintf(cmd, "lsof -p %d | awk '{if ($5==\"REG\") { print $9 ; exit}}' 2> /dev/null", pid);
  fd = popen(cmd, "r");
  n = fread(progdir, 1, nsize, fd);
  pclose(fd);

  /* remove newline*/
  if (n > 1) progdir[--n] = '\0';
#endif
  if (n == 0 || n == nsize || (lb = strrchr(progdir, (int)LUA_DIRSEP[0])) == NULL)
    return 0; /*false not ok*/
  return 1; /*true ok*/
}

