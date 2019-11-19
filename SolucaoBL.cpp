#include <iostream>
#include <fstream>	//for std::ofstream
#include <random>	// for std::mt19937
#include <ctime>	// for std::time
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>

#define MAXV 65000		//numero maximo de vertices
//Modifique os valores abaixo para obter um grafo necessario
#define PERCENT 5		//chance de aparecer aresta. diminuir pra grid
#define MIN_NIVEIS 100
#define MAX_NIVEIS 100
#define MIN_POR_NIVEL 50 // Nos/nivel. O 'volume' do grafo.
#define MAX_POR_NIVEL 50

using namespace std;
//using namespace std::chrono; // USAR SOMENTE SE O SISTEMA OPERACIONAL NÃO FOR WINDOWS

////////////////Contagem de tempo///////
double PCFreq = 0.0;
__int64 CounterStart = 0;

//Ambas funções abaixo utilizam uma ferramenta do windows chamada Query Performance Frequency,
//Que irá resgatar o tempo de execução de um algoritmo com alta precisão (na ordem de microssegundos)
//ATENÇÃO: Este tipo de função funciona APENAS em Windows devido sua ferramenta específica
//Link de origem do código: https://stackoverflow.com/questions/1739259/how-to-use-queryperformancecounter
void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
    cout << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart)/1000000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}
double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
}
/////////////////////////////


/////////////////Estrutura para criar a lista de adjacência///////////
//A estrutura usada é de uma lista simplesmente encadeada, e as funções utilizadas são as de inserir e mostrar a lista.
//Esses códigos tiveram como base o arquivo provido pelo professor Islame Fernandes da disciplina de Estrutura de Dados
//As funções originais montam uma lista encadeada em ordem crescente, estas usam apenas ordem de chegada.
//Como essa resolução é uma busca em largura, é necessário usar a estrutura de fila.
//Tal qual a lista encadeada, a Fila presente nesse relatório baseou-se nos algoritmos fornecidos pelo
//Professor Islame Fernandes.
typedef struct No
{
  int chave;
  bool visita = 0;
  int nivel = 0;
  struct No *next;
} TipoNo;

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
    cout<<cabeca->chave<<" :";
    No *aux = cabeca->next;
    while (aux!=cabeca){
        cout<<aux->chave<<"->";
        aux = aux->next;
    }
    cout<<endl;
}

typedef struct Fila
{
  TipoNo *inicio, *fim;
  int tamanho;
} TipoFila;

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


////////////////////////////////////////////////////////////////////


///////////////Geração de Números Aleatórios//////////////


//Primeiro: cria um namespace para gerar um embaralhador de números aleatórios
namespace MyRandom
{
    // Inicia um embaralhador mersenne com uma semente aleatória baseada no clock do sistema
    std::mt19937 mersenne(static_cast<std::mt19937::result_type>(std::time(nullptr)));
}


//Essa função irá usar o namespace para gerar um número aleatório entre o mínimo e máximo fornecidos.
int getRandomNumber(int min,int max)
{
    std::uniform_int_distribution<> die(min, max); // Inicia uma distribuição do menor ao maior número
    return die(MyRandom::mersenne); // Usando o namespace definido, embaralha essa distribuição e recupera um número aleatório da mesma
}
/////////////////////////////////////////////////


/////////////////Criação dos Grafos///////////

//Essa Função irá criar um grafo baseado nos parâmetros em DEFINE. Este Grafo irá ser criado em um arquivo .txt
void ArmazenaGrafo(string path,No **lista, int vertices) {
    int i;
    std::ofstream outfg;
    //O arquivo .gv pode ser graficamente exibido no site: webgraphviz.com
    //O arquivo eh gravado em DOT: en.wikipedia.org/wiki/DOT_(graph_description_language)

    outfg.open(path); //Abre ou cria arquivo de texto baseado em path
    outfg << "digraph {\n";  //Texto necessário para gerar o grafo no site acima

    for(i=0;i<vertices;i++){
        No *aux = lista[i]->next;
        while(aux!=lista[i]){
            outfg<<" "<<i<<" -> "<<aux->chave<<" ;"<<endl;
            aux=aux->next;
        }
    }

    outfg << "}\n";

    outfg.close();
    cout<<"Grafo armazenado em "<<path<<endl;
}

int geraGrid(int l, int c, No **lista) {
    for(int i=0;i<c-1;i++){
        inserir(lista[i],i+1);
    }
    for(int i=1;i<l;i++){
        for(int j=0;j<c-1;j++){
            inserir(lista[c*i+j],c*i+j+1);
        }
        for(int j=0;j<c;j++){
            inserir(lista[c*(i-1)+j],c*i+j);
        }
    }

    int nos = l*c;
    return nos;
}

