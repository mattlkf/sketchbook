
# An attempt at a simple makefile that allows the use of Arduino libraries.
# Unlike edam's Makefile, this script does only the essential.
# It does not try too hard to auto-fill values or guess things for the user
# unless I deem it useful (e.g. finding a TTY)

# Requirements:
# If you want to use a new board type, update the boards.txt file

# Matt, Feb 2017

# Credit:
# 	edam's Makefile
# 	Pius's ionforge

#_______________________________________________________________________________
#                                                 Values that need configuration

ARDUINODIR := ~/Misc/arduino-1.8.1
SKETCHBOOKDIR := ~/git/sketchbook

# default arduino version
ARDUINOCONST ?= 100

#_______________________________________________________________________________
#																						Check for values in project Makefile

ifndef BOARD
$(error BOARD is unset.)
endif

# If Target not set
TARGET ?= a.out

#_______________________________________________________________________________
#																	Generate paths to certain files / directories

# Where the arduino core files are found
ARDUINOCOREDIR := $(ARDUINODIR)/hardware/arduino/avr/cores/arduino

# A custom boards file we must maintain
BOARDSFILE := $(SKETCHBOOKDIR)/boards.txt

# Search for libraries installed in sketchbook or in Arduino directories. If there are multiple libraries with the same name, choose the first occuring one.
LIBRARYPATH ?= $(SKETCHBOOKDIR)/libraries $(ARDUINODIR)/libraries $(ARDUINODIR)/hardware/arduino/avr/libraries

# Path for avr tools
AVRTOOLSPATH ?= $(subst :, , $(PATH)) $(ARDUINODIR)/hardware/tools \
	$(ARDUINODIR)/hardware/tools/avr/bin

# A simplified source-file search
SOURCES := $(wildcard *.c *.cc *.cpp)
$(info Sources: $(SOURCES))

# Generate list of libraries that were #include-d by source files
INCLUDED_LIBS := $(shell sed -ne "s/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p" $(SOURCES))
$(info Included libraries: $(INCLUDED_LIBS))

