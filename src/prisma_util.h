#include <string.h> // Necessário para strdup ou malloc/strcpy
#include <stdlib.h> // Necessário para malloc/free e strdup (em alguns sistemas)


/*troca one char por other char se encotrados em orig na string received.*/



void pris_changeChar( const char *orig, char *received, char one, char other, int max_size) {
    int i;
    for (i = 0; i < max_size - 1 && orig[i] != '\0'; i++) {
        received[i] = orig[i];
        if (received[i] == one) {
            received[i] = other;
        }
    }
    // Garantir que a string de destino seja sempre terminada
    received[i] = '\0';
}


int pris_multi_sz_len( const char *mult_sz) {
    int total_size = 0;
    while (*mult_sz) {
        size_t current_len = strlen(mult_sz);
        total_size += current_len + 1; // Adiciona o tamanho e o '\0'
        mult_sz += current_len + 1;    // Avança para a próxima string
    }
    return total_size + 1; // Adiciona o terminador duplo final
}

/*pega multistrngs do tipo "ola\0mundo\0em\0prisma" e separa os ítens em indices de tabelas prisma*/

/**/

void pris_Push_multistr_to_luatable(lua_State*L, char *ptr , int length)
{
    
    /* 1. Passo de Contagem (Otimização)
          contamos quantas substrings embutidas temos:
     */
    int n_args = 0;
    if (length <= 0) length = pris_multi_sz_len(ptr);
    
    for(int i = 0 ; i < length ; i++) {
        if(ptr[i] == '\0') n_args++;
    }
    
    /* Cria uma tabela já com o tamanho certo*/
    lua_createtable(L, n_args, 0); 
    
    /* ponteiro apontando para string, para aritmética de ponteiros*/
    char *current_ptr = ptr;
    int index = 1; // O índice da tabela: (1, 2, 3...)
    
    while (*current_ptr != '\0') {
        
        /* Calcula o tamanho da substring a partir do ponteiro atual*/
        size_t len = strlen(current_ptr);
        
        /* 1. PUSH: Usa o ponteiro atual (current_ptr) e o tamanho (len)*/
        lua_pushlstring(L, current_ptr, len);
        
        /* 2. SET: TAB[index] = [string no topo] e desempilha a string*/
        lua_rawseti(L, -2, index++);
        
        // 3. Vamos para o início da próxima string
        // (pula a string atual + o terminador '\0')
        current_ptr += len + 1;
        
        // Opcional: Se a multistring terminar com \0\0 e o ponteiro já avançou para o segundo \0
        if (*current_ptr == '\0') break; 
    }
}


/* array of string C to lua table*/

void pris_strarray_to_table(lua_State*L,char **ext){/*push string array into lua table*/
	int cont_ext = 0;
	lua_newtable(L); /*cria uma tabela prisma em C*/
	while ( ext[cont_ext] ){ /*processando cada item da matriz.*/
		lua_pushstring(L, ext[cont_ext] ); /*pega cada string da matriz de string C*/
	    lua_rawseti(L, -2, cont_ext + 1 ); /*+1 porque em prisma o indice comeca em 1 nao em 0;*/
		cont_ext = cont_ext + 1;
	}
	 /*freev(ext); //liberando da memoria a matriz//tem que fazer um laco e liberar cada indice, so necessario se a matriz for retorno de malloc() ou calloc() ou realloc();*/
}



void pris_table_to_strarray(lua_State*L, int index, char **tab, int len) {
	
	if (tab == NULL) {
        return; 
    }
    
    if (lua_istable(L, index)) {
        for (int i = 0; i < len; i++) {
            lua_rawgeti (L, index, i + 1); // Coloca o valor na pilha (-1)
            
            const char* lua_str = luaL_checkstring(L, -1);
            
            tab[i] = strdup(lua_str); 
            
            /*lua_pop(L, 1); // Remove a string da pilha Lua */
        }
    }
    /* OBSERVAÇÃO: O chamador agora tem a responsabilidade de liberar
    cada item do array tab[i] usando free(tab[i]); */
}

void pris_free_items_strarray(char **tab, int len){
	if (tab == NULL) { 
        return; 
    }
    for (int i = 0; i < len; i++){
		free(tab[i]);
	}
	free(tab);
	/*    O chamador deve liberar tab se necessário:
	 *    The caller must free the array:
	 *    pris_free_items_strarray(my_arr);
	 *    free(my_arr);
	 */
}



int prisma_getint(lua_State*L, int index){
	int ret = luaL_checknumber(L,index);
	return ret;
}
