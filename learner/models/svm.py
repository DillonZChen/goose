import itertools
import numpy as np
import random
from sklearn import svm, linear_model
from sklearn.preprocessing import StandardScaler

# helper function for svms
def transform_pairwise(X, y):
    """
    Transforms data into pairs for convex relaxation of kendal rank correlation coef
    In this method, all pairs are choosen, except for those that have the same target value or equal cost
    Inputs
    ----------
    X : array, shape (n_samples, n_features)
        The input feature vec of states from of several problems
    y : array, shape (n_samples,) or (n_samples, 2)
        The input cost vector. If it's a 2D array, the second column represents
        the problem index
    Returns
    -------
    X_trans : array, shape (k, n_feaures)
        Difference between features of states (si - sj), only consider the state pair from the same problem
    y_trans : array, shape (k,)
        Output rank labels of values {-1, +1}, 1 represent si has potentially larger cost than sj (further away from goal)
    """
    X_new = []
    y_new = []
    if y.ndim == 1:
        y = np.c_[y, np.ones(y.shape[0])]  
    comb = itertools.combinations(range(X.shape[0]), 2)
    for k, (i, j) in enumerate(comb):
        if y[i, 0] == y[j, 0] or y[i, 1] != y[j, 1]:
            # skip if they have the same cost or are from different problem group
            continue
        # otherwise, make the new pair-wise data
        X_new.append(X[i] - X[j])
        y_new.append(np.sign(y[i, 0] - y[j, 0])) # y = 1 if xi further away (larger cost), Vice Vesa
#         randomly output some negative values for training purpose
#         if y_new[-1] != (-1) ** k:
#             y_new[-1] = - y_new[-1]
#             X_new[-1] = - X_new[-1]
    length = len(y_new)
    random_indices = random.sample(range(length), length // 2)
    for i in random_indices:
        y_new[i] = - y_new[i]
        X_new[i] = - X_new[i]
    return np.asarray(X_new), np.asarray(y_new)

def transform_pairwise_sequential(X, y):
    """
    Transforms data into pairs for convex relaxation of kendal rank correlation coef
    In this method, all pairs are choosen, except for those that have the same target value or equal cost
    Inputs
    ----------
    X : array, shape (n_samples, n_features)
        The input feature vec of states from of several problems
    y : array, shape (n_samples,) or (n_samples, 2)
        The input cost vector. If it's a 2D array, the second column represents
        the problem index
    Returns
    -------
    X_trans : array, shape (k, n_feaures)
        Difference between features of states (si - sj), only consider the state pair from the same problem
    y_trans : array, shape (k,)
        Output rank labels of values {-1, +1}, 1 represent si has potentially larger cost than sj (further away from goal)
    """
    X_new = []
    y_new = []
    if y.ndim == 1:
        y = np.c_[y, np.ones(y.shape[0])]
    # Get unique prpblem indecies from the second column
    problems = np.unique(y[:, 1])

    # Group the indices based on the categories
    groups = [list(np.where(y[:, 1] == prob)[0]) for prob in problems] 
    # make the new pair-wise data
    for prob in groups:
        for i in range(len(prob)-1):
            # add sequential pairs
            index = prob[i]
            X_new.append(X[index]-X[index+1])
            y_new.append(np.sign(y[index,0] - y[index+1,0]))
    # randomly output some negative values for training purpose
    length = len(y_new)
    random_indices = random.sample(range(length), length // 2)
    for i in random_indices:
        y_new[i] = - y_new[i]
        X_new[i] = - X_new[i]
    return np.asarray(X_new), np.asarray(y_new)



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
        if mode == 0:
            X_trans, y_trans = transform_pairwise(X, y)
        elif mode == 1:
            X_trans, y_trans = transform_pairwise_sequential(X, y)
        elif mode == 2:
            X_trans, y_trans = X, y
        else:
            raise ValueError("invalid mode input")
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

    def predict(self, x):
        """
        return pseduo heuristic for search
        """
        if hasattr(self, 'coef_'):
            x_r = x.reshape(1,-1)
            return np.dot(x_r, self.coef_.T)
        else:
            raise ValueError("Must call fit() prior to predict()")


    def score(self, X, y):
        """
        Returns the accuracy for the rank prediction, from 0-1
        """
        X_trans, y_trans = transform_pairwise(X, y)
        predictions = super(RankSVM, self).predict(X_trans)
        mse = np.mean((predictions - y_trans)**2)
        accuracy = np.mean((np.abs(predictions - y_trans)) < 0.001)

        print(f"MSE: {mse}; Accuracy: {accuracy}")
    # def h_val(self, x):
    #     """
    #     return pseduo heuristic for search
    #     """
    #     if hasattr(self, 'coef_'):
    #         x_r = x.reshape(1,-1)
    #         return np.dot(x_r, self.coef_.T)
    #     else:
    #         raise ValueError("Must call fit() prior to predict()")
    #


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