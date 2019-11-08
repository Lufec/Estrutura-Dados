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
    No *aux = cabeca->next;
    while (aux!=cabeca){
        cout<<aux->chave<<" ";
        aux = aux->next;
    }
    cout<<endl;
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
    int vertice=0;

    int resp[n];

    BPdir(lista,vertice,n,resp);

    for(int i=0;i<n;i++){
        cout<<resp[i]<<" ";
    }
    cout<<endl;

    return 0;
}
