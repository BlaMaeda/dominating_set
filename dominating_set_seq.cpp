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
    set<int> nodes = graph.get_nodes();

    vector<pair<int, int> > aux(n);
    for (set<int>::iterator s_it = nodes.begin(); s_it != nodes.end(); s_it++) {
        aux[i].first = graph.neighbors(*s_it).size();
        aux[i].second = *s_it;
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

    //cc.push_back(graph); return cc; //XXX

    set<int> not_seen = graph.get_nodes();

    while (!not_seen.empty()) {
        int v;
        set<int> g_nodes;
        set<int> added_nodes;
        added_nodes.insert(*(not_seen.begin()));
        while (!added_nodes.empty()) {
            v = *(added_nodes.begin());
            g_nodes.insert(v);
            not_seen.erase(v);
            added_nodes.erase(v);
            
            const set<int>& neigh = graph.neighbors(v);
            for (set<int>::iterator s_it = neigh.begin(); s_it != neigh.end(); s_it++) {
                if (not_seen.find(*s_it) != not_seen.end()) {
                    added_nodes.insert(*s_it);
                }
            }
        }

        cc.push_back(graph.sub_graph(g_nodes));
    }

    return cc;
}

int main () {
    int no_cases, no_nodes, no_edges;
    
    cin >> no_cases;

    for (int i_case = 1; i_case <= no_cases; i_case++) {
        int no_nodes, no_edges;
        cin >> no_nodes >> no_edges;

        Graph graph(no_nodes);

        int n1, n2;
        for (int e = 0; e < no_edges; e++) {
            cin >> n1 >> n2;
            if (n1 != n2) graph.add_edge(n1, n2);
        }

        graph.print();

        // Separate in connected components
        vector<Graph> cc = connected_components(graph);

        int dom_set_size = 0;
        for (unsigned int i = 0; i < cc.size(); i++) {
            //dom_set_size += dominating_set(cc[i]);
            cc[i].print();
        }

        cout << dom_set_size << endl;
    }

    return 0;
}

