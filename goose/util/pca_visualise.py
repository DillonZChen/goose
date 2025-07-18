import logging
import os

import numpy as np
import termcolor as tc
from matplotlib import pyplot as plt
from sklearn.decomposition import PCA


def visualise(X, y, save_file):
    pca = PCA(n_components=2)
    X_r = pca.fit(X).transform(X)
    X_r -= np.mean(X_r, axis=0)
    X_r /= np.max(np.abs(X_r), axis=0)
    max_y = int(round(max(y)))
    min_y = int(round(min(y)))
    cmap = plt.cm.get_cmap("hsv", max_y - min_y + 1)
    y = np.array(y)
    for i in range(min_y, max_y + 1):
        plt.scatter(X_r[y == i, 0], X_r[y == i, 1], color=cmap(i), alpha=0.8)
    plt.xlim((-1, 1))
    plt.ylim((-1, 1))
    plt.xticks([])
    plt.yticks([])
    # plt.title(domain.name.capitalize())
    plt.gca().set_aspect("equal", adjustable="box")
    save_dir = os.path.dirname(save_file)
    if len(save_dir) > 0:
        os.makedirs(save_dir, exist_ok=True)
    plt.tight_layout()
    plt.savefig(save_file, bbox_inches="tight")
    logging.info(f"Saved PCA visualisation to {tc.colored(save_file, 'blue')}")
