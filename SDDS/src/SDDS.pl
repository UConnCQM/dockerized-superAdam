# Authors: Seda Arat & David Murrugarra
# Name: Script for Stochastic Discrete Dynamical Systems (SDDS)
# Revision Date: Feb 17, 2015

#!/usr/bin/perl

use strict;
use warnings;

############################################################
###### REQUIRED PERL MODULES before running the code #######
############################################################

use Getopt::Euclid;
use JSON::Parse;
use JSON;
#use Data::Dumper;

############################################################

 # TO_DO: use the transition table information if available

############################################################

=head1 NAME

perl SDDS.pl - Simulate a model stochastically or deterministically from a possible initialization.

=head1 USAGE

perl SDDS.pl -i <input-file> -o <output-file> -s <seed-number>

=head1 SYNOPSIS

perl SDDS.pl -i <input-file> -o <output-file> -s <seed-number>

=head1 DESCRIPTION

SDDS.pl - Simulate a model stochastically or deterministically from a possible initialization.

=head1 REQUIRED ARGUMENTS

=over

=item -i[nput-file] <input-file>

The JSON file containing the input information (.json). 

=for Euclid:

network-file.type: readable

=item -o[utput-file] <output-file>

The JSON file containing the average trajectories of all variables (.json).

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
my $arguments = $task->{'task'}->{'method'}->{'arguments'}->[0];

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

# sets the number of simulations that the user has specified (scalar)
my $num_simulations = $arguments->{'numberofSimulations'};

# sets the number of steps that the user has specified (scalar)
my $num_steps = $arguments->{'numberofTimeSteps'};

# sets the initial states that the user has specified for simulations (array)
my $initialState = $arguments->{'initialState'};

# sets the propensities (array);
my $propensities = $arguments->{'propensities'};
my $num_propensities = scalar @$propensities;

my $stochFlag = 0;

# upper limits
my $max_num_simulations = 10**6;
my $max_num_steps = 100;

errorCheck ();
my ($polyFuncs, $tranTables) = get_updateRules ();
my $propensitiesTable = get_propensities ();
my $allTrajectories = get_allTrajectories ();
my $averageTrajectories = get_averageTrajectories ();
my $tempFile = format_output ();

system ("cp $tempFile $outputFile");

#system ("M2 ../../share/prettify-json $tempFile $outputFile");

# print Dumper ($allTrajectories);
# print ("\n*********************************\n");
# print Dumper ($averageTrajectories);

exit;

############################################################

############################################################
####################### SUBROUTINES ########################
############################################################

=pod

errorCheck ();

