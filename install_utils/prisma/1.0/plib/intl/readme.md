# intl.pris — Biblioteca de internacionalização para Prisma

Biblioteca Prisma para criar programas com suporte a traduções.

> Biblioteca experimental escrita 100% em Prisma, inspirada no fluxo GNU gettext.

Nota:

> Esta biblioteca implementa apenas idiomas com nplurals=2.
> Suporte completo às regras de plural do gettext poderá ser adicionado futuramente.


## Endianesss

O endianess do arquivo .mo é detectado automaticamente via magic number.



## Para que serve?

Esta biblioteca permite ler arquivos *.mo e traduzir automaticamente todas as strings marcadas para
tradução em um programa ou lib feitos em Prisma.

## Como instalar?

Antes, verifique se a biblioteca já está instalada por padrão.

Teste com `inclua 'intl'`. 
 
Se ocorrer erro, significa que ela não está instalada.


É de praxe eu incluir libs feitas em puro Prisma nos instaladores de Prisma após a criação e revisão. Então, provavelmente esta lib estará por padrão em Prisma, no github.

#### Caso ainda não esteja instalada baixe do github e instale:
 
Copie `intl.pris` para o seu projeto ou para um diretório de bibliotecas Prisma padrão.

Geralmente no linux é: `/usr/local/share/prisma/1.0/plib/`

No Windows: `C:\Prisma\1.0\bin\plibs\`

## Dependências

- Interpretador Prisma ou compilador `pric` (para embutir o código e gerar executável).

- Utilitários xgettext e msgfmt (já instalados por padrão em Linux)

Binários xgettext e msgfmt para Windows: [https://github.com/vslavik/gettext-tools-windows/releases](https://github.com/vslavik/gettext-tools-windows/releases)


## Documentação

A pasta `doc/` é gerada automaticamente e não faz parte do repositório.

Use o app de documentação prisma: minidoc intl.pris

## Como usar?

### Primeiro deve-se marcar quais strings serão traduzíveis no código fonte:

String comum:

`_("Are you sure to quit?")`

Com suporte a singular e plural (dois sublinhados):

`__("%d file found.", "%d files found.", num)`

 **Se `num` for igual a '0' ou maior que '1' será traduzido para o plural, se for 1 será para o singular.**
 
> Nota: o valor numérico (`num`) deve ser impresso junto com a string
> no próprio código, conforme o formato `%d`.

 
Aqui um exemplo mínimo completo de uso da lib:

```lua
//incluindo a biblioteca:
local intl = inclua'intl';

//carrega o arquivo pt_BR.mo, poderia ser um en_US.mo, es.mo etc
intl.carregue('pt_BR.mo');

//aliases para facilitar (em vez de usar intl._() ou intl.__() usamos só _() e __()
local _ = intl._; //normal sem plural;
local __ = intl.__; //para singular e plural (dois underlines);

funcao principal(arg)
    imprima(_("Do you want to install? [y/N]"));
    imprima(_("Bye!"));
    imprima(__("%d car\n", "%d cars\n", 0));
    retorne 0;
