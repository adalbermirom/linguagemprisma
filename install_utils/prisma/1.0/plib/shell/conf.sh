#!/bin/bash
#--------------------------------------------------------------------------------
# CONFIGURAÇÃO: MUDE ESSAS VARIÁVEIS DE ACORDO COM SEUS ARQUIVOS DE INSTALAÇÃO: #
#                                                                               # 
#                                                                               #
#--------------------------------------------------------------------------------


NOME_PROG='lib shell.pris 0.2 para prisma linux'; #nome que será apresentado na instalação
AUTOR="Adalberto A. Felipe";
EMAIL="adalbermirom@gmail.com"

DESTINO="/usr/local/share/prisma/1.0/plib/shell"; #local de DESTINO da cópia.
ORIGEM="ini.pris"; #alvo (pasta ou arquivo) que será copiado
LICENSE="licenca.txt"
#variaveis com comandos:

SUPER="sudo"

CMD_DELETE="${SUPER} rm -r ${DESTINO}" #CUIDADO AO MODIFICAR PARA NÃO APAGAR PASTA DO SISTEMA
CMD_COPY="${SUPER} cp -a -v ${ORIGEM} ${DESTINO}"; #comando de cópia.
#CMD_CHMOD=" ${SUPER} chmod +x ${DESTINO}"; #permissão para executar.

#essas funções abaixo serão executadas automaticamente pelo instalador, coloque
#comandos dentro dela se quiser, cuidado um erro pode interromper o instalador.

#tela de abertura, coloque aqui o que quiser executar antes de instalar.
ao_iniciar (){
  return;
}

ao_instalar () {
  #coloque aqui os comandos de instalação (cp ou outro);
  #super_crie_dir "${DESTINO}"
  super_crie_dir "${DESTINO}"
  local result=`$CMD_COPY`;
  clear;
  msg_ok $vermelho " "  $result " " "       Processo terminado!" " " 
  return;
}

ao_desinstalar () { 
 $CMD_DELETE;
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
