#Makefile for wizznic.
#To make a system-wide install add DATADIR=/dir/to/data/ and BINDIR=/dir/to/bin/ to the make command

CC = gcc
LD = $(CC)
STRIP = strip

NAME=wizznic
TARGET= $(NAME)

#Add the PER_USER_FILES define if the DATADIR is set
ifneq ($(DATADIR),)
	PUF=-DPER_USER_FILES
endif

DEFS = $(PUF) -DDATADIR="\"$(DATADIR)\""

INCS =  -I. -I/usr/include -I/usr/include/SDL

LDFLAGS=$(CFLAGS)
LIBS = -lSDL -lSDL_image -lSDL_mixer -lpng -lm -lz -lpthread

OBJS = dumplevelimages.o board.o cursor.o draw.o input.o main.o menu.o sprite.o\
text.o ticks.o sound.o game.o player.o list.o levels.o pixel.o stars.o\
levelselector.o leveleditor.o particle.o pack.o settings.o stats.o strings.o\
mbrowse.o teleport.o credits.o waveimg.o userfiles.o

MYCC = $(CC) $(CFLAGS) $(INCS) $(DEFS)

########################################################################

sdl: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)
	$(STRIP) $@

.c.o:
	$(MYCC) -c $< -o $@

install:
	install -d -D -m 755 "$(DESTDIR)$(BINDIR)"
	install -D -m 755 "$(TARGET)" "$(DESTDIR)$(BINDIR)"
	install -d -D -m 755 "$(DESTDIR)$(DATADIR)"
	cp -R data "$(DESTDIR)$(DATADIR)"
	cp -R packs "$(DESTDIR)$(DATADIR)"
	chmod -R 755 "$(DESTIR)$(DATADIR)"

clean:
	rm -f *.o $(NAME)