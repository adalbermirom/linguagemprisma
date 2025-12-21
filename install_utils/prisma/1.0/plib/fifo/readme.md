# fifo.pris — Biblioteca de comunicação inter processo (ipc) em Prisma

Biblioteca Prisma para comunicação entre programas feitos em Prisma, através de um arquivo especial chamado fifo (first in first out).

### Nota:

> Esta lib é suportada somente em sistemas posix, o Windows não tem suporte a arquivos fifo.



## Para que serve?

Esta biblioteca permite que um programa Prisma 'A' troque dados com um programa Prisma 'B', onde 'B' funciona como um servidor, e 'A' como um cliente. 

## Como instalar?

Antes, verifique se a biblioteca já está instalada por padrão.

Teste com `inclua 'fifo'`. 
 
Se ocorrer erro, significa que ela não está instalada.


É de praxe eu incluir libs feitas em puro Prisma nos instaladores de Prisma após a criação e revisão. Então, provavelmente esta lib estará por padrão em Prisma, no github.

#### Caso ainda não esteja instalada baixe do github e instale:
 
Copie `fifo.pris` para o seu projeto ou para um diretório de bibliotecas Prisma padrão.

Geralmente no linux é: `/usr/local/share/prisma/1.0/plib/`

No Windows: `C:\Prisma\1.0\bin\plibs\`

#### Usando o make

 **Para compilar em bytecode Prisma:**
```
make
```

**Para instalar (deve ser em privilégio de root, no Ubuntu, por exemplo, se usa o 'sudo')**

```
make install 
```
**Para gerar a documentação (pasta doc):**
```
make doc
```

**Limpar o arquivo compilado e a pasta doc:**
```
make clean
```

**Para desinstalar (Necessário privilégio root):**

```
make uninstall
```


## Dependências

- Interpretador Prisma ou compilador `pric` (para embutir o código e gerar executável).

- Ambiente Linux ou compatível com posix para suporte a arquivos FIFO.

## Documentação

A pasta `doc/` é gerada automaticamente e não faz parte do repositório.

Use o app de documentação prisma: minidoc intl.pris

## Como usar?

São necessário dois programas: um espera por comunicação (servidor) e outro envia requisição (cliente).

#### Exemplo cliente

```lua

local Fifo = inclua'fifo';

local cliente = Fifo.Cliente('/tmp/8080')

poe'Digite algo para o Servidor / sair'
local resp = '';
local id = '';
enquanto 1 inicio
    imprimaf('>> ');
    local txt = leia(); //espera o usuário digitar algo!
    resp = cliente:requisicao(txt);
    
    imprima('Servidor responde:', resp);
    
    se txt == 'sair' entao
        quebre;
    fim
    
fim
```

#### Exemplo servidor

```
local Fifo = inclua'fifo'

local fifo = Fifo.Serv('/tmp/8080');

poe'Servidor fifo aguardando cliente';

fifo:loop(funcao(msg)
    imprima('Cliente->', msg); //imprime a mensagem do cliente
    
    se msg == 'sair' entao
        imprima('Deligando o servidor...');
        retorne nulo, "Desligando servidor.."; //retornando nulo o servidor quebra o loop();
    fim
    
    retorne 'Olá do servidor.'; //retorna uma string para o cliente.
fim)

fifo:feche();
```

Ao usar ipc via fifo, deve-se tomar cuidado, pois um erro trava os dois programas, difícil debug.

Aconselho o uso para projetos simples. 

Encorajo o uso de socket (comunicação via tcp ip), para mais controle e robustez.

**Para mais detalhes veja a pasta exemplos**

## License

 A licença é totalmente permissiva, domínio público.
 
 *Use como quiser, sem nenhum custo. O autor não poderá ser responsabilidado pelo uso.*
 

```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>
```
