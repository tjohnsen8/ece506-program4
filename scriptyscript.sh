#!bin/bash

OUTN=1
for protocol in 0 1
do
for cachesize in 32000 64000 128000 256000 512000
do
 echo "./dsm smp $cachesize 4 64 16 $protocol swaptions_truncated" > real.$OUTN
 ./dsm $cachesize 4 64 16 $protocol swaptions_truncated >> real.$OUTN
 echo "./simulate_cache_ref smp $cachesize 4 64 16 $protocol swaptions_truncated" > ref.$OUTN
 ./simulate_cache_ref $cachesize 4 64 16 $protocol swaptions_truncated >> ref.$OUTN
 diff real.$OUTN ref.$OUTN > diff.$OUTN
 OUTN=$(($OUTN+1))
done
for blocksize in 64 128 256
do
 echo "./dam smp 256000 1 $blocksize 16 $protocol swaptions_truncated" > real.$OUTN
 ./dam 1000000 4 $blocksize 16 $protocol swaptions_truncated >> real.$OUTN
 echo "./simulate_cache smp 256000 1 $blocksize 16 $protocol swaptions_truncated" > ref.$OUTN
 ./simulate_cache_ref 1000000 4 $blocksize 16 $protocol swaptions_truncated >> ref.$OUTN
 diff real.$OUTN ref.$OUTN > diff.$OUTN
 OUTN=$(($OUTN+1))
done
done