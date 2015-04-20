Random Sampling

=== AUTHORS ===

Seda Arat

=== ABOUT ===

Random sampling module computes a basin of attractors of the systems with large state space. It randomly picks initial states depending on the sampling size and finds at which attractor they end up.

=== RUN ===

perl RandomSampling.pl -i ../test/test2/sample-input.json -o ../test/test2/sample-output.json -s 20

This is a system with 4 proteins that can take up to 5 values. The sampling is random and the seed number (-s in the command line) 20 fixes the random number generator so we can produce the same initial states every time we run the program.