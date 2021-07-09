#! /usr/bin/perl
use strict;

my $target_file  = "./save0/ping-targets.txt";
my $target_mtime = "";
my $report_file  = "./save0/ping-reports.txt";
my $ping_cmd     = "";
my @disconnect;

while (1) {
  ping();
}

sub ping {

  open IN, "<$target_file";
  @_ = stat IN;
  my $mtime = localtime($_[9]);
  if ($target_mtime ne $mtime) {
    $target_mtime = $mtime;
    print "[pinger] $target_mtime\n";
    my @args;
    while (<IN>) {
      split;
      push @args, join('.', @_);
    }
    if (($#args + 1) > 0) {
#      $ping_cmd = 'fping -t 100 ' . join(' ', @args);
      $ping_cmd = 'sh data/pings.sh ' . join(' ', @args);
      print "[pinger] $ping_cmd\n";
      @disconnect = ();
    }
  }
  close IN;

  if ($ping_cmd ne '') {
#    open IN, "$ping_cmd 2>/dev/null |";
#    my @tmp;
#    while (<IN>) {
#      unless (/alive$/) {
#        chop;
#        split;
#        $_ = $_[0];
#        s/\./ /g;
#        push @tmp, $_;
#      }
#    }
#    close IN;
#
#    if (@disconnect ne @tmp) {
#      @disconnect = @tmp;
#      open OUT, ">$report_file";
#      print OUT join("\n", @disconnect, "");
#      printf "[pinger] disconnect %s\n", join(", ", @disconnect);
#      close OUT;
#    }
    system $ping_cmd;
    
    select(undef, undef, undef, 0.100);
  }
  else {
    sleep 1;
  }

}

