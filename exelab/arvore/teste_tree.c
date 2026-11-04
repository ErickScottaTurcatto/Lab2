#include <stdlib.h>
#include <stdio.h>


struct No{
    int dado;
    struct No *dir;
    struct No *esq;
};
typedef struct No Tree;


Tree *cria_arv(void)
{
    return NULL;
}

Tree *insere(Tree *arv, int dado)
{
    if (arv == NULL) {
        Tree *aux = (Tree *) malloc(sizeof(Tree));
        if (aux == NULL) {
            printf("Erro de alocação\n");
            exit(1);
        }
        aux->dado = dado;
        aux->dir = NULL;
        aux->esq = NULL;
        return aux;
    } else {
        if(arv->dado > dado) {
            arv->esq = insere(arv->esq, dado);
        } else {
            arv->dir = insere(arv->dir, dado);
        }
        return arv;
    }
}

bool a_vazia(Tree *arv)
{
    return arv == NULL;
}


void imprime_crescente(Tree *arv)
{
    if(arv->esq != NULL) {
        imprime_crescente(arv->esq);
    }
    
    printf("%d ", arv->dado);

    if (arv->dir != NULL) {
        imprime_crescente(arv->dir);
    }
}

void imprime(Tree *arv)
{
    if (arv != NULL) {
        printf("%d ", arv->dado);
        imprime(arv->esq);
        imprime(arv->dir);
    }
}

int n_nos(Tree *arv)
{
    if (a_vazia(arv)) return 0;
    return n_nos(arv->esq) + n_nos(arv->dir) +1;
}

int a_altura(Tree *arv)
{
    if (a_vazia(arv)) return -1;

    int e = a_altura(arv->esq);
    int d = a_altura(arv->dir);

    if(e > d) {
        return e + 1;
    } else {
        return d + 1;
    }
}

int soma(Tree *arv)
{
    if (a_vazia(arv)) return 0;
    return arv->dado + soma(arv->esq) + soma(arv->dir);
}

int nos_folhas(Tree *arv)
{
    if (a_vazia(arv)) return 0;
    else if (arv->dir == NULL && arv->esq == NULL) return 1;
    else return nos_folhas(arv->esq) + nos_folhas(arv->dir);
}

int nos_not_folha(Tree *arv)
{
    if (a_vazia(arv)) return 0;
    else if ((arv->dir == NULL && arv->esq == NULL)) return 0;
    else if (arv->dir == NULL && arv->esq != NULL || arv->dir != NULL && arv->esq == NULL) return 1;
    else return nos_not_folha(arv->esq) + nos_not_folha(arv->dir)+1;
}


int maior(Tree *arv)
{
    if (a_vazia(arv)) return 0;
    Tree *p = arv;
    while(p->dir != NULL) {
        p = p->dir;
    }
    return p->dado;
}

bool busca(Tree *arv, int dado)
{
    if (a_vazia(arv)) return false;

    Tree *p = arv;
    while(p != NULL) {
        if (p->dado == dado) return true;

        if(dado > p->dado) {
            p = p->dir;
        }
        else {
            p = p->esq;
        }
    }

    return false;
}

Tree *remover(Tree *arv, int v)
{
    if (arv == NULL) {
        return NULL;
    }

    if(arv->dado == v) {
        if (arv->dir == NULL && arv->esq == NULL){
            free(arv);
            return NULL;
        } else {
            if (arv->dir != NULL && arv->esq != NULL) {
                Tree *aux = arv->esq;
                while(aux->dir != NULL){
                    aux = aux->dir;
                }
                arv->dado = aux->dado;
                arv->esq = remover(arv->esq, aux->dado);
                return arv;
            } else {
                Tree *aux;
                if(arv->dir != NULL) 
                    aux = arv->dir;
                else
                    aux = arv->esq;
                free(arv);
                return aux; 
            }
        }
    }
    else {
        if (arv->dado > v) {
            arv->esq = remover(arv->esq, v);
        }
        else {
            arv->dir = remover(arv->dir, v);
        }
        return arv;
    }
}



int main(){
    Tree *arvore = cria_arv();
    arvore = insere(arvore, 5);
    arvore = insere(arvore, 2);
    arvore = insere(arvore, 10);
    arvore = insere(arvore, 1);
    arvore = insere(arvore, 4);
    arvore = insere(arvore, 9);
    arvore = insere(arvore, 20);

    imprime_crescente(arvore);
    printf("\n");

    imprime(arvore);
    printf("\n");

    printf("Numero de nos: %d\n", n_nos(arvore));
    printf("Altura: %d\n", a_altura(arvore));
    printf("Soma: %d\n", soma(arvore));
    printf("Maior: %d\n", maior(arvore));
    printf("Nos folhas: %d\n", nos_folhas(arvore));
    printf("Nos não folhas: %d\n", nos_not_folha(arvore));
    printf("Existe o valor (1 = sim  0 = nao): %d\n", busca(arvore, 9));
}