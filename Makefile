ESDK = $(EPIPHANY_HOME)

HOST = host
EPI = epi
EPI_SREC = $(EPI).srec
HOST_SRC = $(HOST).c
EPI_SRC = $(EPI).c
HEADERS = common.h

CFLAGS = -I$(ESDK)/tools/host/include
LDFLAGS = -L$(ESDK)/tools/host/lib
ECFLAGS = -O2 -DNOP_COUNT=$(NOP_COUNT)
ELDFLAGS = -T $(ESDK)/bsps/current/fast.ldf -le-lib
NOP_COUNT ?= 0

EGCC ?= e-gcc
EOBJCOPY ?= e-objcopy --srec-forceS3 --output-target srec
CC ?= gcc
RM = rm -f

.PHONY: all

all: $(HOST) $(EPI_SREC)

$(HOST): $(HOST).c $(HEADERS)
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS) -le-hal -le-loader -lpthread

$(EPI): $(EPI).c $(HEADERS)
	$(EGCC) -o $@ $(ECFLAGS) $< $(ELDFLAGS) -le-lib

$(EPI_SREC): $(EPI)
	$(EOBJCOPY) $< $@

.PHONY: clean

clean:
	$(RM) $(HOST) $(EPI) $(EPI_SREC)
