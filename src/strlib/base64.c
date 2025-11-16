#ifndef BASE64_H
#define BASE64_H

#include <stdlib.h> /* Para size_t*/

/**
 * @brief Codifica dados binários para uma string Base64.
 *
 * A string de saída é alocada dinamicamente (malloc) e deve ser
 * liberada (free) pelo chamador.
 *
 * @param data Ponteiro para os dados de entrada.
 * @param input_length O número de bytes dos dados de entrada.
 * @param output_length Ponteiro para uma variável (size_t) onde o
 * comprimento da string de saída (sem o '\0')
 * será escrito.
 * @return Um ponteiro para a string Base64 terminada em nulo,
 * ou NULL em caso de erro (ex: falha de alocação).
 */
char* base64_encode(const unsigned char* data,
                    size_t input_length,
                    size_t* output_length);

/**
 * @brief Decodifica uma string Base64 para dados binários.
 *
 * O buffer de saída é alocado dinamicamente (malloc) e deve ser
 * liberado (free) pelo chamador.
 *
 * @param data Ponteiro para a string Base64 de entrada.
 * @param input_length O comprimento da string de entrada.
 * @param output_length Ponteiro para uma variável (size_t) onde o
 * número de bytes decodificados será escrito.
 * @return Um ponteiro para os dados binários decodificados,
 * ou NULL em caso de erro (ex: entrada malformada,
 * caractere inválido, falha de alocação).
 */
unsigned char* base64_decode(const char* data,
                             size_t input_length,
                             size_t* output_length);


/*  IMPLEMENTAÇÃO BINDING PRISMA  */

static int pbase64_encode(lua_State *L){
	size_t in_len = 0, out_len = 0;
	const char *data = luaL_checklstring(L, 1, &in_len);
	char *b64 = base64_encode((const unsigned char*)data, in_len, &out_len);
	if(!b64){
		lua_pushnil(L);
		lua_pushstring(L, "Memória insuficiente.");
		return 2;
	}
	lua_pushlstring(L, b64, out_len);
	free(b64);
	lua_pushunsigned(L, out_len);
	return 2;
}


static int pbase64_decode(lua_State *L){
	size_t in_len = 0, out_len = 0;
	const char *data = luaL_checklstring(L, 1, &in_len);
	unsigned char *b64_dec = base64_decode(data, in_len, &out_len);
	if(!b64_dec){
		lua_pushnil(L);
		lua_pushstring(L, "Falha ao decodificar (memória ou entrada inválida).");
		return 2;
	}
	lua_pushlstring(L, (const char*) b64_dec, out_len);
	free(b64_dec);
	lua_pushunsigned(L, out_len);
	return 2;
}


/*==================  implementação ====================*/


#include <stdint.h> /* Para uint32_t (C99)*/
#include <stdlib.h> /* Para malloc, free*/
#include <ctype.h>  /* Para isspace*/

