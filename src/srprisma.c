/*
* srprisma by Adalberto A F dez/2023
* a modification from:
* srlua.c
* Lua interpreter for self-running programs
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 27 Apr 2012 09:24:34
* This code is hereby placed in the public domain.
*/


#if defined(_WIN32)
  #include <windows.h>
  #define _PATH_MAX MAX_PATH
#else
  #define _PATH_MAX PATH_MAX
#endif

#if defined (__CYGWIN__)
  #include <sys/cygwin.h>
#endif

#if defined(__linux__) || defined(__sun)
  #include <unistd.h> /* readlink */
#endif

#if defined(__APPLE__)
  #include <sys/param.h>
  #include <mach-o/dyld.h>
#endif

#if defined(__FreeBSD__)
  #include <sys/types.h>
  #include <sys/sysctl.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*macros e estrutura com informações sobre o programa prisma embutido no executável*/
#define GLUESIG		"%%PRISMA"
#define GLUELEN		(sizeof(GLUESIG)-1)
#define GLUETYP		(sizeof(GLUESIG)-2)
typedef struct { char sig[GLUELEN]; long long size1, size2; } Glue;

#include "prisma.h"
#include "prismaconf.h"
#include "lualib.h"
#include "lauxlib.h"

int getprog(char *progdir, size_t nsize) ;/*obtem o caminho completo do interpretador em diversas plataformas*/
void prisma_def_prismadir(lua_State*L, char *file);


typedef struct
{
 FILE *f;
 size_t size;
 char buff[512];
} State;


static const char *myget(lua_State *L, void *data, size_t *size)
{
 State* s=data;
 size_t n;
 (void)L;
 n=(sizeof(s->buff)<=s->size)? sizeof(s->buff) : s->size;
 n=fread(s->buff,1,n,s->f);
 s->size-=n;
 *size=n;
 return (n>0) ? s->buff : NULL;
}

#define cannot(x) luaL_error(L,"não foi possível %s %s: %s",x,name,strerror(errno))

/*lê 8 bytes e transforma em long int - endianess independente, pois usa sempre LE*/
long long bytes_to_llu(const char*s){
    long long n=0;
    int i=0;
	for( i = 0; i < 8; i++ )
    n |= ( (unsigned char)s[ i ] << ( i << 3 ) );
    return n;
}

static void load(lua_State *L, const char *name)
{
 Glue t;
 State S;
 int c;
 char ui_bytes[8];
 char name2[PATH_MAX];
 FILE *f=NULL;
 getprog( name2, PATH_MAX);
 f=fopen( name2,"rb");
 if (f==NULL) cannot("abrir");
 if (fseek(f, -24, SEEK_END)!=0) cannot("seek"); /*pegando a posição -24 do final do arquivo. */
 fread(t.sig, 8,1,f); /* copiando só o sig */
  /* (t.sig == "%%PRISMA")?*/
 if (memcmp(t.sig,GLUESIG,GLUELEN)!=0) luaL_error(L,"nenhum programa prisma encontrado em %s", name);
  fread(ui_bytes,8,1,f);/* array de bytes para converter em long long int. (8bytes)*/
  t.size1 = bytes_to_llu(ui_bytes);
  fread(ui_bytes,8,1,f);/*copiando o sengundo size t.size2*/
  t.size2 = bytes_to_llu(ui_bytes);
 if (fseek(f,t.size1,SEEK_SET)!=0) cannot("-----> seek");
 S.f=f; S.size=t.size2;
 c=getc(f);
 if (c=='#')
  while (--S.size>0 && c!='\n') c=getc(f);
 else
  ungetc(c,f);
 if (lua_load(L,myget,&S,"=",NULL)!=0) lua_error(L);
 fclose(f);
}

static int pmain(lua_State *L)
{
 int argc=lua_tointeger(L,1);
 char** argv=lua_touserdata(L,2);
 int i;
 lua_gc(L,LUA_GCSTOP,0);
 lua_pushliteral(L, SHOW_VERSION); /*criando a variável global.*/
 lua_setglobal(L,"_VERSAO");  
 lua_pushliteral(L, SHOW_INFO);/*INFORMAÇÃO do interpretador*/
 lua_setglobal(L,"_INFO");
 luaL_openlibs(L);
 lua_gc(L,LUA_GCRESTART,0);
 load(L,argv[0]);
 lua_createtable(L,argc,0);
 for (i=0; i<argc; i++)
 {
  lua_pushstring(L,argv[i]);
  lua_rawseti(L,-2,i);
 }
 lua_setglobal(L,"args");
 /*luaL_checkstack(L,argc-1,"muitos argumentos para o programa Prisma");
 for (i=1; i<argc; i++)
 {
  lua_pushstring(L,argv[i]);
 }
 lua_call(L,argc-1,0); //não preciso desses argumentos
 */
 lua_getglobal(L,"args");/* script_run (args);*/
 lua_call(L,1,0);/*para acessar os argumentos use global args no script*/
 
 return 0;
}

