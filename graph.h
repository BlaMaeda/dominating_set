#ifndef DOM_SET_GRAPH_H
#define DOM_SET_GRAPH_H
#include <iostream>
#include <map>
#include <set>
#include <climits>
#include <algorithm>

class Graph {
public:
    Graph () {}

    void add_edge (int a, int b) { 
        if (!this->contains(a)) {
            nodes.insert(a);
            edges[a].insert(a);
        }
        if (!this->contains(b)) {
            nodes.insert(b);
            edges[b].insert(b);
        }
        edges[a].insert(b); 
        edges[b].insert(a);
    }

    bool contains (int x) {
        return nodes.find(x) != nodes.end();
    }

    std::set<int> get_nodes () { return nodes; }

    const std::set<int>& neighbors (int a) {
        return edges[a];
    }

    /// Make nodes go from 0 to N-1
    void normalize () {
        std::map<int, int> ver2nor;
        std::map<int, std::set<int> >::iterator m_it;
        std::set<int>::iterator s_it;

        int i = 0;
        for (s_it = nodes.begin(); s_it != nodes.end(); s_it++) {
            ver2nor[*s_it] = i++;
        }

        Graph g;
        for (m_it = edges.begin(); m_it != edges.end(); m_it++) {
            for (s_it = m_it->second.begin(); s_it != m_it->second.end(); s_it++) {
                if (*s_it >= m_it->first) {
                    g.add_edge(ver2nor[*s_it], ver2nor[m_it->first]);
                }
            }
        }

        (*this) = g;
    }

    void print () {
        std::map<int, std::set<int> >::iterator m_it;
        std::set<int>::iterator s_it;

        std::cout << "Nodes: " << this->size() << std::endl << std::endl;
        std::cout << "Edges:" << std::endl;

        for (m_it = edges.begin(); m_it != edges.end(); m_it++) {
            for (s_it = m_it->second.begin(); s_it != m_it->second.end(); s_it++) {
                if (*s_it >= m_it->first)
                    std::cout << m_it->first << " " << *s_it << std::endl;
            }
        }
        std::cout << std::endl;
    }

    unsigned int size () const { return nodes.size(); }
    bool empty () const { return edges.empty(); }
private:
    std::set<int> nodes;
    std::map<int, std::set<int> > edges;
};

#endif
