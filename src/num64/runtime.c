#include<stdlib.h>


typedef struct{
    void *u;
}userdata;
 
void pushuserdata(lua_State *L, void *val, const char *mt_name){
    if (!val) {
        lua_pushnil(L);
        return;
    }
    userdata * ud = (userdata*)lua_newuserdata(L, sizeof(userdata));
    ud->u = val;
    
    // Usamos != no C, e passamos o mt_name direto, pois o gerador já manda "userdata<...>"
    if (mt_name && mt_name[0] != '\0') {
        luaL_getmetatable(L, mt_name);
        if(lua_isnoneornil(L,-1)){
            luaL_error (L, "Erro: metatable não encontrada '%s' (Faltou carregar módulo?)\n", mt_name);
        }
        lua_setmetatable(L, -2);
    }
}

// 1. A função de validação de herança
static int prisma_isuserdata(lua_State *L, int arg, const char *mt_name) {
    if (!lua_isuserdata(L, arg)) return 0;
    if (!lua_getmetatable(L, arg)) return 0; 
    
    luaL_getmetatable(L, mt_name); 
    
    while (1) {
        // 1. Checagem por herança direta (Pai/Filho)
        if (lua_rawequal(L, -2, -1)) {
            lua_pop(L, 2); 
            return 1;
        }
        
        // 2. Checagem por interface (Busca a string "userdata<T>" dentro da tabela atual)
        lua_getfield(L, -2, mt_name); 
        
        // Comparar o resultado do getfield [-1] com a metatabela esperada [-2]
        if (lua_rawequal(L, -1, -2)) { 
            lua_pop(L, 3); // Limpa o FieldResult, a ExpectedMeta e a ObjMeta
            return 1;
        }
        lua_pop(L, 1); // Popa apenas o resultado da busca (FieldResult)
        
        // 3. Sobe para o pai
        if (!lua_getmetatable(L, -2)) {
            break; 
        }
        // Agora, o lua_replace(-3) vai substituir a ObjMeta antiga pela ParentMeta
        lua_replace(L, -3); 
    }
    
    lua_pop(L, 2);
    return 0; 
}

// 2. O Extrator Genérico
void *checkuserdata(lua_State *L, int arg, const char *tipo_esperado) {
    // Se o argumento for nulo/nil e permitirmos padrão, retorna NULL direto
    if (lua_isnil(L, arg)) {
        return NULL; 
    }
    
    // Se o gerador exigiu um tipo, faz a checagem rigorosa
    if (tipo_esperado) {
        if (!prisma_isuserdata(L, arg, tipo_esperado)) {
            // Usa o próprio Lua para formatar a mensagem de erro bonita
            luaL_argerror(L, arg, lua_pushfstring(L, "tipo incompativel, esperado: %s ou descendente", tipo_esperado));
        }
    } else {
        // Fallback básico caso queira usar a função sem validação estrita
        if (!lua_isuserdata(L, arg)) luaL_argerror(L, arg, "esperado userdata");
    }
    
    userdata *ud = (userdata*)lua_touserdata(L, arg);
    return ud->u;
}

unsigned char checkuchar(lua_State *L, int idx) {
	unsigned char v_uchar = 0;
	
	// 1. Correção do parêntese e checagem se é string
	if (lua_isstring(L, idx)) {
		const char *ucs = lua_tostring(L, idx);
		
		// Proteção defensiva: garante que a string não é vazia
		if (ucs && ucs[0] != '\0') {
			v_uchar = (unsigned char)ucs[0]; // Captura o primeiro caractere (byte) da string
		} else {
			v_uchar = 0;
		}
	} else {
		// Se não for string, força a validação numérica do Lua
		// Lança um erro automático na tela se o argumento for inválido (Ex: table ou nil)
		v_uchar = (unsigned char)luaL_checkunsigned(L, idx);
	}
	
	return v_uchar;
}

char checkchar(lua_State *L, int idx) {
	char v_char = 0;
	
	if (lua_isstring(L, idx)) {
		const char *ucs = lua_tostring(L, idx);
		
		// Proteção defensiva contra strings vazias
		if (ucs && ucs[0] != '\0') {
			v_char = (char)ucs[0]; // Captura o primeiro byte/caractere
		} else {
			v_char = 0;
		}
	} else {
		// Se for número, aceita inteiros normais e negativos de 8 bits (ex: -128 a 127)
		// Lança erro automático no Lua se for um tipo inválido
		v_char = (char)luaL_checkinteger(L, idx);
	}
	
	return v_char;
}

