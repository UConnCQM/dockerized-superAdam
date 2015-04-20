# Author: Seda Arat
# Name: Basin of Attractors for Large Systems using Random Sampling Synch Update
# Revision Date: February 18, 2015

#!/usr/bin/perl

use strict;
use warnings;

############################################################
###### REQUIRED PERL MODULES before running the code #######
############################################################

use Getopt::Euclid;
use JSON::Parse;
use JSON;
# use Data::Dumper;

############################################################

 # TO_DO: use the transition table information if available

############################################################

=head1 NAME

RandomSampling.pl - Find a basin of attractors of a given discrete system with a large state space using random sampling and a synchronous update schedule.

=head1 USAGE

RandomSampling.pl -i <input-file> -o <output-file> -s <seed-number>

=head1 SYNOPSIS

RandomSampling.pl -i <input-file> -o <output-file> -s <seed-number>

=head1 DESCRIPTION

RandomSampling.pl - Find the basin of attractors of a given discrete system with a large state space using random sampling and a synchronous update schedule.

=head1 REQUIRED ARGUMENTS

=over

=item -i[nput-file] <input-file>

The JSON file containing the input information (.json). 

=for Euclid:

network-file.type: readable

=item -o[utput-file] <output-file>

The JSON file containing a basin of attractors

=for Euclid:

file.type: writable

=head1 OPTIONAL ARGUMENTS

=over

=item -s[eed-number] <seed-number>

The seed number for the random number generator. User enters a number if s/he wants to fix the seed so the random number generator will always generate the same number. Otherwise, no need to be specified.

=back

=head1 AUTHOR

Seda Arat

=cut

# input
my $inputFile = $ARGV{'-i'};

# output
my $outputFile = $ARGV{'-o'};

# seed
my $seed = $ARGV{'-s'};

# it is for random number generator
if (defined $seed) {
  srand ($seed);
}
else {
  srand (time() | $$);
}

# converts input.json to Perl format
my $task = JSON::Parse::json_file_to_perl ($inputFile);
my $input = $task->{'task'}->{'input'}->[0];
my $samplingSize = $task->{'task'}->{'method'}->{'arguments'}->[0]->{'samplingSize'};

# sets the update rules/functions (array)
my $updateRules = $input->{'updateRules'};
my $num_rules = scalar @$updateRules;

# sets the variables in the model (array)
my $variables = $input->{'variables'};
my $num_variables = scalar @$variables;

# sets the number of variables in the model (scalar)
my $numberVariables = $input->{'numberVariables'};

# sets the unified (maximum prime) number that each state can take values up to (scalar)
my $num_states = $input->{'fieldCardinality'};

# upper limits
my $max_num_sampling = 10**6;

my $stateSpaceSize = $num_states ** $numberVariables;

errorCheck ();
my ($polyFuncs, $tranTables) = get_updateRules ();
my ($statesAndAttractors, $basinOfAttractors) = get_basinOfAttractors ();
my $tempFile = format_output ();

system ("cp $tempFile $outputFile");

#system ("M2 ../../share/prettify-json $tempFile $outputFile");

exit;

############################################################

############################################################
####################### SUBROUTINES ########################
############################################################

=pod

errorCheck ();

Checks if the user enters the options/parameters correctly and there is any internal error

=cut

sub errorCheck {
  
  # num_variables
  unless ($numberVariables == $num_variables) {
    print ("<br>INTERNAL ERROR: There is inconsistency betwen the size of variables ($num_variables) and numberVariables ($numberVariables). <br>");
    exit;
  }

  # num_rules
  unless ($numberVariables == $num_rules) {
    print ("<br>INTERNAL ERROR: There is inconsistency betwen the size of update rules ($num_rules) and numberVariables ($numberVariables). <br>");
    exit;
  }

  # samplingSize
  if (isnot_number ($samplingSize) || ($samplingSize < 1) || ($samplingSize > $max_num_sampling)) {
    print ("<br>ERROR: The sampling size must be a number between 1 and $max_num_sampling. <br>");
    exit;
  }

}

############################################################

=pod

get_updateRules ();

Returns 2 hash tables one of which is for polynomial functions and another is for transition tables

=cut

sub get_updateRules {
  my $polyFuncs = {};
  my $tranTables = {};

  foreach my $i (@$updateRules) {
    my $key = $i->{"target"};
    
    if (defined $i->{"functions"}->[0]->{"polynomialFunction"}) {
      my $temp = $i->{"functions"}->[0]->{"polynomialFunction"};
      $temp =~ s/\^/\*\*/g; # replace carret with double stars
      $polyFuncs->{$key} = $temp;
    }
    elsif (defined $i->{"functions"}->[0]->{"transitionTable"}) {
      my $value_tt = {};
      foreach my $j (@{$i->{"functions"}->[0]->{"transitionTable"}}) {
	my $k = join (' ', $j->[0]);
	$value_tt->{$k} = $j->[1];
      }
      $tranTables->{$key} = $value_tt;
    }
    else {
       print ("<br>INTERNAL ERROR: Neither polynomial function or transition table is available as an update rule for x$i. <br>");
      exit;
    }
  }
  
  return ($polyFuncs, $tranTables);
}