int geraGrafo(No** lista) {
    int i, j, k, nos = 0;
    int niveis = getRandomNumber(MIN_NIVEIS, MAX_NIVEIS);
    for (i = 0; i < niveis; i++) {
        int nos_novos = getRandomNumber(MIN_POR_NIVEL, MAX_POR_NIVEL);
        for (j = 0; j < nos; j++) {
            for (k = 0; k < nos_novos; k++) {
                if (getRandomNumber(0, 100) < PERCENT) {
                    inserir(lista[j],k+nos);
                }
            }
        }
        nos += nos_novos;
    }
    std::cout << "O seu grafo possui: " << nos << " nos! \n";
    return nos;
}

No **iniciaLista(int vertices){
    No **lista= new No*[vertices];
    if(lista == nullptr){
        cout<<"Grafo nao criado"<<endl;
        exit(1);
    }
    for(  int i=0;i<vertices;i++){
        lista[i] = new No();
        if(lista[i] == nullptr){
            cout<<"lista "<<i<<" nao criada"<<endl;
            exit(2);
        }
        lista[i]->chave = i;
        lista[i]->next = lista[i];
    }
    return lista;
}

No** ler_grafo(std::ifstream& grafo,string path,int vertices){

    No **lista = iniciaLista(vertices);

    grafo.open(path);

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
        //cout<<x<<" "<<y<<endl;
        inserir(lista[x],y);
        linhas.erase(linhas.begin());
    }
    grafo.close();
    return lista;
};


////////////////////Gerador de exemplos de DNA//////////////

//Primeiro, enumera-se uma sequencia binária para uma letra específica do genoma
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


//Esta parte converte um número inteiro para um genoma de DNA baseado em seu número binário
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


////////////////Resolução do Problema//////////
//O algoritmo abaixo é uma busca em largura direcionada padrão.
//Todo momento de visita é caracterizado por:
//->Enfileirar todos os vértices ligados ao vértice visitado
//->Colocar o valor de nível do vértice de chegada o valor de nível do de origem +1
//->Marca a visita no vértice de origem
//->Marca a aresta como visitada
void BL(No **lista , int &inicio, int n, bool *flag, bool &continua){
    Fila *F = new Fila();
    IniciaFila(F);
    flag[inicio] = 1;  //Marca vértice inicial
    Enfileira(inicio,F); //Enfileira-o
    while(!filaVazia(F)){
        int v = Desenfileira(F); //Começa a análise do vértice recém desenfileirado
        No *vert = lista[v]->next;
        while(vert!=lista[v]){
            int w=vert->chave; //w é o valor da aresta de chegada
            if(!flag[w]){
                //cout<<lista[v]->chave<<"-"<<w<<", I"<<endl;
                lista[w]->nivel = lista[v]->nivel +1; //Modifica o nível do vértice de chegada
                vert->nivel = lista[v]->nivel +1; //Mesmo procedimento para manter o valor do nível do vértice na aresta
                Enfileira(w,F);
                flag[w]=1;  //marca visita
                vert->visita=1; //marca aresta
            }
            else{
                if(vert->visita == 0){  //se o vértice foi visitado, mas aresta não
                    //cout<<lista[v]->chave<<"-"<<w<<", II"<<endl;
                    lista[w]->nivel = lista[v]->nivel + 1;  //Coloca valor do nível de chegada
                    vert->nivel = lista[v]->nivel +1;       //Mesmo procedimento
                    vert->visita=1;                         //visita
               }
            }
            vert=vert->next;
        }
    }

    //Procedimento abaixo confere se há vértices não visitados
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
    bool continua = true; //condição de parada determinada pela visita a todos os vértices
    bool flag[n];
    for(int i=0;i<n;i++){
        flag[i] = 0;
    }
    while(continua){
        BL(lista,inicio,n,flag,continua);
    }
}