/*
Macro helper to transverse linked lists that contains 'next' field
Ex.:    

LIST_FOR(GList *, List, item){
    void *data = item->data;
    //do something with data?
    ...
}
*/

#define LIST_FOR(T, OrigList, v) for(T (v) = (OrigList); (v) != NULL; (v) = (v)->next)

//helper function to convert a table.array of strings into c array of strings (const char **arr)
//must free the array after use it (free(arr));
static const char **check_table_to_array_strings(lua_State* L, int idx) {
    int len = 0;
    const char **arr = NULL;
    
    if (!lua_isnoneornil(L, idx)) {
        luaL_checktype(L, idx, LUA_TTABLE); 
        len = lua_rawlen(L, idx);
        
        // Aloca o array de ponteiros
        arr = (const char **)malloc(sizeof(char *) * (len + 1));
        if (!arr) return NULL;
        
        int i = 0;
        for (i = 0; i < len; i++) {
            lua_rawgeti(L, idx, i + 1);
            const char* str_lua = luaL_checkstring(L, -1);
            arr[i] = str_lua;
            
            lua_pop(L, 1);
        }
        arr[i] = NULL; // Terminador NULL obrigatório
    } else {
        return NULL;
    }
    
    return arr;
}


//Helpers callback binding:


typedef struct {
    lua_State *L;
    int fn;
    int data;
    int self;
} PrismaCallbackContext;

PrismaCallbackContext *prisma_callback_context_new(lua_State *L, int idx_fn, int idx_self, int idx_data) {
    // Converte índices relativos (negativos) para absolutos para não quebrar com os push/pops
    int abs_fn   = (idx_fn != 0)   ? lua_absindex(L, idx_fn)   : 0;
    int abs_self = (idx_self != 0) ? lua_absindex(L, idx_self) : 0;
    int abs_data = (idx_data != 0) ? lua_absindex(L, idx_data) : 0;

    if (abs_fn == 0 || !lua_isfunction(L, abs_fn)) {
        luaL_error(L, "callback helper: o argumento da função é obrigatório e deve ser uma função.");
        return NULL;
    }

    PrismaCallbackContext *ctx = (PrismaCallbackContext*)malloc(sizeof(PrismaCallbackContext));
    if (!ctx) return NULL;

    int fn = LUA_NOREF, self = LUA_NOREF, data = LUA_NOREF;

    // Guarda a função
    lua_pushvalue(L, abs_fn);
    fn = luaL_ref(L, LUA_REGISTRYINDEX);

    // Guarda o self (se informado)
    if (abs_self != 0 && !lua_isnil(L, abs_self)) {
        lua_pushvalue(L, abs_self);
        self = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    // Guarda os dados extras (se informado)
    if (abs_data != 0 && !lua_isnil(L, abs_data)) {
        lua_pushvalue(L, abs_data);
        data = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    ctx->L = L;
    ctx->fn = fn;
    ctx->self = self;
    ctx->data = data;
    return ctx;
}

void prisma_callback_context_free(PrismaCallbackContext *ctx) {
    if (!ctx) return;

    if (ctx->fn != LUA_NOREF) {
        luaL_unref(ctx->L, LUA_REGISTRYINDEX, ctx->fn);
        ctx->fn = LUA_NOREF;
    }
    if (ctx->self != LUA_NOREF) {
        luaL_unref(ctx->L, LUA_REGISTRYINDEX, ctx->self);
        ctx->self = LUA_NOREF;
    }
    if (ctx->data != LUA_NOREF) {
        luaL_unref(ctx->L, LUA_REGISTRYINDEX, ctx->data);
        ctx->data = LUA_NOREF;
    }

    free(ctx);

}

#define PRISMA_CALLBACK_CONTEXT_GET_FN(ctx) \
    lua_rawgeti(ctx->L, LUA_REGISTRYINDEX, ctx->fn)

#define PRISMA_CALLBACK_CONTEXT_GET_SELF(ctx) \
    lua_rawgeti(ctx->L, LUA_REGISTRYINDEX, ctx->self)

#define PRISMA_CALLBACK_CONTEXT_GET_DATA(ctx) \
    lua_rawgeti(ctx->L, LUA_REGISTRYINDEX, ctx->data)

#define PRISMA_CALLBACK_CALL(ctx, nargs, nrets) \
    (lua_pcall(ctx->L, nargs, nrets, 0) == 0)

#define PRISMA_CALLBACK_ERROR(ctx, msg) \
    do { \
        fprintf(stderr, "[Prisma Callback Error] %s: %s\n", msg, lua_tostring(ctx->L, -1)); \
        lua_pop(ctx->L, 1); \
    } while(0)


