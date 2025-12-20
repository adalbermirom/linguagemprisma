#!/bin/bash

# Caso o arquivo já compilado não seja compatível, compile novamente
# chamando este script ou digitando diretamente no terminal o comando abaixo.
prismac -o ini.pris args.pris

#para gerar a documentação (Necessário minidoc instalado).
minidoc args.pris
#será gerada uma pasta doc/ procure dentro dela args.html

