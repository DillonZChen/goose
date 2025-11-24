""" Main training pipeline script. """

import torch
import argparse
from util.save_load import *
from dataset.dataset import get_graphs_from_plan


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("domain_file")
    parser.add_argument("problem_file")
    parser.add_argument("model_file")
    
    parser.add_argument("-o", "--output_file", default=None)
    
    # gpu device (if exists)
    parser.add_argument("--device", type=int, default=0)

    args = parser.parse_args()

    return args


if __name__ == "__main__":
    args = parse_args()
    print_arguments(args)
    
    domain_pddl = args.domain_file
    problem_pddl = args.problem_file
    model_file = args.model_file

    # cuda
    device = torch.device(f"cuda:{args.device}" if torch.cuda.is_available() else "cpu")

    # load data and init model
    model, model_args = load_model(model_file)
    graph = get_graphs_from_plan(domain_pddl, problem_pddl, plan_file=None, args=model_args)[0]
    print(f"model size (#params): {model.get_num_parameters()}")
    
    embedding = model.embeddings(graph)
    
    if args.output_file is not None:
        torch.save(embedding, args.output_file)
        print(f"Saved embedding to {args.output_file}")
