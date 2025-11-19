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


if ! command -v sudo &> /dev/null
then
    # Se 'sudo' não for encontrado, tentamos 'su' (comumente usado em distros mais antigas/orientadas a root)
    if command -v su &> /dev/null
    then
        # Nota: 'su' precisa ser chamado antes do script. Deixar SUPER vazio ou tentar 'doas' é mais seguro.
        # Para evitar complicação, vamos apenas emitir um aviso se sudo não for encontrado:
        echo "AVISO: O comando 'sudo' não foi encontrado."
        echo "Por favor, execute o script como root (sudo ./instalar.sh) ou edite a variável 'SUPER'."
        
        # Tentamos 'doas' como alternativa popular (OpenBSD, algumas instalações Arch/Gentoo)
        if command -v doas &> /dev/null
        then
            SUPER="doas"
        else
            # Forçamos o usuário a executar como root. Deixamos SUPER vazio, o que
            # fará com que os comandos tentem rodar diretamente como root (se o script foi invocado com sudo).
            SUPER=""
        fi
    fi
fi



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

## lang-extensions
super_copie install_utils/extensao-editores/geany/filedefs/filetypes.Prisma.conf /usr/share/geany/filedefs/

super_copie install_utils/extensao-editores/geany/templates/main.prisma /usr/share/geany/templates/files/

super_copie install_utils/extensao-editores/gtksourceview-2.0/language-specs/prismabr.lang  /usr/share/gtksourceview-2.0/language-specs/

super_copie install_utils/prisma  /usr/local/share/

super_copie install_utils/extensao-editores/nano/prisma.nanorc /usr/share/nano/



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
