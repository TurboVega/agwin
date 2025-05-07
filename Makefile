# ----------------------------
# Makefile Options
# ----------------------------

NAME = agwin
DESCRIPTION = "Agon Windows"
COMPRESSED = NO

# The RAM_START specifies where agwin is loaded.
# The RAM_SIZE specifies how much RAM agwin will use.
# The end of agwin RAM is equal to RAM_START+RAM_SIZE.
# Using these particular values, agwin uses a little
# less than 32 KB for code and static data, and a
# little more than 64 KB for stack and heap. The total
# RAM used equals 96 KB. The RAM_END reserves the
# space used by MOS.
#
RAM_START = 0x040000
RAM_SIZE  = 0x018000
RAM_END   = 0x0BC000

LDHAS_ARG_PROCESSING = 0
LDHAS_EXIT_HANDLER = 0

include $(shell agondev-config --makefile)

CFLAGS += -Wall -Wextra -Oz -DAGWIN_APP -DRAM_START=$(RAM_START) -DRAM_SIZE=$(RAM_SIZE) -DRAM_END=$(RAM_END)
CXXFLAGS += -Wall -Wextra -Oz -DAGWIN_APP -DRAM_START=$(RAM_START) -DRAM_SIZE=$(RAM_SIZE) -DRAM_END=$(RAM_END)
