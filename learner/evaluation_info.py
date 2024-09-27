from dataclasses import dataclass
from typing import Dict, Optional

from util.statistics import print_mat


@dataclass
class EvaluationInfo:
    training_times: Optional[Dict[str, float]]
    train_scores: Dict[str, Dict[str, float]]  # schema -> metric -> score
    val_scores: Dict[str, Dict[str, float]]

    def dump(self) -> None:
        print("Training statistics:")
        if len(self.train_scores) > 1:
            if self.training_times is not None:
                print("Training times:")
                for key, value in self.training_times.items():
                    print(f"{key}: {value}s")
                print(f"Total: {sum(self.training_times.values())}s")
            metrics = next(iter(self.train_scores.values())).keys()
            for metric in metrics:
                mat = [[metric, "train", "val"]]
                for key in self.train_scores.keys():
                    mat.append(
                        [key, self.train_scores[key][metric], self.val_scores[key][metric]]
                    )
                print_mat(mat)
        else:
            if self.training_times is not None:
                print(f"Training time: {next(iter(self.training_times.values()))}s")
            metrics = next(iter(self.train_scores.values())).keys()
            mat = [["", "train", "val"]]
            key = next(iter(self.train_scores.keys()))
            for metric in metrics:
                mat.append(
                    [metric, self.train_scores[key][metric], self.val_scores[key][metric]]
                )
            print_mat(mat)