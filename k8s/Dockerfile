# Copyright 2026 The Kubernetes Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

FROM debian:bookworm-slim as build
ARG PKGNAME=nftables_1.0.6

RUN apt-get -y update && \
    apt-get -y dist-upgrade && \
    apt-get -y install --no-install-recommends \
        build-essential devscripts debhelper dh-make \
        bison flex libeditreadline-dev libgmp-dev libjansson-dev libmnl-dev libnftnl-dev libxtables-dev
WORKDIR /build/src
ENV DEBFULLNAME="Dan Winship"
ENV DEBEMAIL=danwinship@redhat.com
COPY . .
ADD --checksum=sha256:2407430ddd82987670e48dc2fda9e280baa8307abec04ab18d609df3db005e4c http://deb.debian.org/debian/pool/main/n/nftables/nftables_1.0.6.orig.tar.xz /build
RUN debuild -us -uc

FROM scratch
COPY --from=build /build/nftables_*.deb .
COPY --from=build /build/libnftables1_*.deb .