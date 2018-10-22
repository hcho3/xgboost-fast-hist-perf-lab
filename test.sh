#!/bin/bash

set -e

# Run 5 trials with different nthread

for i in 1 3 5 9 12 18 36
do
  for k in {1..5}
  do
    ./perflab record ${i}
  done
done
