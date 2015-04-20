#!/usr/bin/perl -w
#
# $Id: test-booldnf.pl,v 1.3 2005/02/03 23:26:43 sarrazip Exp $
#
# Example of bidirectional pipes to talk to the booldnf(1) command
# from a Perl script.
#
# Written by Pierre Sarrazin <http://sarrazip.com/>
# This file is in the public domain.
#

use strict;
use IPC::Open2;


my $command = shift || "booldnf";
my ($reader, $writer);
my $pid = open2($reader, $writer, $command);
if (!defined $pid)
{
    print "open2: $command: $!\n";
    exit 1;
}

while (<>)
{
    print $writer $_;
    my $response = <$reader>;
    print "Response: $response";
}

close $reader;
close $writer;
waitpid $pid, 0;  # see the IPC::Open2 manual page.
