#!/usr/bin/python

from random import choice, randrange

def random_edges(xs):
    """Generate random edges for nodes in xs."""
    n = len(xs)

    possible = []
    result = []
    for i in range(n):
        for j in range(i+1, n):
            possible.append( (xs[i], xs[j]) )

    no_edges = randrange(0, len(possible) + 1)

    for i in range(no_edges):
        edge = choice(possible)
        result.append(edge)
        possible.remove(edge)

    return result

def generate_case(N, f):
    """Generate a random graph of N nodes and append it to the f file handler."""
    nodes = range(1, N+1)
    edges = random_edges(nodes)
    
    f.write("%i\n" % N)
    f.write("%i\n" % len(edges))
    for (n1, n2) in edges:
        f.write("%s %s\n" % (n1, n2))


def generate_set(T, filename, N1 = 1, N2 = 100):
    """Generate T random graphes and writes them in filename.

    T -- Number of cases to generate.
    filename -- Name of the file that will be written.
    N -- Number of nodes in each case. If this is not specified, a random
         number of nodes in the range [1, 100] is used.

    """
    f = open(filename, 'w')
    
    for i in range(T):
        generate_case(randrange(N1, N2+1), f)

    f.write("0 0\n")
    f.close()
