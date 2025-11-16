#!/bin/bash
preto='\E[30;47m'
vermelho='\E[31;47m'
verde='\E[32;47m'
amarelo='\E[33;47m'
azul='\E[34;47m'
magenta='\E[35;47m'
ciano='\E[36;47m'
branco='\E[37;47m'
negrito="\033[1m" 
padrao="\033[0m"

#cria os diretórios necessários antes de copiar os arquivos.
#ex.:   copie ./* /usr/local/share/minhapasta/ 
#       cria todo o caminho se não existir e copia tudo './*' do diretório atual para lá.
copie (){
	local orig="$1";
	local dest="$2";
	local fileout="$3";
	local super="$4";
	#criando os diretorios se necessário:
	$super install -v -d "${dest}"
	$super cp -a -v -r "${orig}" "${dest}/${fileout}"
}

#copie como super usuário:
#ex.:  SUPER=sudo; copie arquivo /pasta/destino/ [nome_saida] 
#      o parâmetro nome_saida é opcional, padrão é o mesmo nome do arquivo a ser copiado.
super_copie(){
	local orig="$1";
	local dest="$2";
	local fileout="$3";
	#criando os diretorios se necessário:
	$SUPER install -v -d "${dest}"
	$SUPER cp -a -v -r "${orig}" "${dest}/${fileout}"
}

espaco () {  #espaco
	echo -n "   ";
}

defina_cor()  # defina_cor $azul
{
	color=${1:-$preto}           # Defaults to black, if not specified.
    echo -e "$color"
    return	
}


# imprime uma mensagem na tela e pula para linha de baixo
# exemplo   escreval "ola mundo" $azul
escreval ()                     # Color-echo.
                             # Argument $1 = message
                             # Argument $2 = color  - 
{
local default_msg="No message passed."
                             # Doesn't really need to be a local variable.

  message=${1:-$default_msg}   # Defaults to default message.
  color=${2:-$preto}           # Defaults to black, if not specified.

  echo -en "$color"
  echo -n "$message"
  echo -e "\033[0m"  # Reset to normal.
  return
}  

#imprime uma mensagem na tela e continua na mesma linha
# Exemplo:   escreva " Uma mensagem qualquer >" $vermelho
escreva ()                     # Color-echo.
                             # Argument $1 = message
                             # Argument $2 = color
{
local default_msg="No message passed."
                             # Doesn't really need to be a local variable.

  message=${1:-$default_msg}   # Defaults to default message.
  color=${2:-$preto}           # Defaults to black, if not specified.

  echo -en "$color"
  echo -en "$message"
  echo -en "\033[0m"  # Reset to normal.

  return
}  

#esta funcao aguarda o usuário digitar um caractere, caso corresponda ao primeiro parâmetro é executado a função do 
#segundo param; caso não seja igual, é executada a função do terceiro parâmetro.
#caso 1 sim nao; 
caso () {
  read -r -s -n 1 op;
  clear;
  if [ ${op^} == ${1} ]; then
    $2
  else
    $3
  fi
}


obtcar () {  #pega um caractere sem precisar presionar enter
  N=${2:-1};
  read -s -n $N $1
}




#sobe N linhas, ex: upline 4; sobe 4 linhas
upline () {
  echo -en "\033[s\033[${1}A"	
}
#desce N linhas   downline 2; desce duas linhas
downline () {
  echo -en "\033[s\033[${1}B"
}

#cria um diretorio caso não exista.
#Exemplo:  crie_dir nova_pasta;
crie_dir() { 
  while [ "$1" != "" ]; do
    if [ ! -d "$1" ]; then
       mkdir $1
    fi
    shift
  done
    return;
}
#para função abaixo defina a variavel SUPER de acordo com sua distro (Exemplo: SUPER=sudo)
super_crie_dir(){ 
  while [ "$1" != "" ]; do
    if [ ! -d "$1" ]; then
      $SUPER mkdir "${1}"
    fi
    shift
  done
    return;
}

#executa $1 se $2 existir e o passa como parametro
#Exemplo:  se_arquivo_existe rm "*.o";
se_arq_existir() { 
    if [ -e "$2" ]; then
       $1 $2
    fi
    return;
}

#exclui um arquivo se existir, caso não exista não é mostrada nenhuma mensagem
#Exemplo:  se_arq_existir_apague "*.o";
se_arq_existir_apague() { 
    if [ -e "$1" ]; then
       rm -r $1
    fi
    return;
}
#A mesma função de cima mas com privilégios super usuário.
#Exemplo:  SUPER=sudo;   super_se_arq_existir_apague "*.o";
super_se_arq_existir_apague() { 
    if [ -e "$1" ]; then
       $SUPER rm -r $1
    fi
    return;
}

