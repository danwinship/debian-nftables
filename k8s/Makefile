# Copyright 2022 The Kubernetes Authors.
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

.PHONY:	build all all-build clean setup-multiarch cleanup-multiarch

IMAGE=nftables-build

ARCH?=amd64
ALL_ARCH = amd64 arm arm64 ppc64le s390x

QEMUVERSION=7.2.0-1
QEMUIMAGE ?= docker.io/multiarch/qemu-user-static

SUDO=$(if $(filter 0,$(shell id -u)),,sudo)

all: all-build

setup-multiarch:
	$(SUDO) podman run --rm --privileged $(QEMUIMAGE):$(QEMUVERSION) --reset -p yes

cleanup-multiarch:
	$(SUDO) podman run --rm --privileged $(QEMUIMAGE):$(QEMUVERSION) --reset

build:
	podman build \
		--platform linux/$(ARCH) \
		--output type=local,dest=. \
		-f Dockerfile \
		../

sub-build-%:
	$(MAKE) ARCH=$* build

all-build: setup-multiarch $(addprefix sub-build-,$(ALL_ARCH))

clean:
	rm -f *.deb
