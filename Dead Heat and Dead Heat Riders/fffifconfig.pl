#! /usr/bin/perl
# $Id: ifconfig.pl 8622 2007-04-22 04:23:14Z ykaneda $
# ifconfig の出力を デバイス名 アドレス ネットマスク MACアドレス フォーマットに整形する
# 例) eth0 192 168 0 1 255 255 255 0 0 0 0 0 0 0

use strict;
use utf8;

open IN, "sudo ifconfig|" or die "cant open $!";

my ($get, $interface, @address, @mask, @mac);

$interface = "0";
@mac = (0, 0, 0, 0, 0, 0);
@address = (0, 0, 0, 0);
@mask = (255, 255, 255, 0);

while (<IN>) {
  chop;
  if (/^eth([0-9])/) {
    $interface = $1;
    $get = 1;
  }
  if (/^lo/) {
    $get = 0;
  }
  if ($get && /HWaddr ([0-9A-Fa-f:]+)/) {
    $_ = $1;
    
    split /:/;
    @mac = @_;
    for (my $i = 0; $i < $#mac + 1; ++$i) {
      $mac[$i] = hex($mac[$i]);
      
    }
    
  } elsif ($get && /inet addr:([0-9\.]*) .+Mask:([0-9\.]+)$/) {
    $_ = $1;
    split /\./;
    @address = @_;
    $_ = $2;
    split /\./;
    @mask = @_;
    last; # コメントの場合last コメント無しの場合first
  }
}
close IN;

open IN, "sudo ethtool eth$interface |" or die "cant open $!";
my $link = 0;
while (<IN>) {
  if (/Link detected: yes/) {
    $link = 1;
    last;
  }
}

print "$interface @address @mask @mac $link\n";

