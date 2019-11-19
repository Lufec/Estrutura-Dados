#include <iostream>
#include <fstream>	//for std::ofstream
#include <random>	// for std::mt19937
#include <ctime>	// for std::time
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <windows.h>

using namespace std;

//using namespace std::chrono; // USAR SOMENTE SE O SISTEMA OPERACIONAL NÃO FOR WINDOWS

//////////////////Definição de parâmetros////////////
#define MAXV 100000  		//numero maximo de vertices
//Modifique os valores abaixo para obter um grafo necessario
#define PERCENT 50		//chance de aparecer aresta. diminuir pra grid
#define MIN_NIVEIS 10   //Número mínimo de níveis desejado
#define MAX_NIVEIS 10    //Número máximo de níveis desejado
#define MIN_POR_NIVEL 30// Número mínimo de vértices por nível.
#define MAX_POR_NIVEL 30 //Número máximo de vértices por nível
/////////////////////////////////////


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
typedef struct No
{
  int chave;
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

//Essa Função irá armazenar o grafo criado pelos parâmetros em DEFINE. Ele será armazenado em um arquivo .txt
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

////Algoritmo gerador de Grid
//Procedimento: Cria os nós em uma linha, liga-os da esquerda pra direita
//Repete o procedimento para uma nova linha, une-se a linha de cima com a de baixo para fechar os quadrados
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


////Algoritmo gerador de grafos dados os parametros em DEFINE.
////Grafos gerados por essa função podem ser aleatórios ou não.
//Para não serem, basta igualar os valores de minimos e máximos. Isso garante a quantidade de vértices.
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

////função responsável pela criação e armazenamento da lista de adjacência
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

////Caso deseje ler um grafo já escrito em .txt, basta chamar essa função
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


//esta parte converte um número inteiro para um genoma de DNA baseado em seu número binário
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

///////////////////////Algoritmo de Resolução//////////////////////
//Primeiro: Busca em profundidade
//Mudanças principais: Não há visitas do tipo II (desnecessário para resolver o problema)
//Se todos os vértices forem visitados, a função é encerrada
void BP(No **lista ,   int &inicio,   int n,bool *flag, bool &continua,   int *resp,   int *niv,   int &it,  int nivel){
    flag[inicio]=1;  //vértice inicial é visitado
    resp[it] = inicio; //seu valor é armazenado
    it++;
    if(it == n){ //condicional que confere se todos os vértices foram visitados
        return;
    }
    No *aux = lista[inicio]->next; //Inicia as visitas das arestas do vértice
    while(aux!=lista[inicio]){ //enquanto houver arestas no vértice
        int w = aux->chave;
        if(!flag[w]){          //Se vértice não foi visitado
            //cout<<lista[inicio]->chave<<"->"<<w<<", I"<<endl;
            if(!flag[lista[inicio]->chave]){ // condicional necessária para execução correta do algoritmo
                resp[it]=w;                  // Armazena valor de chegada
                it++;
                if(it == n){                //se todos os vértices foram visitados, retorne
                    return;
                }
            }
            niv[w] = niv[inicio]+1;  //armazena valor do nível do vértice armazenado em um vetor

            flag[w]=1;                //visita vértice

            nivel++;                  //soma mais um em nível para o próximo vértice chamado receber seu valor
            BP(lista,w,n,flag,continua,resp,niv,it,nivel); //chamada iterativa para o próximo vértice
            nivel--;                  //Quando voltar da chamada iterativa, volta o valor original do nível
        }
        aux=aux->next;
    }

    //condição de continuação, caso haja vértices não visitados
    continua=false;
    for(  int i=0;i<n;i++){
        if(flag[i]==0){
            continua=true;
            inicio=i;
            return;
        }
    }
}


////Semelhante à busca em profundidade direcionada padrão, com os acréscimos necessários, como flag e nível
void BPdir(No **lista,   int inicio,   int n,  int *resp,  int *niv){
    bool continua = true;
    bool flag[n];
      int it=0;

    for(  int i=0;i<n;i++){
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
//Algoritmo de ordenação por comparação. Escolheu-se Quicksort devido sua complexidade O(nlog(n))
void quickSort(  int *seq,   int *niv,   int p,  int r){
    if (p<r){
          int temp;
          int x=niv[r];
          int i=p-1;
        for(  int j=p;j<r;j++){
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

//PROCEDIMENTO 2: Ordenação do vetor resposta dado parametros de nível.
//Primeira parte irá gerar um vetor de nivel2 que irá garantir a paridade correta entre a resposta encontrada e o nível do respectivo vértice de mesmo índice
void criaSequencia(  int n,  int *resp,  int *nivel1,  int *nivel2){
    for(  int i=0;i<n;i++){
        nivel2[i] = nivel1[resp[i]];
    }
    quickSort(resp,nivel2,0,n-1);

}

double AlgoritmoProfundidade(No **lista, int n){
    int vertice=0;
    int resp[n];
    int nivel1[n];
    int nivel2[n];
    for(int i=0;i<n;i++){
        nivel1[i]=0;
    }

    cout<<"Executando BP"<<endl;

    StartCounter();
    //SE O SISTEMA OPERACIONAL NÃO FOR WINDOWS
    //auto begin = std::chrono::high_resolution_clock::now();
    BPdir(lista,vertice,n,resp,nivel1);
    criaSequencia(n,resp,nivel1,nivel2);
    double duration = GetCounter();
    //auto end = std::chrono::high_resolution_clock::now();// Se não estiver usando windows
    //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count(); // Se não estiver usando windows
    cout<<endl<<"Resposta em Numero"<<endl;
    for(int i=0;i<n;i++){
        cout<<resp[i]<<" ";
    }

    cout<<"Nivel em ordem da resposta"<<endl;
    for(int i=0;i<n;i++){
        cout<<nivel2[i]<<" ";
    }
    cout<<endl;

    cout<<"Resposta em traco de DNA"<<endl;
    int nivelAux = 0;
    for(  int i=0;i<n;i++){
        if(nivelAux == nivel2[i]){
        cout<<print_seg(resp[i])<<" ";
        }
        else{
            cout<<endl<<print_seg(resp[i])<<" ";
        }
        nivelAux = nivel2[i];
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

    double execucao = AlgoritmoProfundidade(lista,n);

    ///////////////

    cout<<endl<<"Processo encerrado"<<endl;
    cout<<"tempo de execucao: "<<execucao<<" microsegundos"<<endl;
    cout<<"Armazenando grafo..."<<endl;
    ArmazenaGrafo(path,lista,n);
    cout<<"Grafo Armazenado!"<<endl;

    return 0;
}
