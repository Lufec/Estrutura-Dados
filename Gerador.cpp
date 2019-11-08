#include <iostream>
#include <fstream>	//for std::ofstream
#include <random>	// for std::mt19937
#include <ctime>	// for std::time
#include <string>

#define MAXV 1000		//numero maximo de vertices

//Modifique os valores abaixo para obter um grafo necessario

#define PERCENT 100		//chance de aparecer aresta. diminuir pra grid
#define MIN_NIVEIS 2
#define MAX_NIVEIS 2
#define MIN_POR_NIVEL 2 // Nos/nivel. O 'volume' do grafo.
#define MAX_POR_NIVEL 2


typedef struct aresta {
    int y;
    struct aresta* prox;
}arst;

typedef struct grafo {
    aresta* arestas[MAXV + 1];		//informacao de adjacencia
    std::string fragmts[MAXV];		//cada indice(vertice) irá conter uma string de DNA
    int grau[MAXV + 1];			//grau de cada vertice
    int nvertices;				//numero de vertices no grafo
    int narestas;				//num de arestas no grafo
    bool directed;				//A flag indica se o grafo deve ser interpretado como direcionado
}grf;

void initiaze_graph(grf* g, bool directed) {
    int i;

    g->nvertices = 0;
    g->narestas = 0;
    g->directed = directed;

    for (i = 0; i < MAXV; i++) g->grau[i] = 0;
    for (i = 0; i < MAXV; i++) g->arestas[i] = nullptr;
}

void insert_arest(grafo * g, int x, int y, bool directed) {
    aresta* p;
    p = new aresta;

    p->y = y;
    p->prox = g->arestas[x];

    g->arestas[x] = p;

    g->grau[x]++;

    if (directed == false)
        insert_arest(g, y, x, true);
    else
        g->narestas++;
}



//Somente Algumas coisas q são melhores para gerar numeros aleatorios do q rand()e srand()
namespace MyRandom
{
    // Initialize mersenne twister with a random seed based on the clock (once at system startup)
    std::mt19937 mersenne(static_cast<std::mt19937::result_type>(std::time(nullptr)));
}

int getRandomNumber(int min, int max)
{
    std::uniform_int_distribution<> die(min, max); // we can create a distribution in any function that needs it
    return die(MyRandom::mersenne); // and then generate a random number from our global generator
}


int main() {
    int i, j, k, nos = 0;
    std::ofstream outf("--SEU CAMINHO--/graph.gv");
    //O arquivo .gv pode ser graficamente exibido no site: webgraphviz.com
    //O arquivo eh gravado em DOT: en.wikipedia.org/wiki/DOT_(graph_description_language)

    int niveis = getRandomNumber(MIN_NIVEIS, MAX_NIVEIS);

    outf << "digraph {\n";
    for (i = 0; i < niveis; i++) {
        int nos_novos = getRandomNumber(MIN_POR_NIVEL, MAX_POR_NIVEL);

        //Os laços indicam que para cada vertice "velho" e "novo"
        //sera decidido se inserir uma aresta entre eles ou não. baseando no PERCENT
        for (j = 0; j < nos; j++) {
            for (k = 0; k < nos_novos; k++) {
                if (getRandomNumber(0, 100) <= PERCENT) {
                    outf << " " << j << " -> " << k + nos << ";" << std::endl; //Uma aresta
                }
            }
        }

        nos += nos_novos;

    }
    outf << "}\n";
}
