from typing import Dict, List


SCS_ALL = "all"
SCS_NONE = "none"
SCS_SCHEMA_EXACT = "schema_exact"
SCS_SCHEMA_APPROX = "schema_approx"

ALL_KEY = "_all_"

def get_schemata_from_data(schema_strat, data : List[Dict[str, float]]):
    schemata = sorted(list(data[0].keys()))
    if schema_strat == SCS_NONE:
        schemata = [ALL_KEY]
    elif schema_strat == SCS_ALL:
        pass
    elif schema_strat in {SCS_SCHEMA_EXACT, SCS_SCHEMA_APPROX}:
        schemata.remove(ALL_KEY)
    return schemata
