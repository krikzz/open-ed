GCC	:= e:/devtools/m68k-3.4.6/
CC	:= $(GCC)bin/gcc
LD	:= $(GCC)bin/ld
OBJC	:= $(GCC)bin/objcopy
OUTPUT	:= open-ed
LIB	:= $(GCC)lib
BIN	:= $(GCC)bin
FFDIR	:= ff
TS	:= ./tools/timestamp
SIZE    := $(GCC)bin/size
	
MAKEFLAGS= -j

BDIR		:=build
FLAGS		:= $(OPTION) -std=gnu99 -m68000 -Wall -O3 -fomit-frame-pointer $(INCS)
FLAGSZ80	:= -c -i -x1 -x2 -x3 -z -lnul

vpath %.o $(BDIR)/

SRC_S	= ctr0.s spi.s
SRC_C	= $(wildcard *.c) $(wildcard $(FFDIR)/*.c)

OBJ	= $(SRC_S:.s=.o)
OBJ	+= $(SRC_C:.c=.o)
OFILES	= $(OBJ:%.o=$(BDIR)/%.o)


all: $(OBJ)
	$(LD) -Map=$(BDIR)/memory.map -T rom.ld -nostdlib $(OFILES) $(LIB)/libgcc.a -o $(BDIR)/$(OUTPUT).elf
	$(OBJC) --pad-to 0x1000000 -O binary $(BDIR)/$(OUTPUT).elf $(OUTPUT).md
	$(TS) -f $(OUTPUT).md -a 0x1CC
	@echo "Memory utilization:"
	@$(SIZE) $(BDIR)/$(OUTPUT).elf


%.o: %.c
	$(CC) $(FLAGS) -c $< -o $(basename $(BDIR)/$<).o

%.o: %.s
	$(CC) $(FLAGS) -c $< -o $(basename $(BDIR)/$<).o

clean:
	mkdir -p $(BDIR)
	mkdir -p $(BDIR)/$(FFDIR)
	$(RM) $(BDIR)/*.o
	$(RM) $(BDIR)/*.elf
	$(RM) $(BDIR)/$(FFDIR)/*.o
	$(RM) $(BDIR)/memory.map


