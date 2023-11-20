import os

""" Module for reading information from logs. """


def predict_finished_correctly(f):
  log = open(f, 'r').read()
  finished_correctly = "Initial heuristic value" in log
  return finished_correctly

def search_finished_correctly(f):
  log = open(f, 'r').read()
  finished_correctly = "timed out after" in log or "Solution found." in log or "Time limit has been reached." in log
  return finished_correctly

def scrape_search_log(file):
  stats = {
    "first_h": -1,
    "solved": 0,
    "time": -1,
    "cost": -1,
    "expanded": -1,
    "evaluated": -1,
  }

  if not os.path.exists(file):
     return stats

  for line in open(file, 'r').readlines():

    line = line.replace(" state(s).", "")
    toks = line.split()
    if len(toks) == 0: continue
    if "until last jump" in line: continue

    if "Solution found." in line: 
      stats["solved"] = 1
    elif "Goal found at:" in line or "Actual search time:" in line: 
      stats["time"] = float(toks[-1].replace("s", ""))
    elif "Total plan cost:" in line or "Plan cost:" in line: 
      stats["cost"] = int(toks[-1])
    elif len(toks)>=2 and "Expanded" == toks[-2]: 
      stats["expanded"] = int(toks[-1])
    elif len(toks)>=2 and "Evaluated" == toks[-2]: 
      stats["evaluated"] = int(toks[-1])
    elif "Initial heuristic value" in line:
      try:
        stats["first_h"] = int(toks[-1])
      except:
        print(file)
        stats["first_h"] = -1

  return stats

def scrape_train_log(file):
  stats = {
    "epochs": -1,
    "time": 0,
    "model_path": -1,
    "best_avg_loss": float('inf'),
  }
  
  model_time = False
  arguments = False

  for line in open(file, 'r').readlines():
    line = line.replace(",", "")
    toks = line.split()
    if len(toks) == 0:
        continue
    
    if "___" in line:
        arguments = False
    if arguments:
        if len(toks) == 1:
          stats[toks[0]] = ""
        else:
          assert len(toks) == 2
          stats[toks[0]] = toks[1]
    if "Parsed arguments" in line:
        arguments = True

    if toks[0] == "epoch":
      stats["epochs"] = int(toks[1])
      stats["time"] += float(toks[-1])

    if model_time:
        stats["model_path"] = line.replace("\n", "")
        model_time = False
    if "Model parameter file" in line:
        model_time = True
    if toks[0] == "best_avg_loss":
       stats["best_avg_loss"] = float(toks[1])
       
  return stats
