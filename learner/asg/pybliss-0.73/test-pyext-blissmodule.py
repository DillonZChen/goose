# #! /usr/bin/env python
#
# import pyext_blissmodule as bliss
#
# graph = bliss.create()
# bliss.add_vertex(graph, 0)
# bliss.add_vertex(graph, 1)
# bliss.add_vertex(graph, 1)
# bliss.add_vertex(graph, 1)
# bliss.add_edge(graph, 0, 1)
# bliss.add_edge(graph, 0, 2)
# bliss.add_edge(graph, 0, 3)
# automorphisms = bliss.find_automorphisms(graph)
# assert type(automorphisms) is list
#
# print "Got %d automorphism(s)" % len(automorphisms)
# for aut_no, aut in enumerate(automorphisms):
#     assert type(aut) is list
#     print "automorphism #%d:" % aut_no
#     for from_index, to_index in enumerate(aut):
#         print "%d->%d" % (from_index, to_index)
