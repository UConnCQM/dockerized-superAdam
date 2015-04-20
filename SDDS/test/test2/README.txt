Stochastic Discrete Dynamical System (SDDS)

=== AUTHORS ===

Seda Arat

=== ABOUT ===

SDDS module simulates the average trajectory for each variable out of numberofSimulations trajectories deterministically or stochastically.

=== RUN ===

perl SDDS.pl -i ../test/test2/sample-input.json -o ../test/test2/sample-output.json -s 45

This is a system with 4 proteins that can take up to 5 values. The simulation is stochastic and the seed number (-s in the command line) 45 fixes the random number generator so we can produce the same simulation results.