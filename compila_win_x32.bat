@echo off
@echo
@cls
@echo  ....................................................................
@echo  :                 Compilar Prisma em Windows x86 (32bits)          :
@echo  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
@echo  ::  PARA SAIR CLIQUE NO X DA JANELA OU ENTER PARA COMPILAR PRISMA ::
@echo  ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
@PAUSE


@REM SE NECESSÁRIO PODE PASSAR O CAMINHO COMPLETO DOS EXECUTÁVEIS ABAIXO:
@SET PATH=%PATH%;C:\\MinGW\\bin
@SET COMPILADOR=C:\MinGW\bin\gcc.exe
@SET WINDRES=C:\MinGW\bin\windres.exe
@SET AR=C:\MinGW\bin\\ar.exe
@SET RANLIB=C:\MinGW\bin\\ranlib
@SET PARAMS=-O2 -Wall
@SET DEF=-DPRISMA_COMPAT_ALL -D_WIN_XP_
@SET PRISMA_PATH=src\\
@SET BIN_PATH=bin32\\
@SET OBJ_PATH=prisma\\

@SET LIB_PRISMA=libprisma1.0.a
@SET PROG_PRISMA=prisma.exe
@SET PROG_PRISMAC=prismac.exe
@rem @SET DLL_PRISMA=prisma1.0.dll
@SET DLL_PRISMA=prisma1.0.dll
@SET RESOURCE_PRISMA=resource.rc


@REM x86_64-w64-mingw32-gcc  -O2 -Wall -DPRISMA_COMPAT_ALL     -c -o  obj\\lapi.o prisma\\lapi.c


@cls;

@mkdir %BIN_PATH%
@mkdir %OBJ_PATH%

@ECHO "REMOVENDO COMPILACAO ANTERIOR"
@DEL %OBJ_PATH%\\*.o
@del %BIN_PATH%\\*.exe
@del %BIN_PATH%\\*1.0.dll
@DEL %BIN_PATH%\\*.a

@cls

@echo  iniciando o processo, aguarde!
@echo ...........................................
@echo compilando: prisma.c, prismac.c, srprisma.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o  %OBJ_PATH%prisma.o %PRISMA_PATH%prisma.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%prismac.o %PRISMA_PATH%prismac.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o  %OBJ_PATH%srprisma.o %PRISMA_PATH%srprisma.c



@rem %COMPILADOR% %PARAMS% %DEF%     -c -o  %OBJ_PATH%prismaAuto.o %PRISMA_PATH%prismaAuto.c
@echo compilando: lapi.c, lcode.c, lctype.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o  %OBJ_PATH%lapi.o %PRISMA_PATH%lapi.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o  %OBJ_PATH%lcode.o %PRISMA_PATH%lcode.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lctype.o %PRISMA_PATH%lctype.c
@echo compilando: ldebug.c ldo.c ldump.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%ldebug.o %PRISMA_PATH%ldebug.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%ldo.o %PRISMA_PATH%ldo.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%ldump.o %PRISMA_PATH%ldump.c
@echo compilando: lfunc.c lgc.c llex.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lfunc.o %PRISMA_PATH%lfunc.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lgc.o %PRISMA_PATH%lgc.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%llex.o %PRISMA_PATH%llex.c
@echo compilando: lmem.c lobject.c lopcodes.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lmem.o %PRISMA_PATH%lmem.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lobject.o %PRISMA_PATH%lobject.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lopcodes.o %PRISMA_PATH%lopcodes.c
@echo compilando: lparser.c lstate.c lstring.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lparser.o %PRISMA_PATH%lparser.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lstate.o %PRISMA_PATH%lstate.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lstring.o %PRISMA_PATH%lstring.c
@echo compilando: ltable.c ltm.c lundump.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%ltable.o %PRISMA_PATH%ltable.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%ltm.o %PRISMA_PATH%ltm.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lundump.o %PRISMA_PATH%lundump.c
@echo compilando: lvm.c lzio.c lauxlib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lvm.o %PRISMA_PATH%lvm.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lzio.o %PRISMA_PATH%lzio.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lauxlib.o %PRISMA_PATH%lauxlib.c
@echo compilando: lbaselib.c lbitlib.c lcorolib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lbaselib.o %PRISMA_PATH%lbaselib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lbitlib.o %PRISMA_PATH%lbitlib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lcorolib.o %PRISMA_PATH%lcorolib.c
@echo compilando: ldblib.c liolib.c lmathlib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%ldblib.o %PRISMA_PATH%ldblib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%liolib.o %PRISMA_PATH%liolib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lmathlib.o %PRISMA_PATH%lmathlib.c
@echo compilando: loslib.c lstrlib.c ltablib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%loslib.o %PRISMA_PATH%loslib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lstrlib.o %PRISMA_PATH%lstrlib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%ltablib.o %PRISMA_PATH%ltablib.c
@echo compilando: loadlib.c linit.c lutf8lib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%loadlib.o %PRISMA_PATH%loadlib.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%linit.o %PRISMA_PATH%linit.c
@%COMPILADOR%  %PARAMS% %DEF%     -c -o %OBJ_PATH%lutf8.o %PRISMA_PATH%lutf8lib.c


