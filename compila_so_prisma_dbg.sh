#!/usr/bin/bash
#essa compilação é só para debug, testes. Use compila_linux.sh

cd ./src/
rm *.o
rm *.a
# Flags de compilação para modo debug
CFLAGS="-Og -g -Wall -Wextra -Wpedantic -DLUA_COMPAT_ALL -DLUA_USE_LINUX"
LDFLAGS="-lm -ldl -Wl,-E"

# Compilação dos arquivos fonte
for file in lapi lcode lctype ldebug ldo ldump lfunc lgc llex lmem lobject lopcodes \
            lparser lstate lstring ltable ltm lundump lvm lzio lauxlib lbaselib \
            lbitlib lcorolib ldblib liolib lmathlib loslib lstrlib ltablib loadlib \
            linit lutf8lib; do
    gcc $CFLAGS -c -o ${file}.o ${file}.c
done

# Criação da biblioteca estática
ar rcu libprisma1.0.a *.o
ranlib libprisma1.0.a

# Compilação dos binários
gcc $CFLAGS -c -o prisma.o prisma.c
gcc -o prisma prisma.o libprisma1.0.a $LDFLAGS

gcc $CFLAGS -c -o prismac.o prismac.c
gcc -o prismac prismac.o libprisma1.0.a $LDFLAGS

gcc -c -o srprisma.o srprisma.c
gcc -o srprisma srprisma.o libprisma1.0.a $LDFLAGS


