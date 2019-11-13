#include <iostream>
#include <fstream>	//for std::ofstream
#include <random>	// for std::mt19937
#include <ctime>	// for std::time
#include <string>
#include <vector>
#include <sstream>

#define MAXV 65000		//numero maximo de vertices
//Modifique os valores abaixo para obter um grafo necessario
#define PERCENT 5		//chance de aparecer aresta. diminuir pra grid
#define MIN_NIVEIS 100
#define MAX_NIVEIS 100
#define MIN_POR_NIVEL 10 // Nos/nivel. O 'volume' do grafo.
#define MAX_POR_NIVEL 10

using namespace std;

typedef struct No
{
  int chave;
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
    if(novoNo == nullptr){
        cout<<"No nao criado"<<endl;
        exit(3);
    }
    if (novoNo!=nullptr){
        novoNo->chave = novoVaor;
        novoNo->next = cabeca;
        pont->next = novoNo;
    }
}

void printDireto(No *cabeca){
    cout<<cabeca->chave<<" :";
    No *aux = cabeca->next;
    while (aux!=cabeca){
        cout<<aux->chave<<"->";
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

    outfg.open("C:/Users/lu_fe/Downloads/OneDrive/Materias/Estrutura dados/Trab/trab/graph10000.txt");

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
    if(lista == nullptr){
        cout<<"Grafo nao criado"<<endl;
        exit(1);
    }
    for(int i=0;i<vertices;i++){
        lista[i] = new No();
        if(lista[i] == nullptr){
            cout<<"lista "<<i<<" nao criada"<<endl;
            exit(2);
        }
        lista[i]->chave = i;
        lista[i]->next = lista[i];
    }

    grafo.open("C:/Users/lu_fe/Downloads/OneDrive/Materias/Estrutura dados/Trab/trab/graph10000.txt");

    if(!grafo)
    {
        cout<<"Não abriu"<<endl;
        exit(1);
    }

    //2
    vector<string> linhas;
    string linha;
    getline(grafo,linha);
    while(getline(grafo,linha))
    {
        linhas.push_back(linha);
        stringstream tipo(linhas[0]);
        string lixo;
        char lixo2;
        int x,y;
        tipo >> x >> lixo >>y >>lixo2;
        cout<<x<<" "<<y<<endl;
        inserir(lista[x],y);
        linhas.erase(linhas.begin());
    }
   /* linhas.erase(linhas.begin());
    for(int i=0; i<static_cast<int>(linhas.size()-1);i++){
        stringstream tipo(linhas[i]);
        string lixo;
        char lixo2;
        int x,y;
        tipo >> x >> lixo >>y >>lixo2;
        cout<<x<<" "<<y<<endl;
        inserir(lista[x],y);
        linhas.erase(linhas.begin());
    }
*/
    grafo.close();
    return lista;
};

void BP(No **lista , int &inicio, int n,bool *flag, bool &continua, int *resp, int *niv, int &it, int nivel){
    flag[inicio]=1;
    resp[it] = inicio;
    it++;
    if(it == n){
        return;
    }
    No *aux = lista[inicio]->next;
    while(aux!=lista[inicio]){
        int w = aux->chave;
        if(!flag[w]){
            cout<<lista[inicio]->chave<<"->"<<w<<", I"<<endl;
            if(!flag[lista[inicio]->chave]){
                resp[it]=w;
                it++;
                if(it == n){
                    return;
                }
            }
            niv[w] = niv[inicio]+1;

            flag[w]=1;

            nivel++;
            BP(lista,w,n,flag,continua,resp,niv,it,nivel);
            nivel--;
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

void BPdir(No **lista, int inicio, int n,int *resp,int *niv){
    bool continua = true;
    bool flag[n];
    int it=0;

    for(int i=0;i<n;i++){
        flag[i] = 0;
    }

    while(continua){
        int nivel = 0;
        BP(lista,inicio,n,flag,continua,resp,niv,it,nivel);
        if(it == n){
            return;
        }
    }
}

void quickSort(int *seq, int *niv, int p, int r){
    if (p<r){
        int temp;
        int x=niv[r];
        int i=p-1;
        for(int j=p;j<r;j++){
            if(niv[j]<x){
                i++;
                temp=seq[i];
                seq[i]=seq[j];
                seq[j]=temp;

                temp=niv[i];
                niv[i]=niv[j];
                niv[j]=temp;
            }
        }
        temp=seq[i+1];
        seq[i+1]=seq[r];
        seq[r]=temp;

        temp=niv[i+1];
        niv[i+1]=niv[r];
        niv[r]=temp;
        int q=i+1;

        quickSort(seq,niv,p,q-1);
        quickSort(seq,niv,q+1,r);

    }
}
void criaSequencia(int n, int *resp, int *nivel1, int *nivel2){
    for(int i=0;i<n;i++){
        nivel2[i] = nivel1[resp[i]];
    }
    cout<<"Criou nivel"<<endl;
    //quickSort(resp,nivel2,0,n-1);

}

int main()
{
    cout<<"Gerando grafo de parametros:"<<endl<<"Percent = "<<PERCENT<<endl<<"Min niveis = "<<MIN_NIVEIS
       <<endl<<"Max Niveis = "<<MAX_NIVEIS<<endl<<"Min por nivel = "<<MIN_POR_NIVEL
      <<endl<<"Max por nivel = "<<MAX_POR_NIVEL<<endl;
    int n=geraDAG();
    //int n=10000;
    No **lista;

    ifstream grafo;
    cout<<"Lendo arquivo"<<endl;
    lista = ler_dot(grafo,n);

   /* cout<<endl<<"Grafo armazenado em lista : "<<endl;
    for(int i=0;i<n;i++){
        printDireto(lista[i]);
    }*/

    int vertice=0;
    int resp[n];
    int nivel1[n];
    int nivel2[n];
    for(int i=0;i<n;i++){
        nivel1[i]=0;
    }

    cout<<"Executando BP"<<endl;
    BPdir(lista,vertice,n,resp,nivel1);

    //cout<<"Pos BP"<<endl;
    /*for(int i=0;i<n;i++){
        printDireto(lista[i]);
    }*/
    cout<<endl;
    cout<<"Executando criacao de sequencia"<<endl;
    criaSequencia(n,resp,nivel1,nivel2);

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

    cout<<"Nivel 1 a n"<<endl;
    for(int i=0;i<n;i++){
        cout<<nivel1[i]<<" ";
    }
    cout<<"Nivel ordem da resposta"<<endl;
    for(int i=0;i<n;i++){
        cout<<nivel2[i]<<" ";
    }
    cout<<endl<<"Processo encerrado"<<endl;



    return 0;
}