############################################################

=pod

get_basinOfAttractors ();

Returns 2 hash tables one of which is for initialState-attractor and another is for attractor-size

=cut

sub get_basinOfAttractors {
  my $is_attractor_table = {};
  my $attractor_size_table = {};

  my $length = 5;
  my %state_attractor_table;
  my %attractor_table;
  
  for (my $index = 1; $index <= $samplingSize; $index++) {
    my $n = int (rand ($stateSpaceSize));
    my $is = convert_from_integer_to_state ($n);
    
    if (exists $is_attractor_table->{$is}) {
      next;
    }
    
    my @array = ($is);
    
  attr: while (1) {
      for (my $n = 1; $n <= $length; $n++) {
	push (@array, get_nextstate ($array[-1]));
      }
      
      my $arraysize = scalar @array;
      
      for (my $j = 0; $j < $arraysize - 1; $j++) {
	for (my $k = $j + 1; $k < $arraysize; $k++) {
	  if ($array[$j] eq $array[$k]) {
	    
	    my @sub_array = @array[$j ... $k - 1];
	    my $sortedAttractor = join (' , ', sort {$a cmp $b} @sub_array);
	    
	    unless (exists $attractor_size_table->{$sortedAttractor} ) {
	      my $attractor = join (' -> ', @sub_array);
	      $attractor_size_table->{$sortedAttractor} = [$attractor, 0];
	    }
	    
	    for (my $s = 0; $s < $k; $s++) {
	      my $a = $array[$s];
	      unless (exists $is_attractor_table->{$a}) {
		$is_attractor_table->{$a} = $sortedAttractor;
		${$attractor_size_table->{$sortedAttractor}}[1]++;
	      }
	    }
	    last attr;
	  }
	}
      }
      
      push (@array, get_nextstate ($array[-1]));
      
    } # end of while loop
    
  } # end of for loop
  
  return ($is_attractor_table, $attractor_size_table);
}

############################################################

=pod

format_output ();

Returns a hash table containing proper output for this code.

=cut

sub format_output {
  my @attractorsAndSizes = ();

  foreach my $attrAndSize (values %$basinOfAttractors) {
    my %hash = (
		"attractor" => $attrAndSize->[0],
		"size" => $attrAndSize->[1]
	       );
    push (@attractorsAndSizes, \%hash);
    
  }

  my $output = {"output" => [{
			      "type" => "randomSamplingOutput",
			      "fieldCardinality" => $num_states,
			      "numberVariables" => $num_variables,
			      "samplingSize" => $samplingSize,
			      "stateSpaceSize" => $stateSpaceSize,
			      "basinOfAttraction" => \@attractorsAndSizes
			     }]
	       };
  
  my $tempFile = "temp.json";
  my $json = JSON->new->indent ();
  $json = $json->canonical([1]);
  open (OUT," > $tempFile") or die ("<br>ERROR: Cannot open the file for output. <br>");
  print OUT $json->encode ($output);
  close (OUT) or die ("<br>ERROR: Cannot close the file for output. <br>");

  return $tempFile;
}

############################################################

=pod

isnot_number ($n);

Returns true if the input is not a number, false otherwise

=cut

sub isnot_number {
  my $n = shift;

  if ($n =~ m/\D/) {
    return 1;
  }
  else {
    return 0;
  }
}

############################################################

=pod

convert_from_integer_to_state ($n);

Converts the integer representation of a state to state itself as a string

=cut

sub convert_from_integer_to_state {
  my $n = shift;

  my ($quotient, $remainder);
  my @state = ();

  do {
    $quotient = int $n / $num_states;
    $remainder = $n % $num_states;
    push (@state, $remainder);
    $n = $quotient;
  } until ($quotient == 0);

  my $dif = $num_variables - (scalar @state);

  if ($dif) {
    for (my $i = 0; $i < $dif; $i++) {
      push (@state, 0);
    }
  }

  @state = reverse @state;
  my $str_state = join (" ", @state);
  return $str_state;
}

############################################################

# Returns the next state of a given state as a string

sub get_nextstate {
  my $state = shift;
  my @currState = split (" ", $state);

  my @nextState;
  
  for (my $i = 1; $i <= @currState; $i++) {
    my $func = $polyFuncs->{"x$i"};
    my $tt = $tranTables->{"x$i"};

    if (defined $func) {
      my $temp = $func;
      for (my $j = 1; $j <= $num_variables; $j++) {
	$temp =~ s/x[$j]/\($currState[$j - 1]\)/g;
      }
      
      my $ns = eval ($temp) % $num_states;
      push (@nextState, $ns);
    }
    elsif (defined $tt) {

      # TO_DO: use the transition table information if available

    }
    else {
      print ("<br>INTERNAL ERROR: Neither polynomial function or transition table is available as an update rule for x$i. This error must have been detected earlier. <br>");
      exit;
    }
  }
  
  my $str_nextState = join (" ", @nextState);
  return $str_nextState;
}

############################################################
