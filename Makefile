.POSIX:
.SUFFIXES:
.PHONY: default all clean install uninstall

RM = rm -f
CP = cp -a
LNS = ln -sf
MKDIR = mkdir -p
PRINTF = printf

CFLAGS = -W -Wall -O3
LDFLAGS = -s
ARFLAGS = -rcs

PREFIX = /usr/local
BINDIR = bin
LIBDIR = lib
INCDIR = include
PKGDIR = $(LIBDIR)/pkgconfig

NAME = freeaptx
MAJOR = 0
MINOR = 1
PATCH = 1

LIBNAME = lib$(NAME).so
SONAME = $(LIBNAME).$(MAJOR)
SOFILENAME = $(SONAME).$(MINOR).$(PATCH)
PCNAME = lib$(NAME).pc

UTILITIES = $(NAME)enc $(NAME)dec
STATIC_UTILITIES = $(NAME)enc.static $(NAME)dec.static

HEADERS = $(NAME).h
SOURCES = $(NAME).c
AOBJECTS = $(NAME).o
IOBJECTS = $(NAME)enc.o $(NAME)dec.o

BUILD = $(SOFILENAME) $(SONAME) $(LIBNAME) $(IOBJECTS) $(UTILITIES)

default: $(SOFILENAME) $(SONAME) $(LIBNAME) $(UTILITIES) $(HEADERS)

all: $(BUILD)

clean:
	$(RM) $(BUILD)

install: default
	$(MKDIR) $(DESTDIR)$(PREFIX)/$(LIBDIR)
	$(CP) $(SOFILENAME) $(SONAME) $(LIBNAME) $(DESTDIR)$(PREFIX)/$(LIBDIR)
	$(MKDIR) $(DESTDIR)$(PREFIX)/$(BINDIR)
	$(CP) $(UTILITIES) $(DESTDIR)$(PREFIX)/$(BINDIR)
	$(MKDIR) $(DESTDIR)$(PREFIX)/$(INCDIR)
	$(CP) $(HEADERS) $(DESTDIR)$(PREFIX)/$(INCDIR)
	$(MKDIR) $(DESTDIR)$(PREFIX)/$(PKGDIR)
	$(PRINTF) 'prefix=%s\nexec_prefix=$${prefix}\nlibdir=$${exec_prefix}/%s\nincludedir=$${prefix}/%s\n\n' $(PREFIX) $(LIBDIR) $(INCDIR) > $(DESTDIR)$(PREFIX)/$(PKGDIR)/$(PCNAME)
	$(PRINTF) 'Name: lib%s\nDescription: Open Source aptX codec library\nVersion: %u.%u.%u\n' $(NAME) $(MAJOR) $(MINOR) $(PATCH) >> $(DESTDIR)$(PREFIX)/$(PKGDIR)/$(PCNAME)
	$(PRINTF) 'Libs: -L$${libdir} -l%s\nCflags: -I$${includedir}\n' $(NAME) >> $(DESTDIR)$(PREFIX)/$(PKGDIR)/$(PCNAME)

uninstall:
	for f in $(SOFILENAME) $(SONAME) $(LIBNAME); do $(RM) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$$f; done
	for f in $(UTILITIES); do $(RM) $(DESTDIR)$(PREFIX)/$(BINDIR)/$$f; done
	for f in $(HEADERS); do $(RM) $(DESTDIR)$(PREFIX)/$(INCDIR)/$$f; done
	$(RM) $(DESTDIR)$(PREFIX)/$(PKGDIR)/$(PCNAME)

$(UTILITIES): $(LIBNAME)

$(IOBJECTS): $(HEADERS)

$(LIBNAME): $(SONAME)
	$(LNS) $(SONAME) $@

$(SONAME): $(SOFILENAME)
	$(LNS) $(SOFILENAME) $@

$(SOFILENAME): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -I. -shared -fPIC -Wl,-soname,$(SONAME) -o $@ $(SOURCES)

.SUFFIXES: .o .c .static

.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBNAME)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -I. -c -o $@ $<
