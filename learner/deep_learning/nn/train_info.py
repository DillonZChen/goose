class GnnTrainInfo:
    def __init__(self) -> None:
        self.best_epoch = 0
        self.best_train_loss = float("inf")
        self.best_val_loss = float("inf")
        self.best_combined_loss = float("inf")
        self.train_curve = []
        self.val_curve = []
        self.training_time = 0

    def dump(self) -> None:
        print("Training statistics:")
        print(f"best epoch: {self.best_epoch}")
        print(f"train_loss at best epoch: {self.best_train_loss}")
        print(f"val_loss at best epoch: {self.best_val_loss}")
        print(f"combined_loss at best epoch: {self.best_combined_loss}")
        print(f"total epochs: {len(self.train_curve)}")
        print(f"training time: {self.training_time}")
