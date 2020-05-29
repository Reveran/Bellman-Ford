#include <sstream>
#include <iostream>
#include <vector>
#include "raylib.h"
#include <cmath>

using namespace std;

class Node;

class Edge{
    private:
        Node* A;
        Node* B;
        float distancia;
        Edge* siguiente = NULL;
        Color color = GRAY;
        friend class Grafo;
    public:
        Edge(Node*, Node*, float);
        ~Edge();
};

Edge::Edge(Node* _A, Node* _B, float _distancia){
    A = _A;
    B = _B;
    distancia = _distancia;
    siguiente = NULL;
}

Edge::~Edge(){
}

class Node {
    private:
        string nombre;
        int x, y;
        Node* siguiente = NULL;
        friend class Grafo;
        bool inPath = false;
        Color color = { 150, 150, 150, 200 };
        vector<Edge*> Aris;
    public:
        Node(string, int, int);
        ~Node();
};

Node::Node(string _nombre, int _x, int _y){
    nombre = _nombre;
    x = _x;
    y = _y;
}


Node::~Node(){
};


class Grafo{
    private:
        int nodes = 0;
        int edges = 0;
        int origen;
        int destino;
        vector<Node*> Nodos;
        vector<Edge*> Arist;
        vector<int> shortP;
        vector<vector<float>> current;
        float distance = 0;
    public:
        Grafo(string,vector<Vector2>&);
        void draw();
        void update();
        void BellmanFord();
        void calcShort();
        ~Grafo();
};

Grafo::Grafo(string cadena, vector<Vector2> &aristas){
    string a; 
    stringstream check1(cadena);

    while (getline(check1, a, ',')){
        Nodos.push_back(new Node("N" + to_string(nodes), (atoi)(strtok((char*)a.c_str(), " ")), (atoi)(strtok(NULL, " "))));
        nodes += 1;
    }

    Edge* e;
    for (auto ar : aristas){
        e = new Edge(Nodos[ar.x], Nodos[ar.y], sqrt( pow(Nodos[ar.x]->x - Nodos[ar.y]->x, 2) + pow(Nodos[ar.x]->y - Nodos[ar.y]->y, 2) ));
        Nodos[ar.x]->Aris.push_back(e);
        Nodos[ar.y]->Aris.push_back(e);
        Arist.push_back(e);
        edges += 1;
        e = NULL;
    }

    origen = GetRandomValue(0,nodes);
    destino = 1;
}

void Grafo::draw(){
    float scale = 0.81;

    for (auto c : Arist){
        DrawLineEx({scale*c->A->x, scale*c->A->y},{scale*c->B->x, scale*c->B->y}, 5, c->color);
        c = c->siguiente;
    }
    for (auto b : Nodos){
        DrawCircle(b->x*scale, b->y*scale, 10, b->color);
        DrawText(b->nombre.c_str(), b->x*scale - 7, b->y*scale - 5, 10, BLACK);
        b = b->siguiente;
    }
    DrawText((char*)("N" + to_string(origen) + " - N" + to_string(destino) + "\n" + to_string(distance) + " Pixels").c_str(), 610, 40, 15, BLACK);
}

void Grafo::BellmanFord(){
    vector<vector<vector<float>>> mtx;

    int i = 0;
    bool inB = false;

    mtx = {{},{}};

    for (auto a : Nodos){   // Primera iteracion
        mtx[0].push_back({(float)origen,999999});
        mtx[1].push_back({(float)origen,999999});
        for (auto e : a->Aris){
            if (e->A == Nodos[origen]){
                if (a == e->A){
                    mtx[0][i][1] = 0;
                } else {
                    mtx[0][i][1] = e->distancia;
                }
            } else if (e->B == Nodos[origen]){
                if (a == e->B){
                    mtx[0][i][1] = 0;
                } else {
                    mtx[0][i][1] = e->distancia;
                }
            }
        }
        i += 1;
    }

   
    int o;
    while (true){           // Demas iteraciones
        i = 0;
        for (auto n : Nodos){
            for (auto e : n->Aris){
                if (mtx[0][i][1] < mtx[1][i][1]){
                    mtx[1][i][1] = mtx[0][i][1];
                }
                if (e->B == n){
                    o = (stoi)(e->A->nombre.substr(1,3));
                    if ((mtx[0][o][1] + e->distancia) < mtx[0][i][1]){
                        mtx[1][i][0] = o;
                        mtx[1][i][1] = (mtx[0][o][1] + e->distancia);
                    }
                }
                if (e->A == n){
                    o = (stoi)(e->B->nombre.substr(1,3));
                    if ((mtx[0][o][1] + e->distancia) < mtx[0][i][1]){
                        mtx[1][i][0] = o;
                        mtx[1][i][1] = (mtx[0][o][1] + e->distancia);
                    }
                }
                
            }
            i += 1;
        }
        if (mtx[0] != mtx[1]){
            mtx[0] = mtx[1];
        } else {
            break;        
        }
        
    }
    current = mtx[0];
    calcShort();
}

void Grafo::calcShort(){
    shortP.clear();
    for (auto m : Nodos){
        m->inPath = false;
    }
    int i = destino;
    while (i != origen){    // Obtencion de la ruta mas corta
        shortP.push_back(i);
        i = current[i][0];
    }
    shortP.push_back(origen);
    distance = current[destino][1];
}

void Grafo::update(){

    for (int n : shortP){
        Nodos[n]->inPath = true;
    }
    

    for (auto e : Arist){
        if (e->A->inPath && e->B->inPath){
            e->color = ORANGE;
        } else {
            e->color = GRAY;
        }
        //DrawRectangleRec({(float)0.81*(e->A->x + e->B->x)/2 - 8, (float)0.81*(e->A->y + e->B->y)/2 - 8, 16, 16},RED);
    }

    for (auto n : Nodos){
        if (CheckCollisionPointCircle(GetMousePosition(),{(float)0.81*n->x, (float)0.81*n->y},10)){
            if (IsMouseButtonReleased(0)){
                destino = (stoi)(n->nombre.substr(1,3));
                calcShort();
                n->color = ORANGE;
            } else if (IsMouseButtonReleased(1)){
                origen = (stoi)(n->nombre.substr(1,3));
                BellmanFord();
                n->color = ORANGE;
            }
            n->color.a = 200;
        } else if (n->inPath){
            n->color = ORANGE;
        } else {
            n->color = GRAY;
        }
    }
}
Grafo::~Grafo(){
}
