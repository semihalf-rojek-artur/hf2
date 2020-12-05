ifeq ($(PLATFORM), gcw0)
  CC         := /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc
  STRIP      := /opt/gcw0-toolchain/usr/bin/mipsel-linux-strip
  SYSROOT    := $(shell $(CC) -print-sysroot)
  CFLAGS     := --std=c99 --pedantic -Wall $(shell $(SYSROOT)/usr/bin/sdl-config --cflags)
  LDFLAGS    := $(shell $(SYSROOT)/usr/bin/sdl-config --libs) -lm
endif
ifeq ($(PLATFORM), macos)
  CFLAGS     := $(shell sdl-config --cflags)
endif
ifeq ($(PLATFORM), mingw)
  CC         := i686-w64-mingw32-gcc
  STRIP      := i686-w64-mingw32-strip
  SYSROOT    ?= /usr/i686-w64-mingw32
  CFLAGS     := --std=c99 --pedantic -Wall $(shell $(SYSROOT)/bin/sdl-config --cflags)
  LDFLAGS    := $(shell $(SYSROOT)/bin/sdl-config --libs) -lm
  TARGET     := hf2.exe
endif

SRCDIRS      = .

CC           ?= gcc
STRIP        ?= strip
TARGET       ?= hf2.elf
SYSROOT      ?= $(shell $(CC) -print-sysroot)
MACHINE      ?= $(shell $(CC) -dumpmachine)
DESTDIR      ?= $(SYSROOT)
CFLAGS       ?= --std=c99 --pedantic -Wall $(shell sdl-config --cflags)
LDFLAGS      ?= $(shell sdl-config --libs) -lm
OUTDIR       ?= output/$(MACHINE)

ifdef DEBUG
  CFLAGS += -DDEBUG -ggdb3 -Wall -Wextra
  OUTDIR := $(OUTDIR)-debug
  TARGET := $(TARGET)-debug
else
  CFLAGS += -O2
endif

BINDIR       := $(OUTDIR)/bin
SRCDIR       := src
OBJDIR       := $(OUTDIR)/obj
#IGNORED_FILES:= $(SRCDIR)/
SRC          := $(filter-out $(IGNORED_FILES),$(foreach dir,$(SRCDIRS),$(sort $(wildcard $(addprefix $(SRCDIR)/,$(dir))/*.c))))
OBJ          := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

.PHONY: all clean

all:	$(TARGET)

$(TARGET): $(OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $(BINDIR)/$@
ifndef DEBUG
	$(STRIP) $(BINDIR)/$@
endif

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@ -I include

$(BINDIR) $(OBJDIR):
	mkdir -p $@

clean:
	rm -Rf $(OUTDIR)

