#ifndef DOM_SET_GRAPH_H
#define DOM_SET_GRAPH_H
#include <map>
#include <set>

class Graph {
public:
    Graph () {}

    void add_edge (int a, int b) { 
        edges[a].insert(b); 
        edges[b].insert(a);
    }

    const std::set<int>& neighbors (int a) {
        return edges[a];
    }

    unsigned int size () const { return edges.size(); }
    bool empty () const { return edges.empty(); }
private:
    std::map<int, std::set<int> > edges;
};

#endif
