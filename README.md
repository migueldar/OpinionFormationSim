# OPINION FORMATION SIM
This repo is a provides a gui to simulate one dimensional opinion formation in societies. 

## Simulation

We start the simulation, with N agents arranged in a rectangular lattice, each of them is given an opinion taken uniformly from [0, 1]. Then the links(relationships) are created, a link between any pair of agents has a probability of $\frac{1}{{d^2}}$ of being created, where $d$ is the distance between the agents in the lattice. Every step in the simulation, links have a probability to activate, when a link activates between to agents, they re-adjust their opinion if their difference in opinion is smaller than a threshold $d$ (if $|x - x'|$ \> $d$) in the following way:

$x = x + \mu(x' - x)$ \
$x' = x' + \mu(x - x')$

where $x$ and $x'$ are the opinions of the agents and $\mu$ is the convergence parameter whose values may range from 0 to 0.5. The aim of this model is to emulate complex societies and how opinion of individuals vary depending on their social networks. All the previously mention parameters can be modified in the gui, to allow for a wide variety of possible simulations.

## Dependencies
This project was made using [Dear ImGui](https://github.com/ocornut/imgui), with the rendering backends glfw, and openGL 3.0. The backends will need to be installed in your system for the project to run (use your package manager). Testing was only done in Linux, the project probably will not work in other distros.
