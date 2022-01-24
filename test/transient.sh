#!/bin/bash

set -e

./transient ./ssparse ./test/fattree_iq_blast.mpf.gz ./fattree_iq_blast.csv

python3 <<EOF
import handycsv
csv = handycsv.GridStats.read("./fattree_iq_blast.csv")
assert csv.head() == "Time"
EOF
