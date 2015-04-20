#!/usr/bin/perl -w

use strict;
use IPC::Open2;

$| = 1;  # no buffering on STDOUT

my $numErrors = 0;

if (@ARGV < 1)
{
    print STDERR "Expected name of program as argument.\n";
    exit 1;
}

my $checkdnf = shift || die;

my ($reader, $writer);
my $pid = open2($reader, $writer, $checkdnf);
if (!defined $pid)
{
    print STDERR "Error when starting $checkdnf\n";
    exit 1;
}

while (<>)
{
    chomp;
    s/\s+//g;
    next if /^\s*$/ || /^\s*\#/;
    last if $_ eq "END";

    if (/[^a-zA-Z\|\&\!\(\)\=\=]/)
    {
	print STDERR "Invalid char in line $.\n";
	exit 1;
    }
    if (/^([^=]+)==([^=]+)$/)
    {
	testEquality($1, $2);
    }
    elsif (/^([^=]+)$/)
    {
	testEquality($1, $1);
    }
    else
    {
	print STDERR "Invalid char in line $.\n";
	exit 1;
    }
}

print "\n";
if ($numErrors == 0)
{
    print "$0: Success.\n";
    exit 0;
}
print "$0: $numErrors error(s)\n";
exit 1;


sub testEquality
{
    my ($expr, $dnfExpr) = @_;

    print "Line $.: checking if DNF($expr) == $dnfExpr: ";

    print $writer "$1\n";
    my $response = "";
    while (<$reader>)
    {
	chomp;
	last if $_ eq ".";
	$response .= $_;
    }

    if (normalize($response) eq normalize($dnfExpr))
    {
	print "yes\n";
    }
    else
    {
	print "NO\n\t", "Received: \"$response\"\n";
	$numErrors++;
    }
}


sub normalize
{
    my ($expr) = @_;
    my @terms = split("|", $expr);
    my @normTerms = ();
    my $t;
    foreach $t (@terms)
    {
	my @factors = split("&", $t);
	my $normT = join("&", sort @factors);
	push @normTerms, $normT;
    }
    $expr = join("|", sort @normTerms);
    return $expr;
}
