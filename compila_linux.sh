#!/bin/bash
#echo "Se quiser sair agora pressione o x fechar da janela\nOu tecle ENTER para continuar";


#include lib.sh
. ./install_utils/lib.sh


#rm src/*.o
#rm src/*.a

#----------------------------------------------------
#----------------------------------------------------


#compilacao
cd src/
rotulo $vermelho "                                                    "
rotulo $vermelho "                 Compilando Prisma                  "
rotulo $vermelho "                                                    "
#executa o arquivo make dentro da pasta prisma 'makefile'
make linux
if [ $? -eq 0 ] ; then
  echo ok!
else
  echo -e "Erro na compilação\nEnter para continuar..."
  read g;
  exit 1;
fi



#compila srprisma
gcc -c -o srprisma.o srprisma.c
gcc -o srprisma srprisma.o libprisma1.0.a -lm -Wl,-E -ldl #-lreadline
cd ..


#usando o novo comando 'copie' que está em ./lib.sh
copie src/prisma bin/
copie src/prismac bin/
copie src/libprisma1.0.a bin/
copie src/srprisma bin/

#install_utils/prisma/1.0/plib
#compila o pric.prisma para pric, embutindo a lib estrito.pris
cp -r -a -v install_utils/prisma/1.0/plib/estrito.pris ./
./bin/prisma install_utils/pric/pric.prisma -o bin/pric install_utils/pric/pric.prisma -l ./install_utils/prisma/1.0/plib/estrito.pris -s ./bin/srprisma
rm -v ./estrito.pris
./bin/pric -p bin/prismaterm install_utils/prismaterm/prismaterm.prisma



#gravando as informações da máquina e do compilador: função em ./lib.sh
grave_sis_info "info_sis_prisma.txt"

echo -e "\n\n------------------------------------------------------------\n\n"

echo
echo
rotulo $vermelho "                                                        "
rotulo $vermelho " Processo terminado, pressione alguma tecla para continuar.. "
rotulo $vermelho " Verifique pasta bin                                    "
echo
read g;
#prismaterm
./instalar.sh



