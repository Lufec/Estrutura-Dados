#include <iostream>
#include <fstream>	//for std::ofstream
#include <random>	// for std::mt19937
#include <ctime>	// for std::time
#include <string>
#include <sstream>
#include <vector>

#define MAXV 100		//numero maximo de vertices
//Modifique os valores abaixo para obter um grafo necessario
#define PERCENT 20		//chance de aparecer aresta. diminuir pra grid
#define MIN_NIVEIS 1
#define MAX_NIVEIS 10
#define MIN_POR_NIVEL 5 // Nos/nivel. O 'volume' do grafo.
#define MAX_POR_NIVEL 10

using namespace std;

typedef struct No
{
  int chave;
  bool visita = 0;
  int nivel = 0;
  struct No *next;
} TipoNo;

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

typedef struct Fila
{
  TipoNo *inicio, *fim;
  int tamanho;
} TipoFila;

namespace MyRandom
{
    // Initialize mersenne twister with a random seed based on the clock (once at system startup)
    std::mt19937 mersenne(static_cast<std::mt19937::result_type>(std::time(nullptr)));
}

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
    cout<<cabeca->chave<<"n"<<cabeca->nivel<<" :";
    No *aux = cabeca->next;
    while (aux!=cabeca){
        cout<<aux->chave<<"n"<<aux->nivel<<" ->";
        aux = aux->next;
    }
    cout<<endl;
}

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

No** iniciar_lista(int vertices){
    No **lista= new No*[vertices];
    for(int i=0;i<vertices;i++){
        lista[i] = new No();
        lista[i]->chave = i;
        lista[i]->next = lista[i];
    }
    return lista;
};

No** ler_dot(std::ifstream &grafo, int vertices){
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

void IniciaFila(TipoFila *fila)
{
  fila->inicio = NULL;
  fila->fim = NULL;
  fila->tamanho=0;
}

int filaVazia(TipoFila *fila)
{
  return (fila->inicio == NULL);
}

void Enfileira(int raiz, TipoFila *fila)
{
  TipoNo *aux;
  aux = (TipoNo *) malloc(sizeof(TipoNo));
  aux->chave=raiz;
  aux->next=NULL;
  if (filaVazia(fila)){
    fila->inicio=aux;
    fila->fim=aux;
  }
  else {
  fila->fim->next = aux;
  fila->fim = aux;
  }
  fila->tamanho++;
}

int Desenfileira(TipoFila *fila)
{
  TipoNo *q; int v;
  if (filaVazia(fila))
  {
    printf("Fila esta vazia\n");
    return -1;
  }
  q = fila->inicio;
  v = fila->inicio->chave;
  fila->inicio = fila->inicio->next;
  free(q);
  fila->tamanho--;
  return v;
}

void BL(No **lista , int &inicio, int n, bool *flag, bool &continua){
    Fila *F = new Fila();
    IniciaFila(F);
    flag[inicio] = 1;
    Enfileira(inicio,F);
    while(!filaVazia(F)){
        int v = Desenfileira(F);
        No *vert = lista[v]->next;
        while(vert!=lista[v]){
            int w=vert->chave;
            if(!flag[w]){
                cout<<lista[v]->chave<<"-"<<w<<", I"<<endl;
                lista[w]->nivel = lista[v]->nivel +1;
                vert->nivel = lista[v]->nivel +1;
                Enfileira(w,F);
                flag[w]=1;
                vert->visita=1;
            }
            else{
                if(vert->visita == 0){
                    cout<<lista[v]->chave<<"-"<<w<<", II"<<endl;
                    lista[w]->nivel = lista[v]->nivel + 1;
                    vert->nivel = lista[v]->nivel +1;
                    vert->visita=1;
               }
            }
            vert=vert->next;
        }
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
void BLdir(No **lista, int inicio, int n){
    bool continua = true;
    bool flag[n];
    for(int i=0;i<n;i++){
        flag[i] = 0;
    }
    while(continua){
        BL(lista,inicio,n,flag,continua);
    }
}
void acertaNivel(No **lista, int n){
    bool continuar = true;
    while(continuar){
        continuar = false;
        for(int i=0;i<n;i++){
            No *aux =lista[i]->next;
            int nivelAtual = lista[i]->nivel;
            while(aux!=lista[i]){
                if(nivelAtual>aux->nivel){
                    aux->nivel = nivelAtual+1;
                    lista[aux->chave]->nivel = nivelAtual + 1;
                    continuar = true;
                }
                aux=aux->next;
            }

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
void criaSequencia(No **lista, int n, int *seq, int *nivel){
    for(int i=0;i<n;i++){
        seq[i]=i;
        nivel[i] = lista[i]->nivel;
    }

    quickSort(seq,nivel,0,n-1);

}

void BP(No **lista , int &inicio, int n,bool *flag, bool &continua, int *resp, int &it){
    flag[inicio]=1;
    No *aux = lista[inicio]->next;
    while(aux!=lista[inicio]){
        int w = aux->chave;
        if(!flag[w]){
            cout<<lista[inicio]->chave<<"-"<<w<<", I"<<endl;
            resp[it]=w;
            it++;
            flag[w]=1;
            aux->visita=1;
            BP(lista,w,n,flag,continua,resp, it);
        }
        else{
            if(aux->visita == 0){
                cout<<lista[inicio]->chave<<"-"<<w<<", II"<<endl;
                aux->visita = 1;
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
    resp[it]=inicio;
    it++;

    for(int i=0;i<n;i++){
        flag[i] = 0;
    }
    while(continua){
        BP(lista,inicio,n,flag,continua,resp,it);
    }
}

int main()
{
    cout<<"Gerando grafo..."<<endl;
    int n=geraDAG();
    No **lista;

    ifstream grafo;

    lista = ler_dot(grafo,n);

    cout<<endl<<"Grafo armazenado em lista : "<<endl;
    for(int i=0;i<n;i++){
        printDireto(lista[i]);
    }

    cout<<endl;
    int vertice=0;
    int resp[n];
    int nivel[n];

    BLdir(lista,vertice,n);
    cout<<endl<<"Apos colocar niveis iniciais"<<endl;
    for(int i=0;i<n;i++){
        printDireto(lista[i]);
    }

    acertaNivel(lista,n);

    cout<<endl<<"Apos acertar os niveis corretos"<<endl;
    for(int i=0;i<n;i++){
        printDireto(lista[i]);
    }

    criaSequencia(lista,n,resp,nivel);

    cout<<endl<<"Sequencia Final"<<endl;
    for(int i=0;i<n;i++){
        cout<<resp[i]<<" ";
    }
    cout<<endl;
    cout<<"Niveis"<<endl;
    for(int i=0;i<n;i++){
        cout<<nivel[i]<<" ";
    }
    return 0;
}
