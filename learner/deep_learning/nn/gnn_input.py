from dataclasses import dataclass
from typing import Dict, List, Optional

from torch import Tensor


@dataclass
class GnnInput:
    x: Tensor
    edge_indices: List[Tensor]
    index_to_node: Optional[Dict[int, str]] = None

    @property
    def n_features(self) -> int:
        return self.x.size(1)

    @property
    def n_edge_labels(self) -> int:
        return len(self.edge_indices)

    @property
    def n_nodes(self) -> int:
        return self.x.size(0)

    @property
    def n_edges(self) -> int:
        # assuming undirected graph
        return sum(e.size(1) for e in self.edge_indices) // 2

    def to(self, device) -> None:
        self.x = self.x.to(device)
        self.edge_indices = [e.to(device) for e in self.edge_indices]

    def dump(self, verbosity=0) -> None:
        print(f"n_features: {self.n_features}")
        print(f"n_edge_labels: {self.n_edge_labels}")
        print(f"n_nodes: {self.n_nodes}")
        print(f"n_edges: {self.n_edges * 2}")
        for i, edges in enumerate(self.edge_indices):
            print(f"  n_edges_{i}: {edges.size(1)}")
        if verbosity > 0:
            print("x:")
            for i, row in enumerate(self.x.long().tolist()):
                print(f"{i}: ", end="")
                for val in row:
                    print(val, end=" ")
                print()
            print("edge_indices:")
            for i, edges in enumerate(self.edge_indices):
                print(f"label_{i}")
                for val in edges[0]:
                    print(val.item(), end=" ")
                print()
                for val in edges[1]:
                    print(val.item(), end=" ")
                print()
