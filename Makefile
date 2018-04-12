#
# Makefile
# Adrian Perez, 2015-08-25 11:58
#

CFLAGS ?= -Os -Wall

PKG_MODULES   := webkit2gtk-4.0 webkit2gtk-web-extension-4.0
WEB_EXT_FLAGS := $(shell pkg-config ${PKG_MODULES} --cflags) $(shell perl -MExtUtils::Embed -MGlib::Install::Files -e 'ccopts ; print "-I$$Glib::Install::Files::CORE"')
WEB_EXT_LIBS  := $(shell pkg-config ${PKG_MODULES} --libs) $(shell perl -MExtUtils::Embed -e ldopts)

CPPFLAGS += ${WEB_EXT_FLAGS}
LDLIBS   += ${WEB_EXT_LIBS}

all: loader.so

loader.so: loader.o
	${CC} ${LDFLAGS} -fPIC -shared -o $@ $^ ${LDLIBS}
loader.so: CFLAGS += -fPIC

clean:
	${RM} loader.o loader.so

# vim:ft=make
#