static void fatal(const char* progname, const char* message)
{
#ifdef _WIN32
 MessageBox(NULL,message,progname,MB_ICONERROR | MB_OK);
#else
 fprintf(stderr,"%s: %s\n",progname,message);
#endif
 exit(EXIT_FAILURE);
}



/*============== PRISMADIR ==============*/
#ifdef _WIN32
 #define _BARRA_ '\\'
 #include <string.h>
 #include <windows.h>
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

void win_ex_string (lua_State *L , const char *S );
const char *win_CAR;
const char *win_CAR2;
const char *win_CAR3;

void win_def_conv_c_w(lua_State*L){	
	win_ex_string ( L , win_CAR );
	win_ex_string ( L , win_CAR2 );
	win_ex_string ( L , win_CAR3 );
}

int main(int argc, char *argv[])
{
 int status=0, result=0;
 lua_State *L;
 
 /*PRISMADIR:*/
  char filename[PATH_MAX];
  int condition = getprog(filename, PATH_MAX);
 /*END PRISMADIR:*/
 if (!condition) fatal("srprisma","nao foi possivel localizar este executavel");
 L=luaL_newstate();
 if (L==NULL) fatal(argv[0],"memoria insuficiente");

  /*PRISMADIR:*/
  if(condition){
      prisma_def_prismadir(L,filename);
      argv[0]=filename;
      /*puts("\ndentro\n");//for debug*/
  }else
      prisma_def_prismadir(L, argv[0]);
  /*END PRISMADIR:*/
 win_def_conv_c_w(L);
 lua_pushcfunction(L,&pmain);
 lua_pushinteger(L,argc);
 lua_pushlightuserdata(L,argv);
 if (lua_pcall(L,2,0,0)!=0){
	 /* lua_tostring(L,-1); */
	 lua_getglobal(L,"conv_w");  
	 lua_pushvalue(L, -2); /*coloca a string de erro na pilha*/
	 if( lua_type(L,-2) == LUA_TFUNCTION ){
		 int status = lua_pcall(L,1,1,0);
		 if(status) fatal("conv_c", "Erro ao chamar conv_c() direto de srprisma");
		 else fatal(argv[0], lua_tostring(L,-1)); /*string erro  no topo*/
     }else /*conv_w não encontrado*/
         fatal(argv[0], lua_tostring(L,-1));
}
 
 lua_getglobal(L,"principal");     
 lua_getglobal(L,"args");
  if( lua_type(L,-2) == LUA_TFUNCTION ){
	  status = lua_pcall(L,1,1,0);
	  if(status){ /* ocorreu um erro?*/
		  /*fatal("Função principal()", lua_tostring(L,-1));*/
		  /* lua_tostring(L,-1); */
	     lua_getglobal(L,"conv_w");  
	     lua_pushvalue(L, -2); /*coloca a string de erro na pilha*/
	     if( lua_type(L,-2) == LUA_TFUNCTION ){
		     int status = lua_pcall(L,1,1,0);
		     if(status) fatal("conv_w", "Erro ao chamar conv_c() direto de srprisma");
		     else fatal("em principal()", lua_tostring(L,-1)); /*string erro  no topo*/
         }else /*conv_w não encontrado*/
             fatal("em principal()", lua_tostring(L,-1)); 
	 }else
	     result = lua_tonumber(L, -1);
  } 
  lua_close(L);
  return (status == LUA_OK) ? result : EXIT_FAILURE;
}



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



/*execute string for win*/
void win_ex_string (lua_State *L , const char *S ){

  int error = luaL_loadbuffer(L, S , strlen(S), S ) || lua_pcall(L, 0, LUA_MULTRET, 0);
   if (error) {

    const char*string_error = lua_tostring(L, -1);
    lua_pushstring(L , string_error);
    lua_setglobal(L, "Win_err");
      fprintf(stderr, "%s", string_error );
      lua_pop(L, 1);  /* pop error message from the stack */
   }

}


