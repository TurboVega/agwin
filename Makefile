# ----------------------------
# Makefile Options
# ----------------------------

NAME = agwin
DESCRIPTION = "Agon Windows"
COMPRESSED = NO
LDHAS_ARG_PROCESSING = 0

BSSHEAP_LOW = 060000
BSSHEAP_HIGH = 09FFFF

CFLAGS = -Wall -Wextra -Oz -DAGWIN_APP
CXXFLAGS = -Wall -Wextra -Oz -DAGWIN_APP


BITMAPS = $(call NATIVEPATH,/images/*)
BITMAPS_DEST = $(call NATIVEPATH,$(BINDIR)/bitmaps)

#Unsure why the copy of this in makefile.mk doesn't cover this already...
ifeq ($(OS),Windows_NT)
COPYDIR ?= ( xcopy $1 $2 /S /Q /Y /I /K 1>nul 2>nul || call )
else
COPYDIR ?= cp -r $1 $2
endif

# ----------------------------

include $(shell cedev-config --makefile)
