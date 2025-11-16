rm *.o
echo "aguarde o processo";
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lapi.o lapi.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lcode.o lcode.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lctype.o lctype.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o ldebug.o ldebug.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o ldo.o ldo.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o ldump.o ldump.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lfunc.o lfunc.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lgc.o lgc.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o llex.o llex.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lmem.o lmem.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lobject.o lobject.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lopcodes.o lopcodes.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lparser.o lparser.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lstate.o lstate.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lstring.o lstring.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o ltable.o ltable.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o ltm.o ltm.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lundump.o lundump.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lvm.o lvm.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lzio.o lzio.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lauxlib.o lauxlib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lbaselib.o lbaselib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lbitlib.o lbitlib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lcorolib.o lcorolib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o ldblib.o ldblib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o liolib.o liolib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lmathlib.o lmathlib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o loslib.o loslib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lstrlib.o lstrlib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o ltablib.o ltablib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o loadlib.o loadlib.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o linit.o linit.c
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o lutf8lib.o lutf8lib.c
ar rcu libprisma.a lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o lmathlib.o loslib.o lstrlib.o ltablib.o loadlib.o linit.o lutf8lib.o 

ranlib libprisma.a
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o prisma.o prisma.c
gcc -o prisma   prisma.o libprisma.a -lm -Wl,-E -ldl   
gcc -O2 -Wall -DLUA_COMPAT_ALL -DLUA_USE_LINUX    -c -o prismac.o prismac.c
gcc -o prismac   prismac.o libprisma.a -lm -Wl,-E -ldl   
echo "terminado .. Enter para continuar..."
read g;

clear
./prisma

