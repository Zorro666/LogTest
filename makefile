LOGTEST_CORE:= logtest.cpp
LOGTEST_DEPENDS:=$(LOGTEST_CORE) 

LOGTEST_TEST_DEPENDS:=logtest.cpp $(LOGTEST_DEPENDS) 

PROJECTS:=\
		  logtest\

all: $(PROJECTS)

C_CPP_COMMON_COMPILE_FLAGS:= -g -Wall -Wextra -Wuninitialized -Winit-self -Wstrict-aliasing -Wfloat-equal -Wshadow -Wconversion -Werror -fpack-struct=4 -Wno-multichar

C_COMPILE:=gcc -c
C_COMPILE_FLAGS:=-ansi -pedantic-errors

CPP_COMPILE:=g++ -c
CPP_COMPILE_FLAGS:=

LINK:=g++
LINK_FLAGS:=-g -lm -lpthread

ifdef WINDIR
TARGET_EXTENSION := .exe
LINK_FLAGS += -lwsock32
C_CPP_COMMON_COMPILE_FLAGS += -DLOGTEST_PLATFORM_WINDOWS=1
else
C_CPP_COMMON_COMPILE_FLAGS += -DLOGTEST_PLATFORM_LINUX=1
TARGET_EXTENSION := 
endif	# ifdef WINDIR

define upperString
$(shell echo $1 | tr [a-z] [A-Z] )
endef

define PROJECT_template
$2_SRCFILES += $1.cpp
$2_SRCFILES += $($2_DEPENDS)
$2_DFILES:=$$($2_SRCFILES:.cpp=.d)

$2_OBJFILE:=$1.o
$2_OBJFILES:=$$($2_SRCFILES:.cpp=.o)

SRCFILES += $$($2_SRCFILES)
OBJFILES += $$($2_OBJFILES)
DFILES += $$($2_DFILES)

TARGETS += $1

$1: $$($2_OBJFILES) 
endef
     
$(foreach project,$(PROJECTS),$(eval $(call PROJECT_template,$(project),$(call upperString,$(project)))))

TARGET_EXES := $(foreach target,$(TARGETS),$(target)$(TARGET_EXTENSION))

test:
	@echo C_COMPILE=$(C_COMPILE)
	@echo C_COMPILE_FLAGS=$(C_COMPILE_FLAGS)
	@echo CPP_COMPILE=$(CPP_COMPILE)
	@echo CPP_COMPILE_FLAGS=$(CPP_COMPILE_FLAGS)
	@echo LINK=$(LINK)
	@echo LINK_FLAGS=$(LINK_FLAGS)
	@echo PROJECTS=$(PROJECTS)
	@echo TARGETS=$(TARGETS)
	@echo SRCFILES=$(SRCFILES)
	@echo OBJFILES=$(OBJFILES)
	@echo DFILES=$(DFILES)
	@echo LOGTEST_TEST_SRCFILES=$(LOGTEST_TEST_SRCFILES)
	@echo LOGTEST_TEST_OBJFILES=$(LOGTEST_TEST_OBJFILES)
	@echo LOGTEST_TEST_DEPENDS=$(LOGTEST_TEST_DEPENDS)
	@echo LOGTEST_TEST_DFILES=$(LOGTEST_TEST_DFILES)
	@echo LOGTEST_TEST_OBJFILE=$(LOGTEST_TEST_OBJFILE)
	@echo TARGET_EXTENSION=$(TARGET_EXTENSION)
	@echo TARGET_EXES=$(TARGET_EXES)

%.o: %.c
	@echo Compiling $<
	@$(C_COMPILE) -MMD $(C_CPP_COMMON_COMPILE_FLAGS) $(C_COMPILE_FLAGS) -o $*.o $<

%.o: %.cpp
	@echo Compiling $<
	@$(CPP_COMPILE) -MMD $(C_CPP_COMMON_COMPILE_FLAGS) $(CPP_COMPILE_FLAGS) -o $*.o $<

%: %.o 
	@echo Linking $@
	@$(LINK) -o $@ $^ $(LINK_FLAGS)

.PHONY: all clean nuke format tags
.SUFFIXES:            # Delete the default suffixes

FORCE:

tags:
	@ctags -R --exclude=makefile --c++-kinds=+p --fields=+iaS --extra=+q .

update_tests:
	@cp -vf log.txt correctTestOutput.txt

testit: all
	les_test -tests

runtests: testit

clean: FORCE
	@$(RM) -vf $(OBJFILES)
	@$(RM) -vf $(DFILES)
	@$(RM) -vf tags
	@$(RM) -vf cscope.out
	@$(RM) -vf cscope.in.out
	@$(RM) -vf cscope.po.out


nuke: clean
	@$(RM) -vf $(TARGET_EXES)


sinclude $(DFILES)

