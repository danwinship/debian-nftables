#!/bin/sh

# Run the internal tests of nftables (py)

set -e
cd tests/py
NFT=$(which nft) python3 ./nft-test.py
