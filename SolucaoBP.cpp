#include <iostream>
#include <fstream>	//for std::ofstream
#include <random>	// for std::mt19937
#include <ctime>	// for std::time
#include <string>
#include <vector>
#include <sstream>

#define MAXV 100		//numero maximo de vertices
//Modifique os valores abaixo para obter um grafo necessario
#define PERCENT 20		//chance de aparecer aresta. diminuir pra grid
#define MIN_NIVEIS 5
#define MAX_NIVEIS 10
#define MIN_POR_NIVEL 5 // Nos/nivel. O 'volume' do grafo.
#define MAX_POR_NIVEL 10

using namespace std;

typedef struct No
{
  int chave;
  bool visita = 0;
  char nivel = 0;
  struct No *next;
} TipoNo;

typedef struct aresta {
    int y;
    struct aresta* prox;
}arst;

typedef struct grafo {
    No* Nos[MAXV + 1];		//informacao de adjacencia
    std::string fragmts[MAXV];		//cada indice(vertice) irá conter uma string de DNA
    int grau[MAXV + 1];			//grau de cada vertice
    int nvertices;				//numero de vertices no grafo
    int nNos;				//num de Nos no grafo
    bool directed;				//A flag indica se o grafo deve ser interpretado como direcionado
}grf;

enum
{
    BASE_A = 0x0, /* binary: 00 */
    BASE_C = 0x1, /* binary: 01 */
    BASE_G = 0x2, /* binary: 10 */
    BASE_T = 0x3, /* binary: 11 */
};

using bit_DNA = std::uint8_t; // cria um alias pra um inteiro nao sinalizado de 8 bits
/*
Cada base: 4 (A,G,T,C)
Cada tipo pode armazenar 4^(n/2)
uint8_t = 256 (4^(8/2)) permutacoes de DNA
uint16_t = 65536 permutacoes
uint32_t = 4.29E9 permts
uint64_t = 1.84E19 permts
*/

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


namespace MyRandom
{
    // Initialize mersenne twister with a random seed based on the clock (once at system startup)
    std::mt19937 mersenne(static_cast<std::mt19937::result_type>(std::time(nullptr)));
}

void inserir(No *cabeca, int novoVaor){
    No *ant = cabeca->next;
    No *pont = cabeca;
    bool naLista=false;
    while(ant!=cabeca){
        if(ant->chave == novoVaor){
            naLista = true;
        }
        ant=ant->next;
        pont = pont->next;
    }
    if(naLista){
        cout<<"Elemento "<<novoVaor<<" encontra-se na lista"<<endl;
        return;
    }
    No *novoNo = new No();
    if (novoNo!=nullptr){
        novoNo->chave = novoVaor;
        novoNo->next = cabeca;
        pont->next = novoNo;
    }
}

void printDireto(No *cabeca){
    cout<<cabeca->chave<<"n"<<(int)cabeca->nivel<<" :";
    No *aux = cabeca->next;
    while (aux!=cabeca){
        cout<<aux->chave<<"n"<<(int)aux->nivel<<"->";
        aux = aux->next;
    }
    cout<<endl;
}

/*void initiaze_graph(grf* g, bool directed) {
    int i;

    g->nvertices = 0;
    g->nNos = 0;
    g->directed = directed;

    for (i = 0; i < MAXV; i++) g->grau[i] = 0;
    for (i = 0; i < MAXV; i++) g->Nos[i] = nullptr;
}

void insert_arest(grafo * g, int x, int y, bool directed) {
    No* p;
    p = new No;

    p->chave = y;
    p->next = g->Nos[x];

    g->Nos[x] = p;

    g->grau[x]++;

    if (directed == false)
        insert_arest(g, y, x, true);
    else
        g->nNos++;
}
*/
//Somente Algumas coisas q são melhores para gerar numeros aleatorios do q rand()e srand()

int getRandomNumber(int min, int max)
{
    std::uniform_int_distribution<> die(min, max); // we can create a distribution in any function that needs it
    return die(MyRandom::mersenne); // and then generate a random number from our global generator
}

int geraDAG() {
    int i, j, k, nos = 0;
    std::ofstream outfg;
    //O arquivo .gv pode ser graficamente exibido no site: webgraphviz.com
    //O arquivo eh gravado em DOT: en.wikipedia.org/wiki/DOT_(graph_description_language)

    outfg.open("C:/Users/lu_fe/Downloads/OneDrive/Materias/Estrutura dados/Trab/trab/graphVisu.txt");

    int niveis = getRandomNumber(MIN_NIVEIS, MAX_NIVEIS);
    outfg << "digraph {\n";
    for (i = 0; i < niveis; i++) {
        int nos_novos = getRandomNumber(MIN_POR_NIVEL, MAX_POR_NIVEL);

        //Os laços indicam que para cada vertice "velho" e "novo"
        //sera decidido se inserir uma aresta entre eles ou não. baseando no PERCENT
        for (j = 0; j < nos; j++) {
            for (k = 0; k < nos_novos; k++) {
                if (getRandomNumber(0, 100) < PERCENT) {
                    outfg << " " << j << " -> " << k + nos << " ;" << std::endl; //Uma aresta
                }
            }
        }

        nos += nos_novos;

    }
    outfg << "}\n";

    outfg.close();

    std::cout << "O seu grafo possui: " << nos << " nos! \n";
    return nos;
}

