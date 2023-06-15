# #! /usr/bin/env python
#
# import pybind11_blissmodule as bliss
#
# graph = bliss.DigraphWrapper()
# graph.add_vertex(0)
# graph.add_vertex(1)
# graph.add_vertex(1)
# graph.add_vertex(1)
# graph.add_edge(0, 1)
# graph.add_edge(0, 2)
# graph.add_edge(0, 3)
# automorphisms = graph.find_automorphisms()
# assert type(automorphisms) is list
#
# print "Python tester"
# print "Got %d automorphism(s)" % len(automorphisms)
# for aut_no, aut in enumerate(automorphisms):
#     assert type(aut) is list
#     print "automorphism #%d:" % aut_no
#     for from_index, to_index in enumerate(aut):
#         print "%d->%d" % (from_index, to_index)
#
#
