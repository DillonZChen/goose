import networkx as nx
from _wlplan.graph import Graph


def from_networkx(graph: nx.Graph):
    node_colours = []
    node_names = []
    name_to_idx = {}

    for i, (node, attr) in enumerate(graph.nodes(data=True)):
        if "colour" in attr:
            colour = attr["colour"]
        elif "color" in attr:
            colour = attr["color"]
        else:
            raise ValueError(f"Node colour not specified for node {node}")

        node_colours.append(colour)
        node_names.append(node)
        name_to_idx[node] = i

    edges = [[] for _ in range(len(node_names))]
    for u, v, attr in graph.edges(data=True):
        if "label" in attr:
            relation = attr["label"]
        else:
            raise ValueError(f"Edge label not specified for edge ({u}, {v})")
        edges[name_to_idx[u]].append((relation, name_to_idx[v]))

    wlplan_graph = Graph(node_colours, node_names, edges)

    return wlplan_graph


def to_networkx(graph: Graph):
    G = nx.Graph()
    for u, colour in enumerate(graph.node_colours):
        node_name = graph.get_node_name(u)
        G.add_node(node_name, colour=colour)
    for u in range(len(graph.edges)):
        for r, v in graph.edges[u]:
            G.add_edge(graph.get_node_name(u), graph.get_node_name(v), relation=r)
    return G
