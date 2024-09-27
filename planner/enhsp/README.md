# ENHSP-2024
An [ENHSP](https://sites.google.com/view/enhsp/) binary containing satisficing configurations from **Novelty Heuristics, Multi-Queue Search, and Portfolios for Numeric Planning** at SOCS-24. See [reference](#reference) for the bib entry.

## Dependencies
The binary is compiled with OpenJDK 17. If you are using Ubuntu, you can install the required Java Runtime Environment with
```
sudo apt install openjdk-17-jre
``` 
If you are using a different system, any JRE version `>=17` is likely to work.

## Usage
The input is a PDDL domain file `<domain>`, a PDDL problem file `<problem>`, and an ENHSP configuration `<config>`. The command line instruction is
```
java -jar enhsp.jar -o <domain> -f <problem> -planner <config>
```
where the top new configurations ordered by total coverage in the paper are
- `sat-mq3h3n`
- `sat-mq3h`
- `sat-mq3n`
- `sat-hiqb2add`
- `sat-hiqb2mrphj`
- `sat-hmd`

Note that performance of a configuration depends on the domain, and lower ranked configurations could perform better for your use case.

## TODOs
We plan to integrate the source code with the original ENHSP source code in the near or far future.

## Reference
```
@inproceedings{chen:thiebaux:socs2024,
  author       = {Dillon Ze Chen and Sylvie Thi{\'{e}}baux},
  title        = {Novelty Heuristics, Multi-Queue Search, and Portfolios for Numeric Planning},
  booktitle    = {17th International Symposium on Combinatorial Search (SoCS)},
  year         = {2024},
}
```