# Generate list of library directories
AVAILABLE_LIBS := $(notdir $(wildcard $(addsuffix /*, $(LIBRARYPATH))))
# $(info Available libraries: $(AVAILABLE_LIBS))

LIBS := $(filter $(AVAILABLE_LIBS), $(INCLUDED_LIBS))
$(info Will compile: $(LIBS))

# Find the actual paths to the libraries (choose first occuring)
LIBRARYDIRS_PLAIN := $(foreach lib, $(LIBS), \
	$(firstword $(wildcard $(addsuffix /$(lib), $(LIBRARYPATH)))))

# Let the entire set of library dirs include sub-directories "src" and "utility"
LIBRARYDIRS += $(LIBRARYDIRS_PLAIN)
LIBRARYDIRS += $(addsuffix /src, $(LIBRARYDIRS_PLAIN))
LIBRARYDIRS += $(addsuffix /utility, $(LIBRARYDIRS_PLAIN))
LIBRARYDIRS += $(addsuffix /src/utility, $(LIBRARYDIRS_PLAIN))

$(info Librarydirs: $(LIBRARYDIRS))

# software
findsoftware = $(firstword $(wildcard $(addsuffix /$(1), $(AVRTOOLSPATH))))
CC := $(call findsoftware,avr-gcc)
CXX := $(call findsoftware,avr-g++)
LD := $(call findsoftware,avr-ld)
AR := $(call findsoftware,avr-ar)
OBJCOPY := $(call findsoftware,avr-objcopy)
AVRDUDE := $(call findsoftware,avrdude)
AVRSIZE := $(call findsoftware,avr-size)

# figure out which arg to use with stty (for OS X, GNU and busybox stty)
STTYFARG := $(shell stty --help 2>&1 | \
	grep -q 'illegal option' && echo -f || echo -F)

$(info CC: $(CC))

#_______________________________________________________________________________
#																															Guess a few things

TTYGUESS := 0
ifndef TTY
TTY := $(firstword $(wildcard \
	/dev/ttyACM? /dev/ttyUSB? /dev/tty.usbserial* /dev/tty.usbmodem*))
TTYGUESS := 1
endif

#_______________________________________________________________________________
#																													 Read from boards file

readboardsparam = $(shell sed -ne "s/$(BOARD).$(1)=\(.*\)/\1/p" $(BOARDSFILE))
BOARD_BUILD_MCU := $(call readboardsparam,build.mcu)
BOARD_BUILD_FCPU := $(call readboardsparam,build.f_cpu)
BOARD_BUILD_VARIANT := $(call readboardsparam,build.variant)
BOARD_UPLOAD_SPEED := $(call readboardsparam,upload.speed)
BOARD_UPLOAD_PROTOCOL := $(call readboardsparam,upload.protocol)
BOARD_USB_VID := $(call readboardsparam,build.vid)
BOARD_USB_PID := $(call readboardsparam,build.pid)
BOARD_BOOTLOADER_UNLOCK := $(call readboardsparam,bootloader.unlock_bits)
BOARD_BOOTLOADER_LOCK := $(call readboardsparam,bootloader.lock_bits)
BOARD_BOOTLOADER_LFUSES := $(call readboardsparam,bootloader.low_fuses)
BOARD_BOOTLOADER_HFUSES := $(call readboardsparam,bootloader.high_fuses)
BOARD_BOOTLOADER_EFUSES := $(call readboardsparam,bootloader.extended_fuses)
BOARD_BOOTLOADER_PATH := $(call readboardsparam,bootloader.path)
BOARD_BOOTLOADER_FILE := $(call readboardsparam,bootloader.file)


#_______________________________________________________________________________
#																										  Check that board was valid

ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "cl"
ifeq "$(BOARD_BUILD_MCU)" ""
$(error BOARD=$(BOARD) is invalid.)
endif
endif
endif

#_______________________________________________________________________________
#																										  				 Prepare for build

OBJECTS := $(addsuffix .o, $(basename $(SOURCES)))
DEPFILES := $(patsubst %, .dep/%.dep, $(SOURCES))
ARDUINOLIB := .lib/arduino.a
ARDUINOLIBOBJS := $(foreach dir, $(ARDUINOCOREDIR) $(LIBRARYDIRS), \
	$(patsubst %, .lib/%.o, $(wildcard $(addprefix $(dir)/, *.c *.cpp))))

# avrdude confifuration
ifeq "$(AVRDUDECONF)" ""
ifeq "$(AVRDUDE)" "$(ARDUINODIR)/hardware/tools/avr/bin/avrdude"
AVRDUDECONF := $(ARDUINODIR)/hardware/tools/avr/etc/avrdude.conf
else
AVRDUDECONF := $(wildcard $(AVRDUDE).conf)
endif
endif

# flags
CPPFLAGS += -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections
CPPFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPPFLAGS += -mmcu=$(BOARD_BUILD_MCU)
CPPFLAGS += -DF_CPU=$(BOARD_BUILD_FCPU) -DARDUINO=$(ARDUINOCONST)
CPPFLAGS += -DUSB_VID=$(BOARD_USB_VID) -DUSB_PID=$(BOARD_USB_PID)
CPPFLAGS += -I. -Iutil -Iutility -I $(ARDUINOCOREDIR)
CPPFLAGS += -I $(ARDUINODIR)/hardware/arduino/avr/variants/$(BOARD_BUILD_VARIANT)/
CPPFLAGS += $(addprefix -I , $(LIBRARYDIRS))
CPPDEPFLAGS = -MMD -MP -MF .dep/$<.dep
CPPINOFLAGS := -x c++ -include $(ARDUINOCOREDIR)/Arduino.h
AVRDUDEFLAGS += $(addprefix -C , $(AVRDUDECONF)) -DV
AVRDUDEFLAGS += -p $(BOARD_BUILD_MCU) -P $(TTY)
AVRDUDEFLAGS += -c $(BOARD_UPLOAD_PROTOCOL) -b $(BOARD_UPLOAD_SPEED)
LINKFLAGS += -Os -Wl,--gc-sections -mmcu=$(BOARD_BUILD_MCU)

# include dependencies
ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "cl"
-include $(DEPFILES)
endif
endif

# $(info COMPILE.c: $(COMPILE.c))

#_______________________________________________________________________________
#																																					 Rules

.PHONY:	all up upload cl clean boards size


all: target clean
	@echo "Done"

up: upload

cl: clean

target: $(TARGET).hex

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET).elf $(TARGET).hex $(ARDUINOLIB) *~
	rm -rf .lib .dep

upload: target uploadcore clean

uploadcore:
	@echo "\nUploading to board..."
	@test -n "$(TTY)" || { \
		echo "error: TTY could not be determined automatically." >&2; \
		exit 1; }
	@test 0 -eq $(TTYGUESS) || { \
		echo "*GUESSING* at serial device:" $(TTY); \
		echo; }
ifeq "$(BOARD_BOOTLOADER_PATH)" "caterina"
	stty $(STTYFARG) $(TTY) speed 1200
	sleep 1
else
	stty $(STTYFARG) $(TTY) hupcl
endif
	$(AVRDUDE) $(AVRDUDEFLAGS) -U flash:w:$(TARGET).hex:i

size: sizecore clean

sizecore: $(TARGET).elf 
	echo && $(AVRSIZE) --format=avr --mcu=$(BOARD_BUILD_MCU) $(TARGET).elf

# building the target

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

.INTERMEDIATE: $(TARGET).elf

$(TARGET).elf: $(ARDUINOLIB) $(OBJECTS)
	$(CC) $(LINKFLAGS) $(OBJECTS) $(ARDUINOLIB) -lm -o $@

%.o: %.c
	mkdir -p .dep/$(dir $<)
	$(COMPILE.c) $(CPPDEPFLAGS) -o $@ $<

%.o: %.cpp
	mkdir -p .dep/$(dir $<)
	$(COMPILE.cpp) $(CPPDEPFLAGS) -o $@ $<

%.o: %.cc
	mkdir -p .dep/$(dir $<)
	$(COMPILE.cpp) $(CPPDEPFLAGS) -o $@ $<

%.o: %.C
	mkdir -p .dep/$(dir $<)
	$(COMPILE.cpp) $(CPPDEPFLAGS) -o $@ $<

%.o: %.ino
	mkdir -p .dep/$(dir $<)
	$(COMPILE.cpp) $(CPPDEPFLAGS) -o $@ $(CPPINOFLAGS) $<

%.o: %.pde
	mkdir -p .dep/$(dir $<)
	$(COMPILE.cpp) $(CPPDEPFLAGS) -o $@ $(CPPINOFLAGS) $<

# building the arduino library

$(ARDUINOLIB): $(ARDUINOLIBOBJS)
	$(AR) rcs $@ $?

.lib/%.c.o: %.c
	mkdir -p $(dir $@)
	$(COMPILE.c) -o $@ $<

.lib/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(COMPILE.cpp) -o $@ $<

.lib/%.cc.o: %.cc
	mkdir -p $(dir $@)
	$(COMPILE.cpp) -o $@ $<

.lib/%.C.o: %.C
	mkdir -p $(dir $@)
	$(COMPILE.cpp) -o $@ $<
