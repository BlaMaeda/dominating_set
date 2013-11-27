#include <iostream>
#include <vector>
#include <algorithm>
#include "graph.h"
using namespace std;

bool all (const vector<bool>& v) {
    for (int i = 0; i < v.size(); i++) {
        if (!v[i]) return false;
    }

    return true;
}

bool all (const vector<bool>& v1, const vector<bool>& v2) {
    for (int i = 0; i < v1.size(); i++) {
        if (!v1[i] && !v2[i]) return false;
    }

    return true;
}

void backtrack(Graph& graph, const vector<vector<bool> >& coverable, 
               const vector<int>& vertices, int idx, int no_erased, int &min_erased, vector<bool>& erased, int n) {
    if (all(erased)) {
        if (no_erased < min_erased) {
            min_erased = no_erased;
        }
        return;
    }
    if (idx == n) return;
    
    if (no_erased + 1 >= min_erased) {
        return;
    }

    if (!all(erased, coverable[idx])) {
        return;
    }

    int v = vertices[idx];
    
    vector<bool> new_erased = erased;
    const set<int>& neigh = graph.neighbors(v);
    for (set<int>::iterator s_it = neigh.begin(); s_it != neigh.end(); s_it++) {
        new_erased[*s_it] = true;
    }

    backtrack(graph, coverable, vertices, 
              idx+1, no_erased+1, min_erased, new_erased, n);
    backtrack(graph, coverable, vertices, 
              idx+1, no_erased, min_erased, erased, n);
}

bool comp (const pair<int,int>& p1, const pair<int,int>& p2) {
    return p1.first > p2.first;
}

// Compute the size of the dominating set
// for `graph'.
// It's assumed that `graph' is connected
int dominating_set(Graph& graph) {
    int n = graph.size();

    vector<pair<int, int> > aux(n);
    for (int i = 0; i < n; i++) {
        aux[i].first = graph.neighbors(i).size();
        aux[i].second = i;
    }
    sort(aux.begin(), aux.end(), comp);

    vector<int> vertices(n);
    for (int i = 0; i < n; i++) {
        vertices[i] = aux[i].second;
    }

    vector<vector<bool> > coverable(n, vector<bool>(n, false));
    const set<int>& neigh = graph.neighbors(vertices[n-1]);
    for (set<int>::iterator s_it = neigh.begin(); s_it != neigh.end(); s_it++) {
        coverable[n-1][*s_it] = true;
    }
    for (int i = n-2; i >= 0; i--) {
        coverable[i] = coverable[i+1];
        const set<int>& neigh = graph.neighbors(vertices[i]);
        for (set<int>::iterator s_it = neigh.begin(); s_it != neigh.end(); s_it++) {
            coverable[i][*s_it] = true;
        }
    }

    vector<bool> erased(n, false);
    
    int min_erased = n;
    backtrack(graph, coverable, vertices, 0, 0, min_erased, erased, n);
    return min_erased;
}

// Separate `graph' in its connected components
// and return them as a vector of graphs
vector<Graph> connected_components(Graph graph) {
    int n = graph.size();
    vector<Graph> cc;
    
    // Tengo todos los nodos en un set s
    // Mientras s no este vacio, tomo el primer nodo y lo pongo en un set t
    // Mientras t no este vacio, tomo el primer nodo, lo elimino de s y de t, y agrego en t todos los vecinos que
    // esten en s

    set<int> s = graph.get_nodes();
    while (!s.empty()) {
        Graph g;
        set<int> t;

        t.insert(*(s.begin()));
        while (!t.empty()) {
            int v = *(t.begin());
            s.erase(v);
            t.erase(v);

            g.add_edge(v, v);
            const set<int>& v_neigh = graph.neighbors(v);
            for (set<int>::iterator s_it = v_neigh.begin(); s_it != v_neigh.end(); s_it++) {
                if (s.find(*s_it) == s.end()) continue;
                t.insert(*s_it);
                g.add_edge(v, *s_it);
            }
        }
        g.normalize();
        cc.push_back(g);
    }

    return cc;
}

int main () {
    int no_nodes, no_edges;
    
    while (cin >> no_nodes >> no_edges && no_nodes != 0) {
        Graph graph;

        int n1, n2;
        for (int e = 0; e < no_edges; e++) {
            cin >> n1 >> n2;
            graph.add_edge(n1, n2);
        }

        int k = 0;
        while (no_nodes > graph.size()) {
            while (graph.contains(k)) {
                k++;
            }
            graph.add_edge(k, k);
            k++;
        }

        graph.normalize();

        // Separate in connected components
        vector<Graph> cc = connected_components(graph);

        int dom_set_size = 0;
        for (unsigned int i = 0; i < cc.size(); i++) {
            dom_set_size += dominating_set(cc[i]);
        }

        cout << dom_set_size << endl;
    }

    return 0;
}

