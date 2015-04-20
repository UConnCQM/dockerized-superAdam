Stochastic Discrete Dynamical System (SDDS)

=== AUTHORS ===

Seda Arat

=== ABOUT ===

SDDS module simulates the average trajectory for each variable out of numberofSimulations trajectories deterministically or stochastically.

=== RUN ===

perl SDDS.pl -i ../test/test1/sample-input.json -o ../test/test1/sample-output.json -s 10

This is a ternary system. The simulation is stochastic and the seed number (-s in the command line) 10 fixes the random number generator so we can produce the same simulation results.