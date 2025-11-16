
#ifndef __strlib_hex_
#define __strlib_hex_
/* macro to `unsign' a character */
#define uchar(c)	((unsigned char)(c))

/* Substitua toda a tabela 'hex_chars' de 256 entradas*/
/* por apenas isto:*/
static const char hex_digits[] = "0123456789abcdef";

static int hex_cod(lua_State*L){
 size_t len;
 const char *orig = luaL_checklstring(L,1,&len);
 luaL_Buffer b;
 char *dest = luaL_buffinitsize(L, &b, len*2);
 size_t i; /* Use size_t para índices e tamanhos*/

 for(i=0; i<len; i++){
    /* O macro uchar() está correto e é uma boa prática!*/
    int c = uchar(orig[i]);

    /* Faça isto (muito mais rápido e eficiente):*/
    dest[i*2]   = hex_digits[(c >> 4) & 0x0F]; /* Pega o nibble de cima*/
    dest[i*2+1] = hex_digits[c & 0x0F];        /* Pega o nibble de baixo*/
 }

 luaL_pushresultsize(&b, len*2);
 lua_pushnumber(L, len*2); /* Retornar o tamanho é útil, boa ideia.*/
 return 2;
}


/* Função 'base16' otimizada*/
static int base16(int c) {
    if (c >= '0' && c <= '9') {
        return c - '0';       /* '0' a '9' -> 0 a 9*/
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;  /* 'a' a 'f' -> 10 a 15*/
    }
    if (c >= 'A' && c <= 'F') { /* <-- Importante!*/
        return c - 'A' + 10;  /* 'A' a 'F' -> 10 a 15*/
    }
    return -1; /* Indica caractere inválido*/
}



static int hex_decod(lua_State*L){
    size_t len=0;
    const char *hex = luaL_checklstring(L, 1, &len);

    /* 1. Verificar tamanho ímpar (BUG FIX)*/
    if (len % 2 != 0) {
        return luaL_error(L, "invalid hex string (odd length)");
    }

    luaL_Buffer b;
    size_t dest_len = len / 2;
    char *dest = luaL_buffinitsize(L, &b, dest_len);
    
    size_t i = 0, cont = 0;
    
    /* 2. Loop mais claro (ESTILO)*/
    for(i=0; i < len; i += 2) {
        int c1 = base16(hex[i]);
        int c2 = base16(hex[i+1]);

        /* 3. Verificar caracteres inválidos (BUG FIX)*/
        if (c1 == -1 || c2 == -1) {
             return luaL_error(L, "invalid hex character in string");
        }
        
        /* (c1 * 16) é o mesmo que (c1 << 4)*/
        dest[cont++] = (c1 << 4) | c2;
    }

    luaL_pushresultsize(&b, dest_len);
    return 1;
}

#endif
