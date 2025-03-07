import os
from itertools import count


def _try_remove(f):
    try:
        os.remove(f)
    except OSError:
        return False
    return True

def cleanup_temporary_files(args):
    _try_remove(args.sas_file)
    _try_remove(args.sas_file + ".num")
    _try_remove(args.plan_file)

    for i in count(1):
        if not _try_remove("%s.%s" % (args.plan_file, i)):
            break