fim
```
No caso acima o programa foi feito em inglês com traduções para pt_BR.


### Próximo passo é extrair todas as strings traduzíveis

Usamos o comando já nativo no Linux `xgettext`  desta forma:

```
xgettext --language=C --keyword=_ --keyword=__:1,2 --output=modelo.pot meu_app.prisma
```

O xgettext processará o arquivo meu_app.prisma e gerará como saída o arquivo modelo.pot, onde estarão todas as strings para tradução.

 **Se quiser que o xgettext processe vários arquivos basta usar o asterisco e a extensão:**
 
 Exemplo: `*.prisma`
 


Cada string terá um par de chave e strings vazias para tradução (para o plural será uma a mais):

```
msgid "Do you want to install? [y/N]"
msgstr ""
```

A tradução deve ficar assim:

```
msgid "Do you want to install? [y/N]"
msgstr "Quer instalar? [s/N]"
```

Para strings com suporte a singular e plural sairá algo assim:

```
msgid "%d car\n"
msgid_plural "%d cars\n"
msgstr[0] ""
msgstr[1] ""
```

Deverá fica assim:

```
msgid "%d car\n"
msgid_plural "%d cars\n"
msgstr[0] "%d carro\n"
msgstr[1] "%d carros\n"
```

Note que os outros detalhes devem permanecer iguais, o '\n' e o '%d'.

A partir do modelo.pot é possível fazer qualquer tradução. Basta copiar e renomear para, por exemplo, pt_BR.po ou outro nome se for necessário.

O modelo.pot poderá ser distribuído com o programa para que a comunidade ou interessados façam as traduções usando apenas o modelo e compilando para .mo com o utilitário `msgfmt`.

O arquivo .mo gerado é multiplataforma!


### O cabeçalho

Antes de usar o utilitário msgfmt é preciso editar o cabeçalho gerado no modelo:

```
msgid ""
msgstr ""
"Project-Id-Version: PACKAGE VERSION\n"
"Report-Msgid-Bugs-To: \n"
"POT-Creation-Date: 2025-12-19 10:20-0400\n"
"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
"Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
"Language-Team: LANGUAGE <LL@li.org>\n"
"Language: \n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=CHARSET\n"
"Content-Transfer-Encoding: 8bit\n"
"Plural-Forms: nplurals=INTEGER; plural=EXPRESSION;\n"
```

É preciso fazer algumas alterações, principlamente na linha com 'Content-Type:' e 'Plural-Forms:'

Um exemplo editado:

```
"Project-Id-Version: mome_do_app 0.1\n"
"Report-Msgid-Bugs-To: https://github.com/seu_projeto/issues\n"
"POT-Creation-Date: 2025-12-19 10:30-0400\n"
"PO-Revision-Date: 2025-12-19 11:00-0400\n"
"Last-Translator: Seu nome completo <seu_email@email.com>\n"
"Language-Team: Portuguese (Brazil)\n"
"Language: pt_BR\n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"
"Plural-Forms: nplurals=2; plural=(n != 1);\n"
```

O restante são as traduções a serem feitas.

### Compilando para .mo

Use o comando abaixo, caso queira que o arquivo de tradução fique no mesmo local do programa.

`msgfmt pt_BR.po -o pt_BR.mo`

E este comando, caso queira que o arquivo fique instalado no sistema:

`msgfmt pt_BR.po -o /usr/share/locale/pt_BR/LC_MESSAGES/meuApp.mo`

Aqui é necessário que meuApp.mo seja o nome do seu aplicativo, por exemplo:

- calcjuros ->  /usr/share/locale/pt_BR/LC_MESSAGES/calcjuros.mo  (Linux)

- calcjuros.exe ->  ..\\share\\locale\\pt_BR\\LC_MESSAGES\\calcjuros.mo (Windows)

Fique atento onde foi instalado, pois é ali que seu programa deverá buscar o arquivo de tradução.

### Usando a lib para buscar o arquivo de tradução:

Pode usar a função de busca direta:

```lua
intl.carregue('pt_BR.mo');
```

Ou com duas funções abaixo para carregar o idioma padrão do sistema.

```lua
intl.def_caminho("/usr/share/locale/");
intl.inicie();
```
 
 **Observe que '/usr/share/locale/' é para linux, em outro sistema pode ser diferente**

Para ficar mais portátil (Linux e Windows) poderia ser: `../share/locale/`

Não se preocupe com as barras, pois, internamente a lib intl.pris faz a conversão automática para o sistema, / para *nix e \\ para Windows.



```lua
local locale;
se sis.procure('win') entao
    locale = '../share/locale/'
senaose sis.procure('linux') entao
    locale = '/usr/share/locale'   
fim
intl.def_caminho(locale);
```

## Limitação

### singular e plural:
Esta lib foi feita puramente em Prisma, tem certa compatibilidade com arquivos gerados pelo xgettext e msgfmt, mas possui uma limitação:

> Esta implementação suporta apenas idiomas com nplurals=2.

Significa que só suporta idiomas com singular/plural simples.

Não suporta regras complexas (árabe, russo, etc.)

 **Possível solução: fazer uma versão baseada na libintl em C com suporte total**


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
