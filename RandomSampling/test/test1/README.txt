Random Sampling

=== AUTHORS ===

Seda Arat

=== ABOUT ===

Random sampling module computes a basin of attractors of the systems with large state space. It randomly picks initial states depending on the sampling size and finds at which attractor they end up.

=== RUN ===

perl RandomSampling.pl -i ../test/test1/sample-input.json -o ../test/test1/sample-output.json -s 10

This is a boolean system. The sampling is random and the seed number (-s in the command line) 10 fixes the random number generator so we can produce the same initial states every time we run the program.