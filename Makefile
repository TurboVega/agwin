# ----------------------------
# Makefile Options
# ----------------------------

NAME = agwin
DESCRIPTION = "Agon Windows"
COMPRESSED = NO

RAM_START = 0x040000
RAM_SIZE  = 0x030000

LDHAS_ARG_PROCESSING = 0
LDHAS_EXIT_HANDLER = 0

include $(shell agondev-config --makefile)

CFLAGS += -Wall -Wextra -Oz -DAGWIN_APP
CXXFLAGS += -Wall -Wextra -Oz -DAGWIN_APP