const char *win_CAR = 
"\
\
funcao conv_c( str )\
\
str = string.troque(str , \"Ç\" , car(128) )\
\
str = string.troque(str , \"ü\" , car(129) )\
\
str = string.troque(str , \"é\" , car(130) )\
\
str = string.troque(str , \"â\" , car(131) )\
\
str = string.troque(str , \"ä\" , car(132) )\
\
str = string.troque(str , \"à\" , car(133) )\
\
str = string.troque(str , \"å\" , car(134) )\
\
str = string.troque(str , \"ç\" , car(135) )\
\
str = string.troque(str , \"ê\" , car(136) )\
\
str = string.troque(str , \"ë\" , car(137) )\
\
str = string.troque(str , \"è\" , car(138) )\
\
str = string.troque(str , \"ï\" , car(139) )\
\
str = string.troque(str , \"î\" , car(140) )\
\
str = string.troque(str , \"ì\" , car(141) )\
\
str = string.troque(str , \"Ä\" , car(142) )\
\
str = string.troque(str , \"Å\" , car(143) )\
\
str = string.troque(str , \"É\" , car(144) )\
\
str = string.troque(str , \"æ\" , car(145) )\
\
str = string.troque(str , \"Æ\" , car(146) )\
\
str = string.troque(str , \"ô\" , car(147) )\
\
str = string.troque(str , \"ö\" , car(148) )\
\
str = string.troque(str , \"ò\" , car(149) )\
\
str = string.troque(str , \"û\" , car(150) )\
\
str = string.troque(str , \"ù\" , car(151) )\
\
str = string.troque(str , \"ÿ\" , car(152) )\
\
str = string.troque(str , \"Ö\" , car(153) )\
\
str = string.troque(str , \"Ü\" , car(154) )\
\
str = string.troque(str , \"ø\" , car(155) )\
\
str = string.troque(str , \"£\" , car(156) )\
\
str = string.troque(str , \"Ø\" , car(157) )\
\
str = string.troque(str , \"×\" , car(158) )\
\
str = string.troque(str , \"ƒ\" , car(159) )\
\
str = string.troque(str , \"á\" , car(160) )\
\
str = string.troque(str , \"í\" , car(161) )\
\
str = string.troque(str , \"ó\" , car(162) )\
\
str = string.troque(str , \"ú\" , car(163) )\
\
str = string.troque(str , \"ñ\" , car(164) )\
str = string.troque(str , \"Ñ\" , car(165) )\
str = string.troque(str , \"ª\" , car(166) )\
str = string.troque(str , \"º\" , car(167) )\
str = string.troque(str , \"¿\" , car(168) )\
str = string.troque(str , \"Á\" , car(181) )\
str = string.troque(str , \"Â\" , car(182) )\
str = string.troque(str , \"À\" , car(183) )\
str = string.troque(str , \"¢\" , car(189) )\
str = string.troque(str , \"¥\" , car(190) )\
str = string.troque(str , \"┐\" , car(191) )\
str = string.troque(str , \"└\" , car(192) )\
str = string.troque(str , \"┴\" , car(193) )\
str = string.troque(str , \"┬\" , car(194) )\
str = string.troque(str , \"├\" , car(195) )\
str = string.troque(str , \"─\" , car(196) )\
str = string.troque(str , \"┼\" , car(197) )\
str = string.troque(str , \"ã\" , car(198) )\
str = string.troque(str , \"Ã\" , car(199) )\
\
str = string.troque(str , \"ð\" , car(208) )\
str = string.troque(str , \"Ð\" , car(209) )\
str = string.troque(str , \"Ê\" , car(210) )\
str = string.troque(str , \"Ë\" , car(211) )\
str = string.troque(str , \"È\" , car(212) )\
str = string.troque(str , \"ı\" , car(213) )\
str = string.troque(str , \"Í\" , car(214) )\
str = string.troque(str , \"Î\" , car(215) )\
str = string.troque(str , \"Ï\" , car(216) )\
str = string.troque(str , \"Ì\" , car(222) )\
str = string.troque(str , \"▀\" , car(223) )\
str = string.troque(str , \"Ó\" , car(224) )\
str = string.troque(str , \"ß\" , car(225) )\
str = string.troque(str , \"Ô\" , car(226) )\
str = string.troque(str , \"Ò\" , car(227) )\
str = string.troque(str , \"õ\" , car(228) )\
str = string.troque(str , \"Õ\" , car(229) )\
str = string.troque(str , \"µ\" , car(230) )\
str = string.troque(str , \"þ\" , car(231) )\
str = string.troque(str , \"Þ\" , car(232) )\
str = string.troque(str , \"Ú\" , car(233) )\
str = string.troque(str , \"Û\" , car(234) )\
str = string.troque(str , \"Ù\" , car(235) )\
str = string.troque(str , \"ý\" , car(236) )\
str = string.troque(str , \"Ý\" , car(237) )\
retorne str;\
 \
fim \
funcao conv_w(str)\
\
str = string.troque(str , \"À\" , car(192) )\
str = string.troque(str , \"Á\" , car(193) ) \
str = string.troque(str , \"Â\" , car(194) ) \
str = string.troque(str , \"Ã\" , car(195) ) \
str = string.troque(str , \"Ä\" , car(196) ) \
str = string.troque(str , \"Å\" , car(197) ) \
str = string.troque(str , \"Æ\" , car(198) ) \
str = string.troque(str , \"Ç\" , car(199) ) \
str = string.troque(str , \"È\" , car(200) ) \
str = string.troque(str , \"É\" , car(201) ) \
str = string.troque(str , \"Ê\" , car(202) ) \
str = string.troque(str , \"Ë\" , car(203) ) \
str = string.troque(str , \"Ì\" , car(204) ) \
str = string.troque(str , \"Í\" , car(205) ) \
str = string.troque(str , \"Î\" , car(206) ) \
str = string.troque(str , \"Ï\" , car(207) ) \
str = string.troque(str , \"Ð\" , car(208) ) \
str = string.troque(str , \"Ñ\" , car(209) ) \
str = string.troque(str , \"Ò\" , car(210) ) \
str = string.troque(str , \"Ó\" , car(211) ) \
str = string.troque(str , \"Ô\" , car(212) ) \
str = string.troque(str , \"Õ\" , car(213) ) \
str = string.troque(str , \"Ö\" , car(214) ) \
str = string.troque(str , \"×\" , car(215) ) \
str = string.troque(str , \"Ø\" , car(216) ) \
str = string.troque(str , \"Ù\" , car(217) ) \
str = string.troque(str , \"Ú\" , car(218) ) \
str = string.troque(str , \"Û\" , car(219) ) \
str = string.troque(str , \"Ü\" , car(220) ) \
str = string.troque(str , \"Ý\" , car(221) ) \
str = string.troque(str , \"Þ\" , car(222) ) \
str = string.troque(str , \"ß\" , car(223) ) \
str = string.troque(str , \"à\" , car(224) ) \
str = string.troque(str , \"á\" , car(225) ) \
str = string.troque(str , \"â\" , car(226) ) \
str = string.troque(str , \"ã\" , car(227) ) \
str = string.troque(str , \"ä\" , car(228) ) \
str = string.troque(str , \"å\" , car(229) ) \
str = string.troque(str , \"æ\" , car(230) ) \
str = string.troque(str , \"ç\" , car(231) ) \
str = string.troque(str , \"è\" , car(232) ) \
str = string.troque(str , \"é\" , car(233) ) \
str = string.troque(str , \"ê\" , car(234) ) \
str = string.troque(str , \"ë\" , car(235) ) \
str = string.troque(str , \"ì\" , car(236) ) \
str = string.troque(str , \"í\" , car(237) ) \
str = string.troque(str , \"î\" , car(238) ) \
str = string.troque(str , \"ï\" , car(239) ) \
str = string.troque(str , \"ð\" , car(240) ) \
str = string.troque(str , \"ñ\" , car(241) ) \
str = string.troque(str , \"ò\" , car(242) ) \
str = string.troque(str , \"ó\" , car(243) ) \
str = string.troque(str , \"ô\" , car(244) ) \
str = string.troque(str , \"õ\" , car(245) ) \
str = string.troque(str , \"ö\" , car(246) ) \
str = string.troque(str , \"÷\" , car(247) ) \
str = string.troque(str , \"ø\" , car(248) ) \
str = string.troque(str , \"ù\" , car(249) ) \
str = string.troque(str , \"ú\" , car(250) ) \
str = string.troque(str , \"û\" , car(251) ) \
str = string.troque(str , \"ü\" , car(252) ) \
str = string.troque(str , \"ý\" , car(253) ) \
str = string.troque(str , \"þ\" , car(254) ) \
str = string.troque(str , \"ÿ\" , car(255) ) \
 retorne str;\
fim";

