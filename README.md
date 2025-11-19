# 🌟 Prisma - A Linguagem de Programação (Baseada em Lua5.2)

![prisma-logo](https://linguagemprisma.br4.biz/blog/wp-content/uploads/2015/08/logo_prisma148.png)


Prisma é um ambiente de *runtime* completo e uma linguagem de programação com sintaxe e bibliotecas totalmente em **Português**. Baseada no *runtime* Lua, Prisma oferece alta performance, portabilidade e uma VM robusta, otimizada para o desenvolvimento de *scripts* de sistema, ferramentas de linha de comando e aplicações de *runtime* rápido.

O projeto visa tornar a programação de baixo nível acessível a falantes nativos de Português, mantendo a interoperabilidade com a API C do Lua.

Prisma é um fork de lua5.2 modificado. Várias funções e características, que não existem em Lua5.2, foram implementadas. Um exemplo é a indexação de strings:

```lua
    local s = "Prisma";
    imprima( s[1] ); // --> P
```

---

## 🚀 Status e Versão

| Item | Detalhes |
| :--- | :--- |
| **Versão Atual** | `1.0` |
| **Status** | Estável  |
| **Licença** | Consulte o arquivo `LICENSE.txt` para detalhes (MIT). |
| **Contato** | linguagemprisma@gmail.com |
| **Página** | [linguagemprisma.br4.biz](http://linguagemprisma.br4.biz) |

---

## ✨ Características Principais

* **Sintaxe em Português:** Palavras-chave e bibliotecas nativas traduzidas para o Português (ex: `funcao` para `function`, `se` para `if`, `imprima` para `print`).
* **Portabilidade:** Compila e roda em diversos sistemas operacionais, incluindo **Linux** e **Windows** (x32/x64).
* **Performance:** Herda a **velocidade** e a **pequena pegada** de memória da Máquina Virtual Lua.
* **Ecossistema Completo:** Inclui um compilador (`pric`), bibliotecas padrão e extensões.
* **Integração:** Scripts de suporte para integração com editores populares como **Geany** e **GTK Source View**.

---

## 🛠️ Como Compilar o Prisma

O projeto utiliza *scripts* de *shell* e *batch* para compilação multiplataforma.

### Pré-requisitos

* Um compilador C (GCC no Linux/MinGW no Windows).
* Ferramentas padrão de *build* (Make).

### 1. Compilação em Linux

Utilize o *script* `compila_linux.sh` para compilar o interpretador (`prisma`), o compilador (`prismac`) e a biblioteca estática (`libprisma1.0.a`).

```bash
# Navegue para o diretório raiz do projeto
./compila_linux.sh

#após instalar:
prisma -v
```

### 2. Compilação em Windows

Utilize o *script* `compila_win_x32.bat ou compila_win_x64.bat` para compilar o interpretador (`prisma.exe`), o compilador (`prismac.exe`) e a biblioteca estática (`libprisma1.0.a`).

 **Necessário configurar o caminho do MinGW nos arquivos bat.**
```bash
# Navegue para o diretório raiz do projeto
./compila_win_x64.bat

prisma -v
```

### 2. Compilação em outras plataformas

Ainda não testei compilar em outras plataformas, é quase certo que Prisma, assim como Lua, compile em qualquer plataforma com compilador C89 acima. Com algumas adaptações. Para compilar liso em C89 é preciso desabilitar os goto computados em luaconf.h na linha 610: 

```C
610   #define _USE_GOTO_COMPUTADO
```

### Caso queira um interpretador interativo:

 **Digite no terminal:** `prismaterm`


## Syntax highlighting

Suporte para coloração de vários editores neste link: [https://github.com/adalbermirom/prisma-syntax-conf](https://github.com/adalbermirom/prisma-syntax-conf)


## 📦 Estrutura do Projeto
| Diretório | Conteúdo | Descrição |
| :--- | :--- | :--- |
| `src/` | Fontes C | O núcleo do interpretador e da Máquina Virtual (VM), incluindo os arquivos modificados do Lua. |
| `std_docs/` | Libs Padrão (`.pris`) | Documentação da bibliotecas padrão do Prisma (ex: `mat.pris`, `string.pris`). |
| `install_utils/` | Ferramentas de Instalação | Scripts de instalação e recursos de documentação. |
| `exemplos/` | Exemplos de Código | Scripts de demonstração de recursos da linguagem. |
| `bin/` | Binários Compilados | Onde os executáveis (`prisma`, `prismac`) e bibliotecas estáticas são gerados (ignorado pelo Git). |



## ⚙️ Convenções e API C

O Prisma utiliza uma camada de abstração sobre a API C do Lua, definida em `src/prisma.h`, para padronizar e simplificar a criação de módulos C para o *runtime*.

* **API C Personalizada:** Macros como `PFUNC`, `PRISMA_FUNC` e `PRIS_REGISTER_META` são usadas para registro rápido de funções e metatabelas.
* **Args da Linha de Comando:** A tabela global `args` segue o padrão Lua 5.2/5.3: o nome do interpretador está em **`args[-1]`** e o nome do *script* está em **`args[0]`**.



## Alguns exemplos:

### Olá mundo em Prisma:
```lua
local txt = "Olá mundo em Prisma";

funcao principal()
    imprima(txt);
    retorne 0;
fim

```

### Laço de repetição 'para'
```lua
funcao principal()
    
    para i = 1, 10 inicio
        imprima(i);
    fim
    retorne 0;
fim

```

### Gravando dados em um arquivo:
```lua
local arq = "teste.txt";

funcao principal()
    local a, err = es.abra('teste.txt', 'escrita');
    se nao a entao erro(err); fim
    a:escreva('Uma linha gravada no arquivo');
    a:feche();
    retorne 0;
fim

```









