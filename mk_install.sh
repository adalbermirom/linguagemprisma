#!/bin/bash

#script shell para criar automaticamente um instalador Prisma em Linux, self-extract.run e um compactado .txz

. ./lib.sh

PASTA_INSTALL=prisma_install_linux
NOME_INSTALL=prisma_install_linux

crie_dir $PASTA_INSTALL
crie_dir A_instaladores_

cp -r -v -a bin $PASTA_INSTALL
cp -r -v -a binstall $PASTA_INSTALL
cp -r -v -a conf.sh $PASTA_INSTALL
cp -r -v -a instalar.sh $PASTA_INSTALL
cp -r -v -a lib.sh $PASTA_INSTALL
cp -r -v -a leia-me.txt $PASTA_INSTALL
cp -r -v -a install_utils $PASTA_INSTALL
cp -r -v -a Notas_da_versao_nov_2023.txt $PASTA_INSTALL
cp -r -v -a info_sis_prisma.txt  $PASTA_INSTALL

#criando mki instaler:
mki -i "${PASTA_INSTALL}" "A_instaladores_/${NOME_INSTALL}.run"
#para instalar execute no terminal depois.

#criando o binstall:
#binstall -mki "${PASTA_INSTALL}" "${NOME_INSTALL}"
tar -cf - "${PASTA_INSTALL}" | xz -9 -c - > "A_instaladores_/${NOME_INSTALL}.tar.xz"
#para extrair use:
#tar -xvf  "exemplo.tar.xz" -C "pasta_exemplo/"
#Necessário o binstall no sistema.
# binstall prog.txz.binstall  --> descompacta e executa o instalador.
# binstall -x prog.txz.binstall --> apenas descompacta.

#cria o arquivo com os fontes:
crie_dir prisma_fontes
cp -r -v -a prisma prisma_fontes/
cp -r -v -a install_utils prisma_fontes/
cp -r -v -a compila_so_prisma.sh prisma_fontes/
cp -r -v -a compila_so_prisma.bat prisma_fontes/
cp -r -v -a conf.sh prisma_fontes/
cp -r -v -a instalar.sh prisma_fontes/
#cp -r -v -a info_sis_prisma.txt prisma_fontes/
cp -r -v -a leia-me.txt prisma_fontes/
cp -r -v -a licenca.txt prisma_fontes/
cp -r -v -a doc prisma_fontes/
tar -cf - prisma_fontes | xz -9 -c - > "A_instaladores_/prisma_fontes.tar.xz"

rm -R -v prisma_fontes
rm -R -v "${PASTA_INSTALL}"


