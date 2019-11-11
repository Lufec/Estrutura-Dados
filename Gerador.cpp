
#include <iostream>
#include <fstream>	//for std::ofstream
#include <random>	// for std::mt19937
#include <ctime>	// for std::time
#include <cstdint>

//Modifique os valores abaixo para obter um grafo necessario

#define PERCENT 100		//chance de aparecer aresta. diminuir pra grid
#define MIN_NIVEIS 2
#define MAX_NIVEIS 5
#define MIN_POR_NIVEL 2 // Nos/nivel. O 'volume' do grafo.
#define MAX_POR_NIVEL 7

//Para a traducao das cadeias de DNA:
enum
{
	BASE_A = 0x0, /* binary: 00 */
	BASE_C = 0x1, /* binary: 01 */
	BASE_G = 0x2, /* binary: 10 */
	BASE_T = 0x3, /* binary: 11 */
};


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

//#define MAXV 1000		//numero maximo de vertices

using bit_DNA = std::uint8_t; // cria um alias pra um inteiro nao sinalizado de 8 bits
/*
Cada base: 4 (A,G,T,C)
Cada tipo pode armazenar 4^(n/2)
uint8_t = 256 (4^(8/2)) permutacoes de DNA
uint16_t = 65536 permutacoes
uint32_t = 4.29E9 permts
uint64_t = 1.84E19 permts 


*/

typedef struct aresta {
	int y;
	struct aresta* prox;
}arst;

typedef struct grafo {
	aresta** arestas;	//informacao de adjacencia
	//bit_DNA fragmts[MAXV];
	int *grau;			//grau de cada vertice
	int nvertices;				//numero de vertices no grafo
	int narestas;				//num de arestas no grafo
	bool directed;				//A flag indica se o grafo deve ser interpretado como direcionado
}grf;

void initialize_graph(grf* g, bool directed, int MAXV) {
	int i;
	g->arestas = new aresta*[MAXV];
	g->grau = new int[MAXV];

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




//O algoritimo abaixo convertera os inteiros armazenados na lista sequencial 
//em bases do DNA( A,G,T,C), em seguida os retorna.
//ele recebe como parametro um inteiro de cu
//Exemplo: len = 4 : ATTG
//		   len = 8 : ATGTGTAA
//		   len = 16 : GTCCTCAAAATTTTAC
std::string print_seg(bit_DNA seg) {

	int len = (sizeof(bit_DNA)*8)/2;

		std::string word("");

		for (int i = 0; i < len; i++) {
			bit_DNA w = seg >> 2 * i;

			switch (w & 0x03) {
			case BASE_A:
				word += "A";
				break;
			case BASE_C:
				word += "C";
				break;
			case BASE_T:
				word += "T";
				break;
			case BASE_G:
				word += "G";
				break;
			default:
				word += "f";
				break;
			}
		}
		//std::cout << word << std::endl;
		return word;

}

/*
Gera um Grafo direxcionado aciclico e o grava em um arquivo no seu desktop,
o arquivo pode ser visualizado pela ferramenta online indicada.
o algoritimo eh facilmente adaptado para gerar uma loista de adajacencia.
Basta coloicar a funcao inserir_aresta(j,k+nos) dentro do "if".
Obs. Consulte os parametros da funcao inserir_aresta() 
*/
void geraDAG() {
	int i, j, k, nos = 0;
	std::ofstream outf("C:/Users/Reyne/Desktop/graph.txt");
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
				if (getRandomNumber(0, 100) < PERCENT) {
					outf << " " << j << " -> " << k + nos << ";" << std::endl; //Uma aresta
				}
			}
		}

		nos += nos_novos;

	}
	outf << "}\n";
	std::cout << "O seu grafo possui: " << nos << " nos! \n";
}

int main() {
	grf* g = new grafo;

	initialize_graph(g, true, 1000); // recebe como parametro o grafo, se ele é direcionado e o numero de vertices eh 1000
	
	/*inserir arestas no grafo g, 
	de 1 -> 254
	1->200
	1->25
	1->88
	*/
	insert_arest(g, 1, 254, true);
	insert_arest(g, 1, 200, true);
	insert_arest(g, 1, 25, true);
	insert_arest(g, 1, 88, true);

	aresta* temp = g->arestas[1];
	
	//imprime todos os vertices conectados a 1:
	while (temp) {
		std::cout << temp->y<<" ";
		temp = temp->prox;
	}

	std::cout <<"\nTraduzindo para segmentos de DNA: "<< std::endl;
	temp = g->arestas[1];
	while (temp) {
		std::cout << print_seg(temp->y) << " "; //utilizando a funcao print_seg() é possivel "traduzir" o vertice em bases de DNA
		temp = temp->prox;
	}
	std::cout << "\n";
}
