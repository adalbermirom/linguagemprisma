# Num64

Biblioteca de números inteiros de 64 bits (`int64` e `uint64`) para a linguagem Prisma.

[![Licença: MIT](https://img.shields.io/badge/Licen%C3%A7a-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Versão](https://img.shields.io/badge/Vers%C3%A3o-0.1-green.svg)]()
[![Autor](https://img.shields.io/badge/Autor-Adalberto%20Amorim%20Felipe-orange.svg)]()

---

## 📖 Sobre o Projeto

O **Prisma 1.0** utiliza números de ponto flutuante (`double`) por padrão, o que limita a precisão exata de números inteiros até **2⁵³** (53 bits).

A biblioteca **Num64** foi desenvolvida e embutida diretamente no interpretador Prisma para suprir essa necessidade, permitindo criar, converter e manipular números inteiros de 64 bits com sinal (`int64`) e sem sinal (`uint64`) com total precisão matemática e segurança de memória.

---

## ✨ Recursos Suportados

* **Tipos:** `int64` (com sinal) e `uint64` (sem sinal).
* **Operadores Aritméticos:** Adição (`+`), Subtração (`-`), Multiplicação (`*`), Divisão (`/`) e Módulo (`%`).
* **Operadores Bitwise:** AND (`&`), OR (`|`), XOR (`~`), NOT (`~`), Shift Left (`<<`) e Shift Right (`>>`).
* **Comparações:** Igualdade (`==`), Menor que (`<`), Maior que (`>`), Maior ou igual (`>=`) e Menor ou igual (`<=`).
* **Conversões:** Utilitários para conversão segura entre `int64` e `uint64`.

---

## 💻 Exemplo de Uso

```lua
// Teste da biblioteca num64
local num = num64

imprima('Versao:', num.VER)
poe('////////////////////')

// Operações Aritméticas Básicas
local a = num.int64(2)
local b = num.int64(20)

local c = a + b
imprima('Soma:', c)          // 22

c = b - a
imprima('Subtracao:', c)     // 18

c = a * num.int64(5)
imprima('Multiplicacao:', c) // 10

c = b / num.int64(2)
imprima('Divisao:', c)       // 10

// Instanciação a partir de String (para evitar perda de precisão)
local max_int64 = '9223372036854775807'
local n = num.int64(max_int64)
imprima('Max int64:', tipo(n), n)

// Operações Bitwise e Comparações
local b1 = num.int64(2)
local b2 = num.int64(4)

imprima('Shift Right:', b1 >> b2)
imprima('Shift Left:', b1 << b2)
imprima('Bitwise XOR:', b1 ~ b2)
imprima('Bitwise NOT:', ~b1)

imprima('b1 == b2:', b1 == b2)
imprima('b1 < b2:', b1 < b2)

// Suporte a Unsigned Int64 (uint64)
local max_uint64 = '18446744073709551615'
local u64 = num.uint64(max_uint64)
imprima('Max uint64:', tipo(u64), u64)

// Conversões entre tipos
local conv_i64 = num.conv_int64(u64)
imprima('Convertido para int64:', conv_i64)

local n64bb = num.conv_uint64(conv_i64)
imprima('Revertido para uint64:', n64bb, 'Igual ao original?', n64bb == u64)
```

---

## 👤 Autor & Licença

* **Autor:** Adalberto Amorim Felipe
* **Versão:** 0.1
* **Licença:** Este projeto está sob a licença [MIT](https://opensource.org/licenses/MIT).