const char * win_CAR2 = "\
\
funcao desconv_w(str)\
\
str = string.troque(str , car(192) ,\"À\"  )\
str = string.troque(str , car(193) , \"Á\" ) \
str = string.troque(str , car(194), \"Â\"  ) \
str = string.troque(str  , car(195), \"Ã\" ) \
str = string.troque(str , car(196), \"Ä\"  ) \
str = string.troque(str  , car(197), \"Å\" ) \
str = string.troque(str , car(198), \"Æ\"  ) \
str = string.troque(str , car(199), \"Ç\"  ) \
str = string.troque(str  , car(200), \"È\" ) \
str = string.troque(str  , car(201), \"É\" ) \
str = string.troque(str  , car(202), \"Ê\" ) \
str = string.troque(str  , car(203), \"Ë\" ) \
str = string.troque(str  , car(204), \"Ì\" ) \
str = string.troque(str  , car(205), \"Í\" ) \
str = string.troque(str , car(206), \"Î\"  ) \
str = string.troque(str , car(207), \"Ï\"  ) \
str = string.troque(str , car(208), \"Ð\"  ) \
str = string.troque(str  , car(209), \"Ñ\" ) \
str = string.troque(str  , car(210), \"Ò\" ) \
str = string.troque(str , car(211), \"Ó\"  ) \
str = string.troque(str , car(212), \"Ô\"  ) \
str = string.troque(str , car(213), \"Õ\"  ) \
str = string.troque(str  , car(214), \"Ö\" ) \
str = string.troque(str ,  car(215),\"×\"  ) \
str = string.troque(str  , car(216), \"Ø\" ) \
str = string.troque(str  , car(217), \"Ù\" ) \
str = string.troque(str ,  car(218),\"Ú\"  ) \
str = string.troque(str  , car(219), \"Û\" ) \
str = string.troque(str , car(220), \"Ü\"  ) \
str = string.troque(str , car(221), \"Ý\"  ) \
str = string.troque(str  , car(222), \"Þ\" ) \
str = string.troque(str , car(223), \"ß\"  ) \
str = string.troque(str , car(224), \"à\"  ) \
str = string.troque(str , car(225), \"á\"  ) \
str = string.troque(str , car(226), \"â\"  ) \
str = string.troque(str  , car(227), \"ã\" ) \
str = string.troque(str  , car(228), \"ä\" ) \
str = string.troque(str  , car(229), \"å\" ) \
str = string.troque(str , car(230), \"æ\"  ) \
str = string.troque(str , car(231), \"ç\"  ) \
str = string.troque(str , car(232), \"è\"  ) \
str = string.troque(str , car(233) , \"é\" ) \
str = string.troque(str  , car(234), \"ê\" ) \
str = string.troque(str , car(235), \"ë\"  ) \
str = string.troque(str , car(236), \"ì\"  ) \
str = string.troque(str  , car(237), \"í\" ) \
str = string.troque(str  , car(238), \"î\" ) \
str = string.troque(str  , car(239), \"ï\" ) \
str = string.troque(str  , car(240), \"ð\" ) \
str = string.troque(str  , car(241), \"ñ\" ) \
str = string.troque(str , car(242), \"ò\"  ) \
str = string.troque(str  , car(243), \"ó\" ) \
str = string.troque(str  , car(244), \"ô\" ) \
str = string.troque(str  , car(245), \"õ\" ) \
str = string.troque(str  , car(246), \"ö\" ) \
str = string.troque(str , car(247) , \"÷\" ) \
str = string.troque(str  , car(248), \"ø\" ) \
str = string.troque(str , car(249), \"ù\"  ) \
str = string.troque(str  , car(250), \"ú\" ) \
str = string.troque(str  , car(251), \"û\" ) \
str = string.troque(str  , car(252) , \"ü\") \
str = string.troque(str  , car(253), \"ý\" ) \
str = string.troque(str , car(254), \"þ\"  ) \
str = string.troque(str , car(255), \"ÿ\"  ) \
 retorne str;\
fim \
";