@echo ...........................................
@echo criando prisma1.0.dll
@%COMPILADOR%  -shared -o %BIN_PATH%%DLL_PRISMA% %OBJ_PATH%lapi.o %OBJ_PATH%lcode.o %OBJ_PATH%lctype.o %OBJ_PATH%ldebug.o %OBJ_PATH%ldo.o %OBJ_PATH%ldump.o %OBJ_PATH%lfunc.o %OBJ_PATH%lgc.o %OBJ_PATH%llex.o %OBJ_PATH%lmem.o %OBJ_PATH%lobject.o %OBJ_PATH%lopcodes.o %OBJ_PATH%lparser.o %OBJ_PATH%lstate.o %OBJ_PATH%lstring.o %OBJ_PATH%ltable.o %OBJ_PATH%ltm.o %OBJ_PATH%lundump.o %OBJ_PATH%lvm.o %OBJ_PATH%lzio.o %OBJ_PATH%lauxlib.o %OBJ_PATH%lbaselib.o %OBJ_PATH%lbitlib.o %OBJ_PATH%lcorolib.o %OBJ_PATH%ldblib.o %OBJ_PATH%liolib.o %OBJ_PATH%lmathlib.o %OBJ_PATH%loslib.o %OBJ_PATH%lstrlib.o %OBJ_PATH%ltablib.o %OBJ_PATH%loadlib.o %OBJ_PATH%linit.o %OBJ_PATH%lutf8.o


@echo criando libprisma1.0.a
@%AR% rcu %BIN_PATH%%LIB_PRISMA% %OBJ_PATH%lapi.o %OBJ_PATH%lcode.o %OBJ_PATH%lctype.o %OBJ_PATH%ldebug.o %OBJ_PATH%ldo.o %OBJ_PATH%ldump.o %OBJ_PATH%lfunc.o %OBJ_PATH%lgc.o %OBJ_PATH%llex.o %OBJ_PATH%lmem.o %OBJ_PATH%lobject.o %OBJ_PATH%lopcodes.o  %OBJ_PATH%lparser.o %OBJ_PATH%lstate.o %OBJ_PATH%lstring.o %OBJ_PATH%ltable.o %OBJ_PATH%ltm.o %OBJ_PATH%lundump.o %OBJ_PATH%lvm.o %OBJ_PATH%lzio.o %OBJ_PATH%lauxlib.o %OBJ_PATH%lbaselib.o %OBJ_PATH%lbitlib.o %OBJ_PATH%lcorolib.o %OBJ_PATH%ldblib.o %OBJ_PATH%liolib.o %OBJ_PATH%lmathlib.o %OBJ_PATH%loslib.o %OBJ_PATH%lstrlib.o %OBJ_PATH%ltablib.o %OBJ_PATH%loadlib.o %OBJ_PATH%linit.o %OBJ_PATH%lutf8.o

