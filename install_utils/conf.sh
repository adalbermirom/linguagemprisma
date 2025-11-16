#!/bin/bash
#--------------------------------------------------------------------------------
# CONFIGURAÇÃO: MUDE ESSAS VARIÁVEIS DE ACORDO COM SEUS ARQUIVOS DE INSTALAÇÃO: #
#                                                                               # 
#                                                                               #
#--------------------------------------------------------------------------------


NOME_PROG='Prisma 1.0.107 x86_64 - linux dez/2023'; #nome que será apresentado na instalação
AUTOR="Adalberto A. Felipe";
EMAIL="adalbermirom@gmail.com"

DESTINO=" /usr/local/bin/prisma"; #local de DESTINO da cópia.
ORIGEM="./bin/prisma"; #alvo (pasta ou arquivo) que será copiado
LICENSE="licenca.txt"
#variaveis com comandos:

SUPER="sudo"

CMD_DELETE="${SUPER} rm ${DESTINO}" #CUIDADO AO MODIFICAR PARA NÃO APAGAR PASTA DO SISTEMA
CMD_COPY="${SUPER} cp -v ${ORIGEM} ${DESTINO}"; #comando de cópia.
CMD_CHMOD=" ${SUPER} chmod +x ${DESTINO}"; #permissão para executar.

#essas funções abaixo serão executadas automaticamente pelo instalador, coloque
#comandos dentro dela se quiser, cuidado um erro pode interromper o instalador.

#tela de abertura, coloque aqui o que quiser executar antes de instalar.
ao_iniciar (){
  return;
}

ao_instalar () {
  #coloque aqui os comandos de instalação (cp ou outro);
  #instalando prisma:
  local result=`$CMD_COPY`;
  
#  instalando todos os outros componentes prisma: prismac plib/ clib/ libprisma1.0.a
#para usar comandos super_* da lib.sh é necessário declarar a global SUPER como sudo ou o correspondente a sua distro.
SUPER=sudo
super_copie bin/pric  /usr/local/bin/
super_copie bin/prismaterm /usr/local/bin
super_copie bin/prismac  /usr/local/bin/
super_copie bin/srprisma /usr/local/lib/prisma/1.0/

#cria o diretório clib se nao existir:
$SUPER install -v -d /usr/local/lib/prisma/1.0/clib/
$SUPER cp -a -v -r bin/*.so  /usr/local/lib/prisma/1.0/clib/
super_copie bin/libprisma1.0.a /usr/local/lib/

#utils:
super_copie install_utils/geany/filedefs/filetypes.Prisma.conf /usr/share/geany/filedefs/
super_copie install_utils/geany/templates/main.prisma /usr/share/geany/templates/files/
super_copie install_utils/gtksourceview-2.0/language-specs/prismabr.lang  /usr/share/gtksourceview-2.0/language-specs/
super_copie install_utils/prisma  /usr/local/share/

  msg_ok $vermelho " "  $result " " "       Processo terminado!" " " 
  return;
}

ao_desinstalar () { 	
sudo killall prisma; #mata todos os processos prisma
clear;
#apaga prisma
 $CMD_DELETE;
#apaga o restante:
super_se_arq_existir_apague /usr/local/bin/prismac
super_se_arq_existir_apague /usr/local/lib/prisma
super_se_arq_existir_apague /usr/local/lib/libprisma1.0.a

#utils:
super_se_arq_existir_apague  /usr/local/share/prisma
super_se_arq_existir_apague /usr/local/bin/prismaterm

super_se_arq_existir_apague /usr/share/geany/filedefs/filetypes.Prisma.conf
super_se_arq_existir_apague /usr/share/geany/templates/files/main.prisma
super_se_arq_existir_apague /usr/share/gtksourceview-2.0/language-specs/prismabr.lang

 clear;
 return;	
}

ao_cancelar() {
 return;
}


ao_terminar_instalacao() { #executa esta função quando termina a instalação
    $CMD_CHMOD;
	clear;
	clear;
	return;
}

ao_terminar_desinstalacao() { #executa esta função quando termina a desinstalação
	return;
}


######################################################