Checks if the user enters the options/parameters correctly and there is any internal errors

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

  #num_propensities
  unless ($numberVariables == $num_propensities) {
    print ("<br>ERROR: There must be propensity entries for all $numberVariables variables. It seems there are propensity entries for $num_propensities variables. <br>");
    exit;
  }

  unless ($numberVariables == scalar @$initialState) {
     print ("<br>ERROR: There must be $numberVariables variables in the initial state. Please check the initial state entry. <br>");
    exit;
  }

  # num_simulations
  if (isnot_number ($num_simulations) || $num_simulations < 1 || $num_simulations > $max_num_simulations) {
    print ("<br>ERROR: The number of simulations must be a number between 1 and $max_num_simulations. <br>");
    exit;
  }

  # num_steps
  if (isnot_number ($num_steps) || $num_steps < 1 || $num_steps > $max_num_steps) {
    print ("<br>ERROR: The number of steps must be a number between 1 and $max_num_steps. <br>");
    exit;
  }

  # propensities
  foreach my $p (@$propensities) {
    my $actProp = $p->{"activation"};
    unless (($actProp >= 0) && ($actProp <= 1)) {
      print ("<br>ERROR: The activation propensities for stochastic simulations must be a number between 0 and 1. For deterministic simulations, the activation propensities must be 1. <br>");
      exit;
    }
    if ($actProp != 1) {
      $stochFlag = 1;
    }

    my $degProp = $p->{"degradation"};
    unless (($degProp >= 0) && ($degProp <= 1)) {
      print ("<br>ERROR: The degradation propensities for stochastic simulations must be a number between 0 and 1. For deterministic simulations, the degradation propensities must be 1. <br>");
      exit;
    }
    if ($degProp != 1) {
      $stochFlag = 1;
    }
  }

  # for deterministic simulations, no need to do more than 1 simulation since they will be all the same
  if ($stochFlag != 1 && $num_simulations != 1) {
    $num_simulations = 1;
    print ("<br>FYI: For deterministic simulations, there is no need to do more than 1 simulation since they will be all the same. <br>");
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

get_propensities ();

Returns a hash table whose keys are the ids and values are a hash table containing activation and degradation propensities

=cut

sub get_propensities {
  my $propensitiesTable = {};

  foreach my $i (@$propensities) {
    my $key = $i->{"id"};
    my $value = {
		 "activation" => $i->{"activation"}, 
		 "degradation" => $i->{"degradation"}
		};
    
    $propensitiesTable->{$key} = $value;
  }
  
  return $propensitiesTable;
}

############################################################

=pod

get_allTrajectories ();

Stores all trajectories into a hash table whose keys are the order of 
the trajectories and the values are the trajectories at the initial state 
and length is num_steps+1.
Returns a reference of the all trajectories hash.

=cut

sub get_allTrajectories {
  my %alltrajectories = ();

  for (my $i = 1; $i <= $num_simulations; $i++) {
    my %table = ();
    my @is = @$initialState;

    for (my $k = 1; $k <= $num_variables; $k++) {
      push (@{$table{"x$k"}}, $is[$k - 1]);
    }
    
    for (my $j = 1; $j <= $num_steps; $j++) {
      my @ns = @{get_nextstate (\@is)};
      
      for (my $r = 1; $r <= $num_variables; $r++) {
		push (@{$table{"x$r"}}, $ns[$r - 1]);
      }
      @is = @ns;
    }
    $alltrajectories{$i} = \%table;
  }
  return \%alltrajectories;
}

############################################################

=pod

get_averageTrajectories ();

Stores average trajectories of all variables into a hash.
Returns a reference of average trajectory hash.

=cut

sub get_averageTrajectories {
  my @averageTrajectories;

  for (my $v = 1; $v <= $num_variables; $v++) {
    my @aveTraj;
    
    for (my $t = 0; $t <= $num_steps; $t++) {
      my $sum = 0;
      
      for (my $s = 1; $s <= $num_simulations; $s++) {
	$sum += $allTrajectories->{$s}->{"x$v"}->[$t];
      }
      
      $aveTraj[$t] = $sum / $num_simulations;
    }
    my %hash = (
		"id" => "x$v",
		"averageTrajectory" => \@aveTraj
	       );
    push (@averageTrajectories, \%hash);
  }
  
  return \@averageTrajectories;
}

############################################################

=pod

format_output ();

Returns a hash table containing proper output for this code.

=cut

sub format_output {
  my $simType = "";
  if ($stochFlag) {
    $simType = "stochastic";
  }
  else {
    $simType = "deterministic";
  }

  my $output = {"output" => [{
			      "type" => $simType . " simulation",
			      "numberVariables" => $num_variables,
			      "numberofTimeSteps" => $num_steps,
			      "numberofSimulations" => $num_simulations,
			      "initialState" => $initialState,
			      "fieldCardinality" => $num_states,
			      "averageTrajectories" => $averageTrajectories
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

get_nextstate ($state);

Returns the next state (as a reference of an array) of a given state 
(as a reference of an array) using update functions and propensity parameters.

=cut

sub get_nextstate {
  my $currentState = shift;
  my $nextState;
  if ($stochFlag) {
    $nextState = get_nextstate_stoch ($currentState);
  }
  else {
    $nextState = get_nextstate_det ($currentState);
  }

  return $nextState;
}

############################################################

=pod

get_nextstate_stoch ($state);

Returns the next state (as a reference of an array) of a given state 
(as a reference of an array) using update functions and propensity parameters.

=cut

sub get_nextstate_stoch {
  my $state = shift;
  my $z = get_nextstate_det ($state);

  my @nextsstateStoch;
  
  for (my $j = 0; $j < $num_variables; $j++) {
    my $r = rand ();
    my $i = $j + 1;
    my $prop = $propensitiesTable->{"x$i"};
    
    if ($state->[$j] < $z->[$j]) {
      if ($r < $prop->{"activation"}) {
 	$nextsstateStoch[$j] = $z->[$j];
      }
      else{
 	$nextsstateStoch[$j] = $state->[$j];
      }
    }
    elsif ($state->[$j] > $z->[$j]) {
      if ($r < $prop->{"degradation"}) {
 	$nextsstateStoch[$j] = $z->[$j];
      }
      else{
 	$nextsstateStoch[$j] = $state->[$j];
      }
    }
    else {
      $nextsstateStoch[$j] = $state->[$j];
    }
  }

  return \@nextsstateStoch;
 }

############################################################

=pod

get_nextstate_det ($state);

Returns the next state (as a reference of an array) of a given state using 
update functions. (propensity parameters are all 1)

=cut

sub get_nextstate_det {
  my $currState = shift;
  my @nextState;
  
  for (my $i = 1; $i <= $num_variables; $i++) {
    my $func = $polyFuncs->{"x$i"};
    my $tt = $tranTables->{"x$i"};

    if (defined $func) {
      my $temp = $func;
      for (my $j = 1; $j <= $num_variables; $j++) {
	$temp =~ s/x[$j]/\($currState->[$j - 1]\)/g;
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
  
  return \@nextState;
}

############################################################