const char *win_CAR3 = "\
\
funcao desconv_c( str )\
str = string.troque(str  , car(128), \"Ç\" )\
str = string.troque(str , car(129), \"ü\"  )\
str = string.troque(str , car(130) , \"é\" )\
str = string.troque(str , car(131), \"â\"  )\
str = string.troque(str , car(132) , \"ä\" )\
str = string.troque(str , car(133), \"à\"  )\
str = string.troque(str  , car(134), \"å\" )\
str = string.troque(str  , car(135), \"ç\" )\
str = string.troque(str  , car(136), \"ê\" )\
str = string.troque(str  , car(137), \"ë\" )\
str = string.troque(str  , car(138), \"è\" )\
str = string.troque(str , car(139), \"ï\"  )\
str = string.troque(str  , car(140), \"î\" )\
str = string.troque(str  , car(141), \"ì\" )\
str = string.troque(str , car(142), \"Ä\"  )\
str = string.troque(str , car(143), \"Å\"  )\
str = string.troque(str  , car(144), \"É\" )\
str = string.troque(str , car(145), \"æ\"  )\
str = string.troque(str , car(146), \"Æ\"  )\
str = string.troque(str  , car(147), \"ô\" )\
str = string.troque(str  , car(148) , \"ö\")\
str = string.troque(str  , car(149), \"ò\" )\
str = string.troque(str , car(150), \"û\"  )\
str = string.troque(str , car(151), \"ù\"  )\
str = string.troque(str  , car(152), \"ÿ\" )\
str = string.troque(str  , car(153), \"Ö\" )\
str = string.troque(str  , car(154), \"Ü\" )\
str = string.troque(str , car(155), \"ø\"  )\
str = string.troque(str  , car(156), \"£\" )\
str = string.troque(str  , car(157), \"Ø\" )\
str = string.troque(str , car(158), \"×\"  )\
str = string.troque(str  , car(159), \"ƒ\" )\
str = string.troque(str , car(160), \"á\"  )\
str = string.troque(str  , car(161), \"í\" )\
str = string.troque(str  , car(162), \"ó\" )\
str = string.troque(str  , car(163) , \"ú\")\
str = string.troque(str  , car(164), \"ñ\" )\
str = string.troque(str  , car(165), \"Ñ\" )\
str = string.troque(str  , car(166), \"ª\" )\
str = string.troque(str  , car(167), \"º\" )\
str = string.troque(str , car(168), \"¿\"  )\
str = string.troque(str  , car(181), \"Á\" )\
str = string.troque(str  , car(182), \"Â\" )\
str = string.troque(str  , car(183), \"À\" )\
str = string.troque(str , car(189), \"¢\"  )\
str = string.troque(str  , car(190), \"¥\" )\
str = string.troque(str , car(191), \"┐\"  )\
str = string.troque(str  , car(192), \"└\" )\
str = string.troque(str , car(193), \"┴\"  )\
str = string.troque(str , car(194), \"┬\"  )\
str = string.troque(str  , car(195), \"├\" )\
str = string.troque(str , car(196), \"─\"  )\
str = string.troque(str , car(197), \"┼\"  )\
str = string.troque(str , car(198), \"ã\"  )\
str = string.troque(str  , car(199), \"Ã\" )\
\
str = string.troque(str , car(208), \"ð\"  )\
str = string.troque(str , car(209) , \"Ð\" )\
str = string.troque(str  , car(210), \"Ê\" )\
str = string.troque(str  , car(211), \"Ë\" )\
str = string.troque(str , car(212), \"È\"  )\
str = string.troque(str  , car(213), \"ı\" )\
str = string.troque(str , car(214), \"Í\"  )\
str = string.troque(str , car(215), \"Î\"  )\
str = string.troque(str , car(216), \"Ï\"  )\
str = string.troque(str, car(222) , \"Ì\"  )\
str = string.troque(str , car(223), \"▀\"  )\
str = string.troque(str , car(224), \"Ó\"  )\
str = string.troque(str  , car(225), \"ß\" )\
str = string.troque(str , car(226), \"Ô\"  )\
str = string.troque(str  , car(227), \"Ò\" )\
str = string.troque(str  , car(228), \"õ\" )\
str = string.troque(str , car(229), \"Õ\"  )\
str = string.troque(str , car(230), \"µ\"  )\
str = string.troque(str , car(231), \"þ\"  )\
str = string.troque(str , car(232), \"Þ\" )\
str = string.troque(str  , car(233), \"Ú\" )\
str = string.troque(str , car(234), \"Û\"  )\
str = string.troque(str , car(235), \"Ù\"  )\
str = string.troque(str  , car(236), \"ý\" )\
str = string.troque(str  , car(237), \"Ý\" )\
retorne str;\
 \
fim";

