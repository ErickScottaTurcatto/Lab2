/*typedef struct aresta Aresta;
typedef struct vertices Vertices;
typedef struct grafo Grafo;

struct aresta{
    Vertices *vert;
    struct aresta *prox;
};

struct vertices{
    int id;
    Aresta *aresta;
    struct vertices *prox;
};

struct grafo{
    int n_vertices;
    Vertices *prim;
};

outra meneira:
*/

struct no{
    int vertice;
    struct no *prox;
};
typedef struct no No;

struct grafo{
    int qtd_nos;
    No **lista;
};
typedef struct grafo Grafo;