/* Tabela de codificação (Portátil e O(1))*/
static const char b64_code[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Tabela de decodificação (O(1) lookup, 1KB de custo, essencial)*/
static const char b64_unmap[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, /* + /*/
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 0-9*/
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* A-O*/
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* P-Z*/
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* a-o*/
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, /* p-z*/
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/* ==================================================================
 * ENCODER
 * ================================================================== */
char* base64_encode(const unsigned char* data,
                    size_t input_length,
                    size_t* output_length) {
    
    /* 1. Calcular tamanho da saída e alocar memória*/
    /* (input_length + 2) / 3 -> número de blocos de 3 bytes (arredondado p/ cima)*/
    /* ... * 4 -> cada bloco vira 4 caracteres*/
    size_t out_len = ((input_length + 2) / 3) * 4;
    char* output = malloc(out_len + 1); /* +1 para o '\0'*/
    
    if (output == NULL) {
        if (output_length) *output_length = 0;
        return NULL;
    }

    size_t i = 0; /* Índice de entrada*/
    size_t j = 0; /* Índice de saída*/

    /* 2. Processar blocos completos de 3 bytes*/
    while (i < input_length - 2) {
        /* Empacota 3 bytes (24 bits) em um uint32_t*/
        uint32_t c1 = data[i++];
        uint32_t c2 = data[i++];
        uint32_t c3 = data[i++];
        uint32_t tuple = (c1 << 16) | (c2 << 8) | c3;

        /* Desempacota 24 bits em 4 grupos de 6 bits*/
        output[j++] = b64_code[(tuple >> 18) & 0x3F];
        output[j++] = b64_code[(tuple >> 12) & 0x3F];
        output[j++] = b64_code[(tuple >> 6)  & 0x3F];
        output[j++] = b64_code[tuple & 0x3F];
    }

    /* 3. Processar o "rabo" (padding)*/
    int remainder = input_length - i;
    if (remainder == 1) { /* 1 byte restante*/
        uint32_t c1 = data[i];
        uint32_t tuple = (c1 << 16); /* c2 e c3 são 0*/
        
        output[j++] = b64_code[(tuple >> 18) & 0x3F];
        output[j++] = b64_code[(tuple >> 12) & 0x3F];
        output[j++] = '=';
        output[j++] = '=';

    } else if (remainder == 2) { /* 2 bytes restantes*/
        uint32_t c1 = data[i];
        uint32_t c2 = data[i + 1];
        uint32_t tuple = (c1 << 16) | (c2 << 8); /* c3 é 0*/
        
        output[j++] = b64_code[(tuple >> 18) & 0x3F];
        output[j++] = b64_code[(tuple >> 12) & 0x3F];
        output[j++] = b64_code[(tuple >> 6)  & 0x3F];
        output[j++] = '=';
    }

    /* 4. Finalizar a string*/
    output[j] = '\0';
    if (output_length) *output_length = out_len;
    return output;
}


/* ==================================================================
 * DECODER
 * ================================================================== */
unsigned char* base64_decode(const char* data,
                             size_t input_length,
                             size_t* output_length) {

    /* 1. Alocar memória (estimativa de pior caso)*/
    /* A saída será *no máximo* 3/4 do tamanho da entrada.*/
    /* +2 é uma pequena margem de segurança.*/
    size_t out_len_estimate = (input_length * 3 / 4) + 2;
    unsigned char* output = malloc(out_len_estimate);

    if (output == NULL) {
        if (output_length) *output_length = 0;
        return NULL;
    }

    size_t i = 0; /* Índice de entrada*/
    size_t j = 0; /* Índice de saída*/
    
    /* Loop principal: lê 4 caracteres válidos e escreve 3 bytes*/
    while (i < input_length) {
        uint32_t val[4];
        int n = 0; /* Número de caracteres válidos lidos (0 a 4)*/

        /* 2. Ler 4 caracteres válidos (pulando whitespace)*/
        while (n < 4 && i < input_length) {
            unsigned char c = data[i++];

            if (isspace(c)) continue; /* Pular (MIME)*/

            if (c == '=') {
                val[n++] = 64; /* 64 é nosso marcador interno de padding*/
            } else {
                int v = b64_unmap[c];
                if (v == -1) { /* Caractere inválido!*/
                    free(output);
                    if (output_length) *output_length = 0;
                    return NULL;
                }
                val[n++] = v;
            }
        }
        
        /* Se paramos antes de ler 4, pode ser o fim da string*/
        if (n == 0) break; /* String terminou limpa*/
        if (n != 4) { /* String terminou com bloco incompleto (ex: "ABC")*/
             free(output);
             if (output_length) *output_length = 0;
             return NULL;
        }

        /* 3. Processar o bloco de 4 caracteres*/
        
        /* "AAAA" (bloco normal)*/
        if (val[0] < 64 && val[1] < 64 && val[2] < 64 && val[3] < 64) {
            uint32_t tuple = (val[0] << 18) | (val[1] << 12) | (val[2] << 6) | val[3];
            output[j++] = (tuple >> 16) & 0xFF;
            output[j++] = (tuple >> 8) & 0xFF;
            output[j++] = tuple & 0xFF;
        
        /* "AAA=" (fim, 2 bytes)*/
        } else if (val[0] < 64 && val[1] < 64 && val[2] < 64 && val[3] == 64) {
            uint32_t tuple = (val[0] << 18) | (val[1] << 12) | (val[2] << 6);
            output[j++] = (tuple >> 16) & 0xFF;
            output[j++] = (tuple >> 8) & 0xFF;
            break; /* Fim da string*/

        /* "AA==" (fim, 1 byte)*/
        } else if (val[0] < 64 && val[1] < 64 && val[2] == 64 && val[3] == 64) {
            uint32_t tuple = (val[0] << 18) | (val[1] << 12);
            output[j++] = (tuple >> 16) & 0xFF;
            break; /* Fim da string*/

        /* Inválido (ex: "A=AA", "A==A", "A=B=")*/
        } else {
            free(output);
            if (output_length) *output_length = 0;
            return NULL;
        }
    }

    /* 4. Finalizar e retornar*/
    if (output_length) *output_length = j;
    
    /* Opcional: `realloc(output, j)` para encolher o buffer.*/
    /* Mas não é necessário, o chamador tem o tamanho `j`.*/
    
    return output;
}



#endif /* BASE64_H*/
