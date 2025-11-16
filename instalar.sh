#!/bin/bash

#ATENÇÃO - não modifique este script a menos que saiba o que está fazendo
# para personalizar um instalador - modifique apenas o arquivo conf.sh sem alterar as variáveis.

#include lib
.  ./install_utils/lib.sh
#include conf
. ./install_utils/conf.sh

clear;
pule_linha;
rotulo "${vermelho}" "      __________| PROCESSO DE INSTALAÇÃO |__________" " " 
rotulo $preto " [ ${NOME_PROG}]" " ";
rotulo $vermelho "  Maximize a janela do terminal para" "  uma melhor visualização";

pause;
iniciar () {
 ao_iniciar;
 clear
 echo
 rotulo $azul " " "  Instalar:" "  ${NOME_PROG} " " " "  ${AUTOR}" "  ${EMAIL}";
 rotulo $preto "__________________________________________________________";
 rotulo $preto "  Escolha uma letra abaixo:"
 rotulo $azul " " "  [A]-> Aceitar licença e instalar" "  [C]-> Cancelar" "  [V]-> Ver licença";
 rotulo $vermelho "  [D]-> Desinstalar" " ";
 #rotulo $vermelho "  (para melhor visualizar, coloque em tela cheia)"
}

instalar (){
 clear
 echo;
 rotulo $vermelho " " "  Para instalar digite sua senha sudo:"
 rotulo $preto "___________________________________________"
	
 echo;
 ao_instalar;
 clear;
 ao_terminar_instalacao;
 exit 0;
  
}

desinstalar (){	
  clear;
  echo
  rotulo $vermelho " " "  Digite a senha sudo para desinstalar " " ";
  echo
  ao_desinstalar;
  clear;
  msg_ok $vermelho " " "       Processo terminado!" 
  clear;
  ao_terminar_desinstalacao;
  exit 0;
}




cancelar (){
 ao_cancelar;
 clear
 msg_cancelado;
 clear;
 exit 1;
}

mostra_licenca (){
	clear;
	se_arq_existir cat ${LICENSE} | more;
	obtcar;
}

menu () {
 while [ 1 ]; do
 iniciar; #menu inicial
 obtcar op;	
 case "$op" in
        a)  clear;
            msg_sim_nao instalar $vermelho "  Quer realmente instalar? " " " "  '${NOME_PROG}'" " "; 
            ;;
        A)  clear;
            msg_sim_nao instalar $vermelho "  Quer realmente instalar? " " " "  '${NOME_PROG}'" " "; 
            ;;
         
        C)   clear;
             msg_sim_nao cancelar $vermelho "  Quer realmente cancelar a instalação? " " " "  '${NOME_PROG}'" " "; 
            
            ;;
        c)   clear;
             msg_sim_nao cancelar $vermelho "  Quer realmente cancelar a instalação? " " " "  '${NOME_PROG}'" " "; 
            
            ;;
         
        d)  
            clear;
            msg_sim_nao desinstalar $vermelho "  Quer realmente remover? " " " "  '${NOME_PROG}'" " "; #se sim (s) entao desinstalar é executada
            ;;
        D)  
            clear;
            msg_sim_nao desinstalar $vermelho "  Quer realmente remover? " " " "  '${NOME_PROG}'" " "; #se sim (s) entao desinstalar é executada
            ;;
        v)
            mostra_licenca;
            ;;
        V)
            mostra_licenca;
            ;;
        *)
            clear;
            msg_opcao_invalida;
            ;;
 esac
 done
}

#mostra_licenca ; #mostra lincença
menu;#menu principal

