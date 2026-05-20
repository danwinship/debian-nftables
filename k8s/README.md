# Kubernetes pkg-nftables

This is a fork of
https://salsa.debian.org/pkg-netfilter-team/pkg-nftables with
additional patches and build scripts added to the `bookworm` branch,
to build the version of `nft` that we use for the Kubernetes
`distroless-iptables` image (which in turn is used as the base for the
official `kube-proxy` image).

See ../debian/changelog for details about additional patches relative
to the original Debian package.
