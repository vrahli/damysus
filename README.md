# DAMYSUS: Streamlined BFT Consensus Leveraging Trusted Components



## Current status

The software is under ongoing development.



## Installing

To tests our protocols, we provide a Python script, called
`experiments.py`, as well as a `Dockerfile` to create a Docker
container. We use the
[Salticidae](https://github.com/Determinant/salticidae) library, which
is added here as git submodule.

### Salticidae

After cloning the repository you need to type this to initialize the
Salticidae git submodule:

    `git submodule init`

followed by:

    `git submodule update`


### Python

The Python scripts relies on the following modules:
- subprocess
- pathlib
- matplotlib
- time
- math
- os
- glob
- datetime
- argparse
- enum
- json
- multiprocessing
- random
- shutil
- re

If you haven't installed those modules yet, run:

    `python -m pip install subprocess pathlib matplotlib time math os glob datetime argparse enum json multiprocessing random shutil re`

### Docker

To run the experiments within Docker containers, you need to have
installed Docker on your machine. This
[page](https://docs.docker.com/engine/install/) explains how to
install Docker.

You then need to create the container by typing:

    `docker build -t damysus .`

This will create a container called `damysus`.



## Usage

### Default command

We explain the various options our Python scripts provides. You will
run commands of the following form, followed by various options
explained below:

    `python3 experiments.py --docker --pall`

### Options

In addition, you can use the following options to change some of the parameters:
- `--repeats n` to change the number of repeats per experiment to `n`
- `--payload n` to change the payload size to `n`
- `--faults a,b,c` to run the experiments for f=a, f=b, etc.
- `--pall` is to run all protocols, instead you can use `--p1` up to `--p6`
    - `--p1`: base protocol, i.e., HotStuff
    - `--p2`: Damysus-C (checker only)
    - `--p3`: Damysus-A (accumulator only)
    - `--p4`: Damysus
    - `--p5`: chained base protocol, i.e., chained HotStuff
    - `--p6`: chained Damysus
- `--netlat n` to change the network latency to `n`ms
- `--clients1 n` to change the number of clients to `n` for the non-chained protocols
- `--clients2 n` to change the number of clients to `n` for the chained protocols
- `--tvl` to compute a "max throughput" graph

### Examples

For example, if you run:

    `python3 experiments.py --docker --p1 --p2 --repeats 2 --faults 1`

then you will run the replicas within Docker containers (`--docker`),
test the base protocol (`--p1`) and Damysus-C (`--p2`), repeat the
experiments twice (`--repeats 2`), and test for f=1 (`--faults 1`).

If your run:

    `python3 experiments.py --docker --pall --repeats 10 --faults 1,2,4`

then you will run the replicas within Docker containers (`--docker`),
test all protocols (`--pall`), repeat the experiments 10 times
(`--repeats 10`), and test for f=1, f=2, and f=4 (`--faults 1,2,4`)

### Recommended experiments

We recommend that you try the following experiments. Note that 100
repetitions will take quite a long time. You can start with a smaller
value between 2 and 10 to get an idea of the results you will obtain.

- Run all protocols with a 0ms network latency, 0B payloads, for
  f=1,2,4,10, with 10 repetitions per experiment

    `python3 experiments.py --docker --pall --netlat 0 --payload 0 --faults 1,2,4,10 --repeats 100`

- Run all protocols with a 100ms network latency, 256B payloads, for
  f=1,2,4,10, with 10 repetitions per experiment

    `python3 experiments.py --docker --pall --netlat 1000 --payload 256 --faults 1,2,4,10 --repeats 100`



# Acknowledgments

This work was partially supported by the Australian Research Council
(ARC) under project DE210100019.



# Contact

Feel free to contact any of the authors if you have questions:
[Jeremie Decouchant](https://www.tudelft.nl/ewi/over-de-faculteit/afdelingen/software-technology/distributed-systems/people/jeremie-decouchant),
David Kozhaya,
[Vincent Rahli](https://www.cs.bham.ac.uk/~rahliv/),
and [Jiangshan Yu](https://research.monash.edu/en/persons/jiangshan-yu).
