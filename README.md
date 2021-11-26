# AXL
The Affine Quantum Circuit Specification Language -- A Domain Specific Language and Source-to-Source Compiler

Artifact associated to "On the Impact of Affine Loop Transformations" published in ACM Transactions on Quantum Computing (TQC), September 2021.

Installation:

./axl-install.sh

source axl-vars.sh

make

Use:

./axl < circuit.aql

To cite this work:

@article{10.1145/3465409,
author = {Kong, Martin},
title = {On the Impact of Affine Loop Transformations in Qubit Allocation},
year = {2021},
issue_date = {September 2021},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
volume = {2},
number = {3},
issn = {2643-6809},
url = {https://doi.org/10.1145/3465409},
doi = {10.1145/3465409},
abstract = {Most quantum compiler transformations and qubit allocation techniques to date are either peep-hole focused or rely on sliding windows that depend on a number of external parameters including the topology of the quantum processor. Thus, global optimization criteria are still lacking. In this article, we explore the synergies and impact of affine loop transformations in the context of qubit allocation and mapping. With this goal in mind, we designed and implemented AXL, a domain specific language and source-to-source compiler for quantum circuits that can be directly described with affine relations. We conduct an extensive evaluation spanning circuits from the recently introduced QUEKO benchmark suite, eight quantum circuits taken from the literature, three distinct coupling graphs, four affine transformations (including the Pluto dependence distance minimization and Feautrier’s minimum latency algorithms), four qubit allocators, and two back-end quantum compilers. Our results demonstrate that affine transformations using global optimization criteria can cooperate effectively in several scenarios with quantum qubit mapping algorithms to reduce the circuit depth, size and allocation time.},
journal = {ACM Transactions on Quantum Computing},
month = {sep},
articleno = {9},
numpages = {40},
keywords = {quantum computing, Polyhedral model, qubit allocation, affine transformations}
}
