#ifndef DOM_SET_GRAPH_H
#define DOM_SET_GRAPH_H
#include <iostream>
#include <map>
#include <set>
#include <climits>
#include <algorithm>

class Graph {
public:
    Graph (int n) { no_nodes = n; }

    void add_edge (int a, int b) { 
        if (!this->contains(a)) {
            nodes.insert(a);
        }
        if (!this->contains(b)) {
            nodes.insert(b);
        }
        edges[a].insert(b); 
        edges[b].insert(a);
    }

    Graph sub_graph (const std::set<int>& n) {
        Graph g(n.size());

        for (std::set<int>::iterator s_it = n.begin(); s_it != n.end(); s_it++) {
            int a = *s_it;
            for (std::set<int>::iterator n_it = edges[a].begin(); n_it != edges[a].end(); n_it++) {
                int b = *n_it;
                g.add_edge(a, b);
            }
        }

        return g;
    }

    bool contains (int x) {
        return nodes.find(x) != nodes.end();
    }

    std::set<int> get_nodes () { return nodes; }

    void remove_edge (int a, int b) {
        std::set<int>::iterator s_it;

        s_it = edges[a].find(b);
        if (s_it != edges[a].end()) {
            edges[a].erase(s_it);
        }

        s_it = edges[b].find(a);
        if (s_it != edges[b].end()) {
            edges[b].erase(s_it);
        }
    }

    void remove_node (int a) {
        std::map<int, std::set<int> >::iterator m_it;

        m_it = edges.find(a);
        if (m_it != edges.end()) {
            edges.erase(m_it);
        }
    }

    const std::set<int>& neighbors (int a) {
        return edges[a];
    }

   //  void normalize () {
   //      std::map<int, int> ver2nor;
   //      std::map<int, std::set<int> >::iterator m_it;
   //      std::set<int>::iterator s_it;

   //      int i = 0;
   //      for (s_it = nodes.begin(); s_it != nodes.end(); s_it++) {
   //          ver2nor[*s_it] = i++;
   //      }

   //      Graph g;
   //      for (m_it = edges.begin(); m_it != edges.end(); m_it++) {
   //          for (s_it = m_it->second.begin(); s_it != m_it->second.end(); s_it++) {
   //              if (*s_it >= m_it->first) {
   //                  g.add_edge(ver2nor[*s_it], ver2nor[m_it->first]);
   //              }
   //          }
   //      }

   //      (*this) = g;
   //  }

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

    unsigned int size () const { return no_nodes; }
    bool empty () const { return edges.empty(); }
private:
    std::set<int> nodes;
    std::map<int, std::set<int> > edges;
    int no_nodes;
};

#endif
