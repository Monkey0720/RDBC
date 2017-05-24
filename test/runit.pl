#!/usr/bin/env perl
use strict;
use English;

my $proggy = $ARGV[0];
my $workdir = $ARGV[3];
my $curdir = $ARGV[2];
my $bin_dir = $ARGV[1];

if(0){
print "prog = $proggy\n";
print "workdir = $workdir\n";
print "curdir = $curdir\n";
print "bin_dir = $bin_dir\n";

}


my @infiles = `ls $curdir/$proggy.in*`;
my $file;
my $cnt = 1;
foreach $file (@infiles) {
  chomp $file;
  #print "file is $file\n";
  my @parts = split('/',$file);
  my $infile = $parts[$#parts];
  my $outfile = $infile;
  $outfile =~s/\.in/.out/;
  my @opts = `cat $file`;
  my $opts;
  my $line;
  foreach $line(@opts){
    next if $line=~/^\#/;
    next if $line=~/^\s+$/;
    $opts = $opts.$line;
  }
  chomp $opts;
  my $cmd = "cd $bin_dir ; $proggy $opts &> $workdir/$outfile";
  print "<**executing**> $proggy $opts";
  `$cmd`;

  if($CHILD_ERROR){   
    print "\nWARNING: $proggy with options '$opts' exited with signal " ,( $CHILD_ERROR >> 8) ;
  }

  if($workdir ne $curdir){
    my @diff = `diff $workdir/$outfile $curdir/$outfile`;
    if($#diff == -1){
      print ".... PASSED\n";
    }else{
      print ".... FAILED\n";
      print "--------------------------------------------\n";
      print " < what test program did \n";
      print " > expected output \n";
      print "--------------------------------------------\n";
      print @diff;
    }
  }else{
    print "\n";
  }
}