@%RANLIB% %BIN_PATH%%LIB_PRISMA%

@rem compilando o resource (icone e tema)

@%WINDRES% %PRISMA_PATH%%RESOURCE_PRISMA% %OBJ_PATH%resource.o

@echo criando prisma.exe, prismac.exe, igprisma.exe, srprisma.exe, igsrprisma.exe

@%COMPILADOR%  -o %BIN_PATH%%PROG_PRISMA% %OBJ_PATH%resource.o  %OBJ_PATH%prisma.o %BIN_PATH%%DLL_PRISMA% -lm
@%COMPILADOR%  -o %BIN_PATH%%PROG_PRISMAC%   %OBJ_PATH%prismac.o %BIN_PATH%%DLL_PRISMA% -lm
@%COMPILADOR%  -o %BIN_PATH%ig%PROG_PRISMA%   %OBJ_PATH%prisma.o %OBJ_PATH%resource.o %BIN_PATH%%DLL_PRISMA% -lm -mwindows

@%COMPILADOR%  -o %BIN_PATH%igsrprisma.exe   %OBJ_PATH%srprisma.o %BIN_PATH%%DLL_PRISMA% -lm -mwindows
@%COMPILADOR%  -o %BIN_PATH%srprisma.exe   %OBJ_PATH%srprisma.o  %BIN_PATH%%DLL_PRISMA% -lm

@echo criando pric compiler
@rem compilando pric e embutindo em srprisma.
@%BIN_PATH%%PROG_PRISMA% install_utils/pric/pric.prisma -s %BIN_PATH%srprisma.exe  -o %BIN_PATH%pric.exe install_utils/pric/pric.prisma -linstall_utils/prisma/1.0/plib/estrito.pris

@echo criando prismaterm.exe
@%BIN_PATH%pric.exe -o %BIN_PATH%prismaterm.exe install_utils/prismaterm/prismaterm.prisma
@echo criando configure.exe
@%BIN_PATH%pric.exe -o %BIN_PATH%configure.exe %BIN_PATH%configure.prisma

@echo criando prismacod.exe
@%BIN_PATH%pric.exe -o %BIN_PATH%prismacod.exe install_utils\prismacod\usr\local\bin\prismacod -s %BIN_PATH%igsrprisma.exe

@rem copiando as libs prisma:
@Xcopy /S /I /Y install_utils\\prisma\\1.0\\plib %BIN_PATH%plibs > nul


@set grep=%BIN_PATH%prisma grep.prisma

@rem info do sistema:
@chcp 855
@echo %date% > %BIN_PATH%info_sis_win.txt 
@echo+ >> %BIN_PATH%info_sis_win.txt 
@echo SISTEMA: >> %BIN_PATH%info_sis_win.txt 
@systeminfo | %grep% "Nome do sistema" >> %BIN_PATH%info_sis_win.txt 
@systeminfo | %grep% "Ver" >> %BIN_PATH%info_sis_win.txt
@systeminfo | %grep% "Modelo" >> %BIN_PATH%info_sis_win.txt
@systeminfo | %grep% "Tipo" >> %BIN_PATH%info_sis_win.txt
@systeminfo | %grep% "Memória física total" >> %BIN_PATH%info_sis_win.txt
@echo: >> %BIN_PATH%info_sis_win.txt 
@echo COMPILADOR: >> %BIN_PATH%info_sis_win.txt 

@%COMPILADOR% -### 2>&1 | %grep% "gcc version" >> %BIN_PATH%info_sis_win.txt
@%COMPILADOR% -### 2>&1 | %grep% Target >> %BIN_PATH%info_sis_win.txt
@%COMPILADOR% -### 2>&1 | %grep% Thread >> %BIN_PATH%info_sis_win.txt

@echo ...........................................
@echo     Processo terminado!
@echo ___________________________________________
@PAUSE