//PROCEDIMENTO 2 -> Corrigir níveis
//Na etapa anterior, as visitas do tipo 2 que ocorriam causavam uma mudança no nível já visitado. É necessário
// corrigir o problema do vértice de origem possuir um nível superior ao vértice de chegada
//Enquanto houver trocas ocorrendo, é preciso varrer o grafo inteiro novamente.
//Será garantido a corretude dos níveis dos vértices quando não houver mudanças de níveis após uma passada completa no grafo
void acertaNivel(No **lista, int n){
    bool continuar = true; //Condição que determina se houve troca de nível no grafo
    while(continuar){
        continuar = false;
        for(int i=0;i<n;i++){ //Visita todos os vértices
            No *aux =lista[i]->next;
            int nivelAtual = lista[i]->nivel;
            while(aux!=lista[i]){ //visita todas as arestas dos vértices
                if(nivelAtual>aux->nivel){ //confere se há niveis incoerentes
                    aux->nivel = nivelAtual+1;
                    lista[aux->chave]->nivel = nivelAtual + 1;
                    continuar = true;
                }
                aux=aux->next;
            }

        }
    }
}

// Algoritmo de Sorting. Escolheu-se o quickSort devido seu  caso médio de complexidade ser o mais baixo
//possivel para algoritmos de comparação (O(nlog(n)));
//Há uma modificação em que ocorre troca em dois vetores dado que a condição de troca é só em um deles.
void quickSort(int *seq, int *niv, int p, int r){
    if (p<=r){
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

/////////PROCEDIMENTO 3
//Uma vez corrigido os níveis do grafo, basta pegar os vértices e seus respectivos níveis e ordená-los com base nos níveis.
void criaSequencia(No **lista, int n, int *seq, int *nivel){
    for(int i=0;i<n;i++){
        seq[i]=i;
        nivel[i] = lista[i]->nivel;
    }

    quickSort(seq,nivel,0,n-1);

}

///////////////////////////

double AlgoritmoLargura(No **lista, int n){
    int vertice=0;
    int resp[n];
    int nivel[n];

    StartCounter();

    //SE O SISTEMA OPERACIONAL NÃO FOR WINDOWS, usar a biblioteca chrono
    //auto begin = std::chrono::high_resolution_clock::now();
    BLdir(lista,vertice,n);

    acertaNivel(lista,n);

    criaSequencia(lista,n,resp,nivel);

    double duration = GetCounter();
    //auto end = std::chrono::high_resolution_clock::now();// Se não estiver usando windows
    //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count(); // Se não estiver usando windows

    cout<<endl<<"Resposta em Numero"<<endl;
    for(int i=0;i<n;i++){
        cout<<resp[i]<<" ";
    }

    cout<<"Nivel em ordem da resposta"<<endl;
    for(int i=0;i<n;i++){
        cout<<nivel[i]<<" ";
    }
    cout<<endl;

    cout<<"Resposta em traco de DNA"<<endl;
    int nivelAux = 0;
    for(  int i=0;i<n;i++){
        if(nivelAux == nivel[i]){
        cout<<print_seg(resp[i])<<" ";
        }
        else{
            cout<<endl<<print_seg(resp[i])<<" ";
        }
        nivelAux = nivel[i];
    }


    return duration;
}


int main()
{
    cout<<"Gerando grafo de parametros:"<<endl<<"Percent = "<<PERCENT<<endl<<"Min niveis = "<<MIN_NIVEIS
        <<endl<<"Max Niveis = "<<MAX_NIVEIS<<endl<<"Min por nivel = "<<MIN_POR_NIVEL
        <<endl<<"Max por nivel = "<<MAX_POR_NIVEL<<endl;
    No **lista;
    string path = "C:/Users/lu_fe/Downloads/Materias/Estrutura dados/Trab/trab/grafo.txt";
    ////////Caso deseje ler um arquivo já criado
    /*
    int n = MAXV // Necessário saber quantos vértices tem neste grafo a ser lido
    ifstream grafo;
    cout<<"Lendo arquivo"<<endl;
    lista = ler_grafo(grafo,path,n);
    cout<<"Arquivo lido, grafo gerado"<<endl;
    */
    //////////////////

    //////Caso deseje criar um grafo baseado nos parâmetros definidos

    lista = iniciaLista(MAXV);
    int n=geraGrafo(lista);
    cout<<"Grafo gerado"<<endl;
    ///////////////

    ////////Caso deseje Grafo Grid,
    /*int l,c;
    cout<<"Insira numero de linhas e colunas do grid"<<endl;
    cin>>l>>c;
    int n = geraGrid(l,c,lista);
    */
    ////////////////

    double execucao = AlgoritmoLargura(lista,n);

    ///////////////

    cout<<endl<<"Processo encerrado"<<endl;
    cout<<"tempo de execucao: "<<execucao<<" microsegundos"<<endl;
    cout<<"Armazenando grafo..."<<endl;
    ArmazenaGrafo(path,lista,n);
    cout<<"Grafo Armazenado!"<<endl;

    return 0;
}