No** ler_dot(std::ifstream& grafo, int vertices){
    No **lista= new No*[vertices];

    for(int i=0;i<vertices;i++){
        lista[i] = new No();
        lista[i]->chave = i;
        lista[i]->next = lista[i];
    }

    grafo.open("C:/Users/lu_fe/Downloads/OneDrive/Materias/Estrutura dados/Trab/trab/graphVisu.txt");

    if(!grafo)
    {
        cout<<"Não abriu"<<endl;
        exit(1);
    }

    //2
    vector<string> linhas;
    string linha;

    while(getline(grafo,linha))
    {
        linhas.push_back(linha);
    }

    for(int i=1; i<static_cast<int>(linhas.size()-1);i++){
        stringstream tipo(linhas[i]);
        string lixo;
        char lixo2;
        int x,y;
        tipo >> x >> lixo >>y >>lixo2;
        cout<<x<<" "<<y<<endl;
        inserir(lista[x],y);
    }

    grafo.close();
    return lista;
};

void BP(No **lista , int &inicio, int n,bool *flag, bool &continua, int *resp, int &it, int nivel){
    flag[inicio]=1;
    resp[it] = inicio;
    it++;

    No *aux = lista[inicio]->next;
    while(aux!=lista[inicio]){
        int w = aux->chave;
        if(!flag[w]){
            cout<<lista[inicio]->chave<<"-"<<w<<", I"<<endl;

            resp[it]=w;
            it++;

            lista[w]->nivel = lista[inicio]->nivel+1;
            aux->nivel = lista[inicio]->nivel+1;

            flag[w]=1;
            aux->visita=1;

            nivel++;
            BP(lista,w,n,flag,continua,resp, it,nivel);
            nivel--;
        }
        else{
            if(aux->visita == 0){
                cout<<lista[inicio]->chave<<"-"<<w<<", II"<<endl;
                aux->visita = 1;
                aux->nivel = lista[w]->nivel+1  ;
            }
        }
        aux=aux->next;
    }
    continua=false;
    for(int i=0;i<n;i++){
        if(flag[i]==0){
            continua=true;
            inicio=i;
            return;
        }
    }
}

void BPdir(No **lista, int inicio, int n,int *resp){
    bool continua = true;
    bool flag[n];
    int it=0;

    for(int i=0;i<n;i++){
        flag[i] = 0;
    }

    while(continua){
        int nivel = 0;
        BP(lista,inicio,n,flag,continua,resp,it,nivel);
    }
}

void quickSort(int *seq, char *niv, int p, int r){
    if (p<r){
        int temp1;
        char temp;
        int x=niv[r];
        int i=p-1;
        for(int j=p;j<r;j++){
            if(niv[j]<x){
                i++;
                temp1=seq[i];
                seq[i]=seq[j];
                seq[j]=temp1;

                temp=niv[i];
                niv[i]=niv[j];
                niv[j]=temp;
            }
        }
        temp1=seq[i+1];
        seq[i+1]=seq[r];
        seq[r]=temp1;

        temp=niv[i+1];
        niv[i+1]=niv[r];
        niv[r]=temp;
        int q=i+1;

        quickSort(seq,niv,p,q-1);
        quickSort(seq,niv,q+1,r);

    }
}
void criaSequencia(No **lista, int n, int *seq, char *nivel){
    for(int i=0;i<n;i++){
        seq[i]=i;
        nivel[i] = lista[i]->nivel;
    }

    quickSort(seq,nivel,0,n);

}


int main()
{
    cout<<"Gerando grafo de parametros:"<<endl<<"Percent = "<<PERCENT<<endl<<"Min niveis = "<<MIN_NIVEIS
       <<endl<<"Max Niveis = "<<MAX_NIVEIS<<endl<<"Min por nivel = "<<MIN_POR_NIVEL
      <<endl<<"Max por nivel = "<<MAX_POR_NIVEL<<endl;
    int n=geraDAG();
    No **lista;

    ifstream grafo;

    lista = ler_dot(grafo,n);

    cout<<endl<<"Grafo armazenado em lista : "<<endl;
    for(int i=0;i<n;i++){
        printDireto(lista[i]);
    }

    int vertice=0;
    int resp[n];
    char nivel[n];

    BPdir(lista,vertice,n,resp);

    criaSequencia(lista,n,resp,nivel);

    cout<<"Lista Final"<<endl;
    cout<<endl;
    for(int i=0;i<n;i++){
        printDireto(lista[i]);
    }

    cout<<endl<<"Resposta"<<endl;
    for(int i=0;i<n;i++){
        cout<<resp[i]<<" ";
    }
    cout<<endl;

    cout<<"Nivel"<<endl;
    for(int i=0;i<n;i++){
        cout<<(int)nivel[i]<<" ";
    }
    cout<<endl<<"Processo encerrado"<<endl;



    return 0;
}
