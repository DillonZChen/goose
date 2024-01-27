import networkx as nx
import matplotlib.pyplot as plt


def draw(graph):
    edge_labels = {(u, v): d["c"] for u, v, d in graph.edges(data=True)}
    pos = nx.shell_layout(graph)
    nx.draw(graph, pos=pos, with_labels=True)
    nx.draw_networkx_edge_labels(graph, pos=pos, edge_labels=edge_labels)
    plt.show()


def wl(G):
    cur_colours = {}
    histogram = {}

    iterations = len(G.nodes)
    _hash = {}

    def _get_hash_value(colour) -> int:
        if isinstance(colour, tuple) and len(colour) == 1:
            colour = colour[0]
        if colour not in _hash:
            _hash[colour] = len(_hash)
            # print(colour)
        return _hash[colour]

    def store_colour(colour):
        nonlocal histogram
        colour_hash = _get_hash_value(colour)
        if colour_hash not in histogram:
            histogram[colour_hash] = 0
        histogram[colour_hash] += 1

    # collect initial colours
    for u in G.nodes:
        # initial colour is feature of the node
        colour = G.nodes[u]["c"]
        cur_colours[u] = _get_hash_value(colour)
        # assert colour in _hash and colour>=0, colour
        store_colour(colour)

    # WL iterations
    for itr in range(iterations):
        new_colours = {}
        for u in G.nodes:
            # edge label WL variant
            neighbour_colours = []
            for i in [0, 1]:  # because of multigraph
                for v in G[u]:
                    colour_node = cur_colours[v]
                    edge = (u, v, i)
                    if edge not in G.edges:
                        continue
                    colour_edge = G.edges[edge]["c"]
                    neighbour_colours.append((colour_node, colour_edge))
            neighbour_colours = sorted(neighbour_colours)
            colour = tuple([cur_colours[u]] + neighbour_colours)
            new_colours[u] = _get_hash_value(colour)
            store_colour(colour)

        cur_colours = new_colours

    return histogram


G1 = nx.MultiGraph()
G1.add_nodes_from(
    [
        ("a", {"c": "OBJ"}),
        ("b", {"c": "OBJ"}),
        ("Q(a,b)", {"c": "TRUE"}),
        ("Q(b,a)", {"c": "TRUE"}),
        ("W(a,b)", {"c": "GOAL"}),
        ("W(b,a)", {"c": "GOAL"}),
        ("Q", {"c": "Q"}),
        ("W", {"c": "W"}),
    ]
)
G1.add_edges_from(
    [
        ("Q(a,b)", "Q", {"c": 0}),
        ("Q(b,a)", "Q", {"c": 0}),
        ("W(a,b)", "W", {"c": 0}),
        ("W(b,a)", "W", {"c": 0}),
        ("Q(a,b)", "a", {"c": 1}),
        ("Q(a,b)", "b", {"c": 2}),
        ("Q(b,a)", "a", {"c": 2}),
        ("Q(b,a)", "b", {"c": 1}),
        ("W(a,b)", "a", {"c": 1}),
        ("W(a,b)", "b", {"c": 2}),
        ("W(b,a)", "a", {"c": 2}),
        ("W(b,a)", "b", {"c": 1}),
    ]
)

###

G2 = nx.MultiGraph()
G2.add_nodes_from(
    [
        ("a", {"c": "OBJ"}),
        ("b", {"c": "OBJ"}),
        ("Q(a,a)", {"c": "TRUE"}),
        ("Q(b,b)", {"c": "TRUE"}),
        ("W(a,b)", {"c": "GOAL"}),
        ("W(b,a)", {"c": "GOAL"}),
        ("Q", {"c": "Q"}),
        ("W", {"c": "W"}),
    ]
)
G2.add_edges_from(
    [
        ("Q(a,a)", "Q", {"c": 0}),
        ("Q(b,b)", "Q", {"c": 0}),
        ("W(a,b)", "W", {"c": 0}),
        ("W(b,a)", "W", {"c": 0}),
        ("Q(a,a)", "a", {"c": 1}),
        ("Q(a,a)", "a", {"c": 2}),
        ("Q(b,b)", "b", {"c": 2}),
        ("Q(b,b)", "b", {"c": 1}),
        ("W(a,b)", "a", {"c": 1}),
        ("W(a,b)", "b", {"c": 2}),
        ("W(b,a)", "a", {"c": 2}),
        ("W(b,a)", "b", {"c": 1}),
    ]
)

w1 = wl(G1)
w2 = wl(G2)

print(w1)
print(w2)
print(w1 == w2)

breakpoint()
