PRISMAC   = prismac
LIB_PRIS  = args
PATH_PRIS = src/
SRC       = $(PATH_PRIS)$(LIB_PRIS).pris
OUT       = ini.pris

ifeq ($(OS),Windows_NT)
	PLATFORM = windows
	PREFIX  ?= C:/Prisma/1.0/bin/plibs
	DESTDIR ?= $(PREFIX)/$(LIB_PRIS)
else
	PLATFORM = unix
	PREFIX  ?= /usr/local/share/prisma/1.0/plib
	DESTDIR ?= $(PREFIX)/$(LIB_PRIS)
endif

all: $(OUT)

$(OUT): $(SRC)
	$(PRISMAC) -o $(OUT) $(SRC)

doc:
	cd $(PATH_PRIS) && minidoc $(LIB_PRIS).pris && mv doc ../

install: all
ifeq ($(PLATFORM),unix)
	install -d "$(DESTDIR)"
	install -m 644 $(OUT) "$(DESTDIR)/$(OUT)"
endif
ifeq ($(PLATFORM),windows)
	if not exist "$(DESTDIR)" mkdir "$(DESTDIR)"
	copy /Y $(OUT) "$(DESTDIR)\$(OUT)"
endif

uninstall:
ifeq ($(PLATFORM),unix)
	rm -rf "$(DESTDIR)"
endif
ifeq ($(PLATFORM),windows)
	if exist "$(DESTDIR)" rmdir /S /Q "$(DESTDIR)"
endif

clean:
	rm -f $(OUT)
	rm -rf doc

.PHONY: all clean install uninstall doc
