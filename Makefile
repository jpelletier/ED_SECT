#.PHONY: all clean verify

PROJECT = ed_sect
USE_F8087 = -f87

TCPP_DIR = P:\DOS\APPS\PROG\C_CPP\TCPP
TASM_DIR = P:\DOS\APPS\PROG\ASM\TASM

# Variables *_DIR set in C:USERHOOK.BAT
CC = $(TCPP_DIR)\BIN\TCC
LK = $(TCPP_DIR)\BIN\TLINK
ASM = $(TASM_DIR)\TASM.EXE

OPTIONS = -ms -r -M -w -S -B -E$(ASM) -I$(TCPP_DIR)\INCLUDE;..\INC
LK_OPTIONS = /c /x /L$(TCPP_DIR)\LIB;..\LIB
AS_OPTIONS = /l /m2

# Find all .cpp source files
SRCS = $(wildcard *.cpp)
# Define object files based on .cpp files
#OBJS = $(SRCS:.cpp=.obj)
OBJS = $(PROJECT).obj

# Default target: Build the library
all: $(PROJECT).exe

# Rule to compile C files to Object files
.cpp.obj:
	$(CC) $(OPTIONS) -c $<

# Rule to compile ASM files to Object files
.asm.obj:
	$(ASM) $(AS_OPTIONS) -c $<

# Rule to bundle Object files into the Library
$(PROJECT).exe: $(OBJS)
	@echo "Generating TLINK response file..."
	@echo $(TCPP_DIR)\LIB\C0S.OBJ + > link.rsp
	@echo $(OBJS) >> link.rsp
	@echo $(PROJECT).exe >> link.rsp
	@echo NUL >> link.rsp
	@echo ..\LIB\PPIDE.lib + >> link.rsp
	@echo $(TCPP_DIR)\LIB\EMU.LIB + >> link.rsp
	@echo $(TCPP_DIR)\LIB\MATHS.LIB + >> link.rsp
	@echo $(TCPP_DIR)\LIB\CS.LIB >> link.rsp
	@echo "Linking with TLINK..."
	$(LK) $(LK_OPTIONS) @link.rsp
	@del link.rsp
	@echo "Build successful!"

# Target to clean up generated files
clean:
	@del *.obj
	@del *.exe

load:
	cp $(PROJECT).exe /media/jpellet/NANO8088/utils

