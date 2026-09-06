lib PARGS v1.0
Autor: Adalberto Amorim Felipe

linguagemprisma.br4.biz  |  https://github.com/adalbermirom/linguagemprisma

Licença MIT - pode usar como quiser, sem garantias, sem responsabilizar o autor.

Esta lib serve para tratar dos argumentos de linha de comando de maneira simples e fácil.

Veja, imagine que você crie um programa que converte imagens, então você vai precisar dos seguintes
parâmetros de linha de comando:

   -saida - o nome de saida a ser gravado.
   -entrada - o nome do arquivo que será lido.
   -formato - o nome do formato a ser convetido.
   
img_conv -formato png -entrada foto.jpeg -saida foto.png
   
O código ficará:

local Pargs = inclua'pargs'

local pa = Pargs(); //cria uma nova instancia da classe.

pa:ad("-saida", 1, "Nome do arquivo a ser gravado.");
pa:ad("-entrada", 1, "Nome do arquivo original.");
pa:ad("-formato", 1, "Nome do formato de imagem a ser convertido.");

funcao principal(args)
    pa:analise(args); //args é a tabela global com os argumentos de linha de comando no Prisma.
    
    local saida = pa:obt('-saida');
    local entrada = pa:obt('-entrada');
    local formato = pa:obt('-formato');
    
    se nao saida ou nao entrada ou nao formato entao
        imprima('Erro, faltam parâmetros\n    Use: img_conv -formato formato_imagem -entrada imagem_original -saida imagem_final ');
        pa:use();
        retorne 1;
    fim
    
    //...lógica da conversão aqui (provavelmente usaria uma lib C para fazer a conversão, com os parâmetros obtidos!
    
    retorne 0;
fim


