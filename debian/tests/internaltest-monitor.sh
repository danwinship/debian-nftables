#!/bin/sh

# Run the internal tests of nftables (monitor)

set -e
ln -s $(which nft) src/nft
cd tests/monitor
./run-tests.sh -d
