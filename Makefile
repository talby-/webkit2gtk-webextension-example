#
# Makefile
# Adrian Perez, 2015-08-25 11:58
# Robert Stone, 2016-03-14
#

CFLAGS ?= -Os -Wall

PKG_MODULES   := webkit2gtk-web-extension-4.0
WEB_EXT_FLAGS := $(shell pkg-config ${PKG_MODULES} --cflags)
WEB_EXT_LIBS  := $(shell pkg-config ${PKG_MODULES} --libs)

CPPFLAGS += ${WEB_EXT_FLAGS} $(shell perl -MExtUtils::Embed -MGlib::Install::Files -e 'ccopts ; print "-I$$Glib::Install::Files::CORE"')
LDLIBS   += ${WEB_EXT_LIBS} $(shell perl -MExtUtils::Embed -e ldopts)

all: loader.so

loader.so: loader.o
	${CC} ${LDFLAGS} -fPIC -shared -o $@ $^ ${LDLIBS}
loader.so: CFLAGS += -fPIC

clean:
	${RM} loader.o loader.so

# vim:ft=make
#
