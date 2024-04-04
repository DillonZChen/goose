import itertools
import numpy as np
import random
from sklearn import svm, linear_model
from sklearn.preprocessing import StandardScaler

from models.rank.transform import transform_pairwise


# RankSVM using linearSVC
class RankSVM(svm.LinearSVC):
    """
    Performs pairwise ranking svm with an underlying LinearSVC model
    initialise with a C of regularization term
    default using hinge loss
    """
    
    def __init__(self, C = 1.0):
        super(RankSVM, self).__init__()
        self.C = C
        self.loss = 'hinge'
        self.fit_intercept = False
        self.max_iter = 999999999
        
        
    def fit(self, X, y, mode = 0):
        """
        Fit a pairwise ranking model by first transfer it into pairwise than fitting
        Inputs
        ----------
        X : array, shape (n_samples, n_features)
        y : array, shape (n_samples,) or (n_samples, 2)
        mode: 0 for full pair
              1 for sequential pair
              2 for neighbouring + sequential pair
        Returns
        -------
        self
        """
        X_trans = X
        y_trans = y
        # if mode == 0:
        #     X_trans, y_trans = transform_pairwise(X, y)
        # elif mode == 1:
        #     X_trans, y_trans = transform_pairwise_sequential(X, y)
        # elif mode == 2:
        #     X_trans, y_trans = X, y
        # else:
        #     raise ValueError("invalid mode input")
        print(f'pair used for training: {X_trans.shape, y_trans.shape}')
        super(RankSVM, self).fit(X_trans, y_trans)
        return self

    # def predict(self, X):
    #     """
    #     Predict an ordering on X. For a list of n samples, this method
    #     returns a list from 0 to n-1 with the relative order of the rows of X.
    #     Inputs
    #     ----------
    #     X : array, shape (n_samples, n_features)
    #     Returns
    #     -------
    #     rtn: array, shape (n_samples,)
    #         Returns a list of integers representing the relative order of
    #         the rows in X.
    #     """
    #     if hasattr(self, 'coef_'):
    #         return np.argsort(np.dot(X, self.coef_.T).flatten())
    #     else:
    #         raise ValueError("Must call fit() prior to predict()")

    # def predict(self, x):
    #     """
    #     return pseduo heuristic for search
    #     """
    #     if hasattr(self, 'coef_'):
    #         x_r = x.reshape(1,-1)
    #         return np.dot(x_r, self.coef_.T)
    #     else:
    #         raise ValueError("Must call fit() prior to predict()")


    def score(self, X, y):
        """
        Returns the accuracy for the rank prediction, from 0-1
        """
        X_trans, y_trans = X, y
        predictions = super(RankSVM, self).predict(X_trans)
        mse = np.mean((predictions - y_trans)**2)
        accuracy = np.mean((np.abs(predictions - y_trans)) < 0.001)

        print(f"MSE: {mse}; Accuracy: {accuracy}")

    def h_val(self, x):
        """
        return pseudo heuristic for search
        """
        if hasattr(self, 'coef_'):
            x_r = x.reshape(1,-1)
            return np.dot(x_r, self.coef_.T)
        else:
            raise ValueError("Must call fit() prior to predict()")



def generate_feature_vec_relaxed(planning_graph, state, param):
    pass


class RankHeuristic:
    """
    Implement the heuristic using the trained RankSVM's coef for dot product
    
    Inadmissible, directly reflect the rank
    
    Default scale value is 10000
    """
    
    def __init__(self, svm : RankSVM, planning_graph, scale_1 = 100000, scale_2 = 1000):
        super().__init__()
        self.svm = svm
        self.planning_graph = planning_graph
        self.scale_1 = scale_1
        self.scale_2 = scale_2
        self.expand_nodes = 0
        
    def __call__(self, node):
        self.expand_nodes += 1
        if (self.planning_graph.task.goals <= node.state):
            print("reached goal state")
            return -999999
        vec = generate_feature_vec_relaxed(self.planning_graph, node.state, 99999999)
        h = round((self.svm.h_val(vec).item() + self.scale_1)*self.scale_2)
        if (h<0):
            print(f'original val: {self.svm.h_val(vec).item()}')
            print(f'heristic values:{h}')
        return h