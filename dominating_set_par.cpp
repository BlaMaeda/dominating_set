#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <omp.h>
#include "graph.h"
using namespace std;

/// Check if all components of v are true
bool all (const vector<bool>& v) {
    int n = v.size();
    for (int i = 0; i < n; i++) {
        if (!v[i]) return false;
    }

    return true;
}

/// Check if v1[i] || v2[i] is true for all i
bool all (const vector<bool>& v1, const vector<bool>& v2) {
    int n = v1.size();
    for (int i = 0; i < n; i++) {
        if (!v1[i] && !v2[i]) return false;
    }

    return true;
}

int str2num (const char* s) {
    int n = strlen(s);
    int num = 0;
    int ten = 1;
    for (int i = n-1; i >= 0; i--) {
        int digit = static_cast<int>(s[i] - '0');
        if (digit < 0 || digit > 9) {
            return -1;
        }
        num += digit*ten;
        ten *= 10;
    }
    return num;
}

int active, MAX_ACTIVE;

/// Parallelized backtrack function
void backtrack(Graph& graph, const vector<vector<bool> >& coverable, 
               const vector<int>& vertices, int idx, int no_erased, int &min_erased, vector<bool>& erased, int n) {

    if (all(erased)) {
        if (no_erased < min_erased) {
            #pragma omp critical (min_erased)
            min_erased = no_erased;
        }
        return;
    }
    if (idx == n) {
        return;
    }
    
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

    // Make the recursive calls in separate threads if the
    // number of active threads is small and if there
    // isn't too many remaining nodes
    if ((active < MAX_ACTIVE) && (n-idx >= 20)) {
        #pragma omp critical (active)
        active++;
        #pragma omp parallel shared(min_erased,active)
        {
            #pragma omp sections
            {
                #pragma omp section
                backtrack(graph, coverable, vertices, 
                          idx+1, no_erased+1, min_erased, new_erased, n);
                #pragma omp section
                backtrack(graph, coverable, vertices, 
                          idx+1, no_erased, min_erased, erased, n);
            }
        }
        #pragma omp critical (active)
        active--;
    }
    else {
        backtrack(graph, coverable, vertices, 
                  idx+1, no_erased+1, min_erased, new_erased, n);
        backtrack(graph, coverable, vertices, 
                  idx+1, no_erased, min_erased, erased, n);
    }
}

bool comp (const pair<int,int>& p1, const pair<int,int>& p2) {
    return p1.first > p2.first;
}

/** Compute the size of the dominating set
    for `graph'.
    It's assumed that `graph' is connected
*/
int dominating_set(Graph& graph) {
    int n = graph.size();

    // Sort vertices by number of neighbors
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

    // coverable[i] has a vector<bool> indicating which nodes will
    // be covered by marking the nodes from vertices[i] to vertices[n-1]
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

/**
   Separate `graph' in its connected components
   and return them as a vector of graphs
*/
vector<Graph> connected_components(Graph graph) {
    int n = graph.size();
    vector<Graph> cc;

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

int main (int argc, char** argv) {
    int no_nodes, no_edges;
    active = 1;
    
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

        // Nested parallelization
        omp_set_nested(1);
        if (argc > 1) {
            MAX_ACTIVE = str2num(argv[1]);
            if (MAX_ACTIVE == -1) {
                cout << "Max number of threads is not an integer." << endl;
                return 1;
            }
        }
        else {
            MAX_ACTIVE = 4;
        }

        int dom_set_size = 0;
        for (unsigned int i = 0; i < cc.size(); i++) {
            if (active != 1) {
                assert(false);
            }
            dom_set_size += dominating_set(cc[i]);
        }

        cout << dom_set_size << endl;
    }

    return 0;
}

