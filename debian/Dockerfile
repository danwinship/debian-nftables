FROM debian:bookworm-slim as build

RUN apt -y update && \
    apt -y dist-upgrade && \
    apt -y install build-essential devscripts debhelper dh-make \
        asciidoc-base bison dh-python docbook-xsl flex libeditreadline-dev libgmp-dev libjansson-dev libmnl-dev libnftnl-dev libxtables-dev python3-all:any

WORKDIR /src
ENV DEBFULLNAME="Dan Winship"
ENV DEBEMAIL=danwinship@redhat.com
COPY . .
RUN dh_make --createorig --addmissing -p nftables_1.0.6 -s -y && \
    debuild -us -uc
