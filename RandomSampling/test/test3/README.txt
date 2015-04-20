Random Sampling

=== AUTHORS ===

Seda Arat

=== ABOUT ===

Random sampling module computes a basin of attractors of the systems with large state space. It randomly picks initial states depending on the sampling size and finds at which attractor they end up.


=== RUN ===

perl SDDS.pl -i ../test/test3/sample-input.json -o ../test/test3/sample-output.json -s 2509

This is a ternary system including iron homeostasis proteins. The sampling is random and the seed number (-s in the command line) 2509 fixes the random number generator so we can produce the same initial states every time we run the program.