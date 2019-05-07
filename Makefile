
.SUFFIXES: .c .o .obj .dbg.obj

CLOC=cloc
RM=rm -f

# Kobe
KOBE_OUT=kobe.exe
KOBE_OUT_DBG=kobe-dbg.exe
KOBE_CC=wcc
KOBE_LINK=wlink

KOBE_FILES=./src/kobe.c \
            ./src/util.c \
            ./src/fixed.c \
            ./src/input.c \
            ./src/time.c \
            ./src/palette.c \
            ./src/image.c \
            ./src/render.c \
            ./src/t_backgr.c \
            ./src/t_scroll.c \
            ./src/t_sprite.c

KOBE_CFLAGS=-d3 -db -bt=dos -3 -wx -zq -ob -oh -ol -ol+ -or -ot -oe -oi -s
KOBE_LFLAGS=@lflags
KOBE_LFLAGS_DBG=debug dwarf option map @lflags

kobe-clean:
	$(RM) $(KOBE_OUT)
	$(RM) $(KOBE_OUT_DBG)
	$(RM) $(KOBE_FILES:.c=.o)
	$(RM) $(KOBE_FILES:.c=.mbr)
	$(RM) *.err
	$(RM) *.map
	$(RM) "LOG.TXT"

# SDK
SDK_OUT=sdk
SDK_OUT_DBG=sdk-dbg
SDK_CC=cc

SDK_FILES=./src-sdk/sdk.c \
          ./src-sdk/kimage.c \
          ./src-sdk/kpalette.c \
          ./src-sdk/indexed-image.c

SDK_CFLAGS=-c -O3 -mtune=generic
SDK_CFLAGS_DBG=-c -std=c11 -O0 -Wall -Wextra -pedantic -g -DDEV
SDK_LFLAGS=-lpng

sdk-clean:
	$(RM) $(SDK_FILES:.c=.obj)
	$(RM) $(SDK_FILES:.c=.dbg.obj)
	$(RM) $(SDK_OUT)
	$(RM) $(SDK_OUT_DBG)

# Resources
resources: sdk
	./$(SDK_OUT) palette -i "./res/palette.png"
	./$(SDK_OUT) image -i "./res/button_a.png"
	./$(SDK_OUT) image -i "./res/button_b.png"
	./$(SDK_OUT) image -a columns -i "./res/t_bkgfar.png"
	./$(SDK_OUT) image -a columns -i "./res/t_bkghug.png"
	./$(SDK_OUT) image -a columns -i "./res/bkg01.png"
	./$(SDK_OUT) image -a planes --frame 64x64 -i "./res/bcrate02.png"
	./$(SDK_OUT) image -a planes --frame 64x64 -i "./res/bcrate02-r.png"

resources-dbg: sdk-dbg
	./$(SDK_OUT_DBG) palette -i "./res/palette.png"
	./$(SDK_OUT_DBG) image -i "./res/button_a.png"
	./$(SDK_OUT_DBG) image -i "./res/button_b.png"
	./$(SDK_OUT_DBG) image -a columns -i "./res/t_bkgfar.png"
	./$(SDK_OUT_DBG) image -a columns -i "./res/t_bkghug.png"
	./$(SDK_OUT_DBG) image -a columns -i "./res/bkg01.png"
	./$(SDK_OUT_DBG) image -a planes --frame 64x64 -i "./res/bcrate02.png"
	./$(SDK_OUT_DBG) image -a planes --frame 64x64 -i "./res/bcrate02-r.png"

resources-clean:
	$(RM) res/*.ki
	$(RM) res/*.kp
	$(RM) res/*.kf
	$(RM) res/*.kl


###


default: release
all: release debug resources
clean: kobe-clean sdk-clean resources-clean
release: kobe sdk resources
debug: kobe-dbg sdk-dbg resources-dbg

.c.o:
	$(KOBE_CC) $(KOBE_CFLAGS) $< -fo=$@

kobe: $(KOBE_FILES:.c=.o)
	$(KOBE_LINK) $(KOBE_LFLAGS) name $(KOBE_OUT)

kobe-dbg: $(KOBE_FILES:.c=.o)
	$(KOBE_LINK) $(KOBE_LFLAGS_DBG) name $(KOBE_OUT_DBG)

.c.obj:
	$(SDK_CC) $(SDK_CFLAGS) $< -o $@

.c.dbg.obj:
	$(SDK_CC) $(SDK_CFLAGS_DBG) $< -o $@

sdk: $(SDK_FILES:.c=.obj)
	$(SDK_CC) $(SDK_FILES:.c=.obj) $(SDK_LFLAGS) -o $(SDK_OUT)

sdk-dbg: $(SDK_FILES:.c=.dbg.obj)
	$(SDK_CC) $(SDK_FILES:.c=.dbg.obj) $(SDK_LFLAGS) -o $(SDK_OUT_DBG)
