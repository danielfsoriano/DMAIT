CONTIKI_PROJECT = dmait-client dmait-server
all: $(CONTIKI_PROJECT)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CFLAGS += -ffunction-sections
LDFLAGS += -Wl,--gc-sections,--undefined=_reset_vector__,--undefined=InterruptVectors,--undefined=_copy_data_init__,--undefined=_clear_bss_init__,--undefined=_end_of_init__

#CFLAGS:= -std=c99
#CFLAGS += -ffunction-sections
#LDFLAGS += -Wl,--gc-sections,--undefined=_reset_vector__,--undefined=InterruptVectors,--undefined=_copy_data_init__,--undefined=_clear_bss_init__,--undefined=_end_of_init__

MODULES += os/services/simple-energest

PROJECT_SOURCEFILES += sha256.c
PROJECT_SOURCEFILES += hmac_sha256.c
PROJECT_SOURCEFILES += dmait-logic.c
PROJECT_SOURCEFILES += dmait-client-logic.c
PROJECT_SOURCEFILES += dmait-server-logic.c


CONTIKI=../..
include $(CONTIKI)/Makefile.include
