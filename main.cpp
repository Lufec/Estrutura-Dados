#include <iostream>
#define MAX 100
using namespace std;

typedef struct No
{
  int chave;
  int visita = 0;
  int nivel = 0;
  struct No *next;
} TipoNo;

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

void busca(No *cabeca, No **ant, No **pont, int x){
    *ant = cabeca; // ant agora aponta pra cabeça e isso se reflete fora da funçao
    *pont = nullptr;
    No *aux = cabeca->next;
    while (aux!=cabeca){
        if (aux->chave < x){
            *ant = aux; // ant agora aponta pra aux e isso se reflete fora da funçao
            aux = aux->next;
        } else{
            if(aux->chave==x){
                *pont = aux;
            }
            aux = cabeca;
        }
    }
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
    cout<<cabeca->chave<<" ,n"<<cabeca->nivel<<" ";
    No *aux = cabeca->next;
    while (aux!=cabeca){
        cout<<aux->chave<<" ,n"<<aux->nivel<<" ";
        aux = aux->next;
    }
    cout<<endl;
}

void remover(No *cabeca, int valor){
    No *ant=nullptr, *pont=nullptr;
    busca(cabeca, &ant, &pont, valor); // ATENCAO:  &ant, &pont
    if (pont!=nullptr){ // o elemento deve estar na lista
        ant->next = pont->next;
        delete pont; // IMPORTANTE NAO ESQUECER
    } else {
        cout<<"Elemento "<<valor<<" não se encontra na lista"<<endl;
    }
}

/*void iniciar(int &n,No **lista){
    cout<<"n"<<endl;
    cin>>n;
    lista= new No*[n];

    for(int i=0;i<n;i++){
        lista[i] = new No();
        lista[i]->chave = i;
        lista[i]->next = lista[i];
    }
    int ent1, ent2;
    cout<<"pra sair, insira 0 em V1"<<endl;
    while(1){
        cout<<"Vertice 1 e 2"<<endl;
        cin>>ent1>>ent2;
        if(ent1 == 0){
            break;
        }
        inserir(lista[ent1-1],ent2);
        inserir(lista[ent2-1],ent1);
    }
    cout<<endl<<"Grafo armazenado em lista : "<<endl;
    for(int i=0;i<n;i++){
        cout<<i+1<<": ";
        printDireto(lista[i]);
    }
}*/


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
    int n;
    No **lista;
  //  iniciar(n,lista);
    cout<<"n"<<endl;
    cin>>n;
    lista= new No*[n];

    for(int i=0;i<n;i++){
        lista[i] = new No();
        lista[i]->chave = i;
        lista[i]->next = lista[i];
    }
    int ent1, ent2;
    cout<<"pra sair, insira -1 em V1 ou V2"<<endl;
    while(1){
        cout<<"Vertice 1 e 2"<<endl;
        cin>>ent1>>ent2;
        if(ent1 == -1 || ent2 == -1){
            break;
        }
        inserir(lista[ent1],ent2);
    }
    cout<<endl<<"Grafo armazenado em lista : "<<endl;
    for(int i=0;i<n;i++){
        cout<<i<<": ";
        printDireto(lista[i]);
    }
    cout<<endl;
    int vertice=0;
    int resp[n];
    int nivel[n];

    BLdir(lista,vertice,n);
    cout<<endl<<"Após colocar níveis iniciais"<<endl;
    for(int i=0;i<n;i++){
        cout<<i<<": ";
        printDireto(lista[i]);
    }

    acertaNivel(lista,n);

    cout<<endl<<"Após acertar os niveis corretos"<<endl;
    for(int i=0;i<n;i++){
        cout<<i<<": ";
        printDireto(lista[i]);
    }

    criaSequencia(lista,n,resp,nivel);

    cout<<endl<<"Sequencia Final"<<endl;
    for(int i=0;i<n;i++){
        cout<<resp[i]<<" ";
    }
    cout<<endl;

    return 0;
}
