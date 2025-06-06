# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2018 Intel Corporation

# binary name
APP = fips_validation

# all source are stored in SRCS-y
SRCS-y := fips_validation.c
SRCS-y += fips_validation_aes.c
SRCS-y += fips_validation_hmac.c
SRCS-y += fips_validation_tdes.c
SRCS-y += fips_validation_gcm.c
SRCS-y += fips_validation_cmac.c
SRCS-y += fips_validation_ccm.c
SRCS-y += fips_validation_sha.c
SRCS-y += fips_dev_self_test.c
SRCS-y += fips_validation_xts.c
SRCS-y += fips_validation_rsa.c
SRCS-y += fips_validation_ecdsa.c
SRCS-y += fips_validation_eddsa.c
SRCS-y += main.c

PKGCONF ?= pkg-config

# Build using pkg-config variables if possible
ifneq ($(shell $(PKGCONF) --exists libdpdk && echo 0),0)
$(error "no installation of DPDK found")
endif

all: shared
.PHONY: shared static
shared: build/$(APP)-shared
	ln -sf $(APP)-shared build/$(APP)
static: build/$(APP)-static
	ln -sf $(APP)-static build/$(APP)

PC_FILE := $(shell $(PKGCONF) --path libdpdk 2>/dev/null)
CFLAGS += -O3 $(shell $(PKGCONF) --cflags libdpdk)
LDFLAGS_SHARED = $(shell $(PKGCONF) --libs libdpdk)
LDFLAGS_STATIC = $(shell $(PKGCONF) --static --libs libdpdk)

JANSSON := $(shell $(PKGCONF) --exists jansson; echo $$?)
ifeq ($(JANSSON), 0)
LDFLAGS += $(shell $(PKGCONF) --libs jansson)
CFLAGS += -DUSE_JANSSON
endif

CFLAGS += -DALLOW_EXPERIMENTAL_API

build/$(APP)-shared: $(SRCS-y) Makefile $(PC_FILE) | build
	$(CC) $(CFLAGS) $(SRCS-y) -o $@ $(LDFLAGS) $(LDFLAGS_SHARED)

build/$(APP)-static: $(SRCS-y) Makefile $(PC_FILE) | build
	$(CC) $(CFLAGS) $(SRCS-y) -o $@ $(LDFLAGS) $(LDFLAGS_STATIC)

build:
	@mkdir -p $@

.PHONY: clean
clean:
	rm -f build/$(APP) build/$(APP)-static build/$(APP)-shared
	test -d build && rmdir -p build || true
