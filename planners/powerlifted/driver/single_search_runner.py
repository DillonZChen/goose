import os
import subprocess

def run_single_search(build_dir,
                      domain,
                      instance,
                      time,
                      translator_file,
                      search,
                      model_file,
                      evaluator,
                      generator,
                      state,
                      seed,
                      plan_file,
                      extra):
        cmd = [os.path.join(build_dir, 'search', 'search'),
               '-d', domain,
               '-i', instance,
               '-f', translator_file,
               '-s', search,
               '-m', model_file,
               '-e', evaluator,
               '-g', generator,
               '-r', state,
               '--seed', seed] + \
                   ['--plan-file', plan_file] +\
               extra
        print(f'Executing "{" ".join(cmd)}"')
        try:
            code = subprocess.call(cmd, timeout=time)
            # print(f"Iteration finished correctly; code={code}.")
            print(f"Iteration finished correctly.")
        except subprocess.TimeoutExpired as e:
            print(f"Iteration ran out of time: {e}")
            return -1
        except:
            print(f"Iteration finished with unknown error.")
        return code