msg_sim_nao () {
	echo;
	func=${1};
	cor=${2};
	espaco; escreval  "                                                      "
	while [ "$3" != "" ]; do
	espaco; escreval "                                                      ";
	  upline 1;
	  espaco; escreval "${3}" $cor
      shift
    done;
    espaco; escreval "______________________________________________________";
	espaco; escreval " [S] - sim                                            " $azul;
    espaco; escreval " [N] - não                                            " $vermelho;
    espaco; escreval "                                                      "
    caso "S" $func;
}


rotulo () {
	cor=${1};
	#espaco; escreval  "                                                          "
	while [ "$2" != "" ]; do
	espaco; escreval "                                                          ";
	  upline 1;
	  espaco; escreval "${2}" $cor
      shift
    done;
    #espaco; escreval "__________________________________________________________";
}

msg_ok () {
	cor=${1};
	echo;            
	#espaco; escreval "                                                          ";
	espaco; escreval "                                                          ";
	while [ "$2" != "" ]; do
	espaco; escreval "                                                          ";
	  upline 1;
	  espaco; escreva "${2}\n" $cor
      shift
    done;
    espaco; escreval "                                                          ";
	espaco; escreval "                                   [ENTER para continuar] " $azul;
    #espaco; escreval "                                                          "
    obtcar g;
}

msg_cancelado () {
 echo
 espaco; escreval "                                           "
 espaco; escreval "                                           "
 espaco; escreval "    CANCELADO PELO USUÁRIO                 " $vermelho
 espaco; escreval "                                           "
 espaco; escreval "___________________________________________"
 espaco; escreval "       Enter para sair...                  " $azul
 echo
 echo 
 obtcar a;
}

msg_opcao_invalida () {   
	echo; 
	espaco; escreval  "                                          "
	espaco; escreval  "    Escolha uma opção válida              " $vermelho
    espaco; escreval  "                                          "
    espaco; escreval  "    Pressione alguma tecla para repetir!  " $azul;
    espaco; escreval  "                                          "
    echo
    obtcar g;
	
}

pule_linha() {
  echo;
}


#####################  DETECT OS AND ARCH ###########################


obt_sis_nome(){
 local Kernel=$(uname -s)
 case "$Kernel" in
    Linux)  Kernel="linux"              ;;
    Darwin) Kernel="mac"                ;;
    FreeBSD)    Kernel="freebsd"            ;;
    *) Kernel="";;
 esac
 local _ref=$1;
 if [[ "$__ref" ]]; then
        eval $__ref="'$Kernel'"
    else
        echo "$Kernel"
    fi 
}

obt_sis_arquitetura(){
# Get the machine Architecture
local Architecture=$(uname -m)
case "$Architecture" in
    x86)    Architecture="x86"                  ;;
    i686)    Architecture="x86"                  ;;
    ia64)   Architecture="ia64"                 ;;
    i?86)   Architecture="x86"                  ;;
    amd64)  Architecture="amd64"                    ;;
    x86_64) Architecture="x86_64"                   ;;
    sparc64)    Architecture="sparc64"                  ;;
* )    Architecture="" ;;
esac
 local _ref=$1;
 if [[ "$__ref" ]]; then
        eval $__ref="'$Architecture'"
    else
        echo "$Architecture"
    fi 
 return 
}


obt_sis_info(){
 local Kernel=$(uname -s)
 case "$Kernel" in
    Linux)  Kernel="linux"              ;;
    Darwin) Kernel="mac"                ;;
    FreeBSD)    Kernel="freebsd"            ;;
    *) Kernel="";;
 esac
 local __ref1=$1;
 if [[ "$__ref1" ]]; then
        eval $__ref1="'$Kernel'"
    else
        echo "$Kernel"
    fi 

# Get the machine Architecture
local Architecture=$(uname -m)
case "$Architecture" in
    x86)    Architecture="x86"                  ;;
    i686)    Architecture="x86"                  ;;
    ia64)   Architecture="ia64"                 ;;
    i?86)   Architecture="x86"                  ;;
    amd64)  Architecture="amd64"                    ;;
    x86_64) Architecture="x86_64"                   ;;
    sparc64)    Architecture="sparc64"                  ;;
* )    Architecture="" ;;
esac
 local __ref2=$2;
 if [[ "$__ref2" ]]; then
        eval $__ref2="'$Architecture'"
    else
        echo "$Architecture"
    fi 
 return 
}

pause (){
	espaco; escreval "                                   [ENTER para continuar] " $azul;
	read gg;
}

grave_sis_info(){
	info_sis_file="${1}"
echo -e "Informações do sistema onde Prisma foi compilado:\n" > $info_sis_file
uname -s >> $info_sis_file
uname -r >> $info_sis_file
uname -v >> $info_sis_file
uname -m >> $info_sis_file
uname -o >> $info_sis_file
echo -e "\n-------------------------------\n"  >> $info_sis_file
echo -e "Informações do compilador usado:\n"  >> $info_sis_file
#informações a serem obtidas:
gcc -### 2>&1 | grep "gcc version" >> $info_sis_file
gcc -### 2>&1 | grep Target >> $info_sis_file
gcc -### 2>&1 | grep Thread >> $info_sis_file

}

#####################################################################




