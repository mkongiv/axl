LEX=flex
YACC=bison
CC=g++

MYPREFIX=aqcsl
MYBIN=$(MYPREFIX).exe

ISL_INC=isl-install/include
ISL_LIB=isl-install/lib

OSL_INC=osl-install/include
OSL_LIB=osl-install/lib

PAST_INC=past-install/include
PAST_LIB=past-install/lib

GMP_INC=gmp-install/include
GMP_LIB=gmp-install/lib

PIPLIBGMP_INC=piplib-gmp-install/include
PIPLIBGMP_LIB=piplib-gmp-install/lib

PIPLIBHYB_INC=piplib-hybrid-install/include
PIPLIBHYB_LIB=piplib-hybrid-install/lib

SCOPLIB_INC=scoplib-install/include
SCOPLIB_LIB=scoplib-install/lib

IRCONVERTER_INC=irconverter-install/include
IRCONVERTER_LIB=irconverter-install/lib

CANDL_INC=candl-install/include
CANDL_LIB=candl-install/lib

FM_INC=fm-install/include
FM_LIB=fm-install/lib

PONOS_INC=ponos-install/include
PONOS_LIB=ponos-install/lib


GENERATED=$(MYPREFIX)-scanner.c $(MYPREFIX)-parser.c stack.hh location.hh position.hh $(MYPREFIX)-parser.* $(MYBIN)

INC := -I. -I ir/ -I $(GMP_INC) -I $(ISL_INC) -I $(OSL_INC)
INC := $(INC) -I $(SCOPLIB_INC) -I $(CANDL_INC) -I $(IRCONVERTER_INC) -I $(PAST_INC)
INC := $(INC) -I $(PIPLIBGMP_INC) -I $(PIPLIBHYB_INC)
INC := $(INC) -I $(FM_INC) -I $(PONOS_INC) 

LIBS := -L $(ISL_LIB) -L $(OSL_LIB) -L $(GMP_LIB) -L $(SCOPLIB_LIB) -L $(PAST_LIB) -L $(PIPLIBGMP_LIB) -L $(PIPLIBHYB_LIB) -L $(IRCONVERTER_LIB) -L $(CANDL_LIB) -L $(FM_LIB) -L $(PONOS_LIB) 
LIBS := $(LIBS) -lisl -losl -lgmp -lscoplib -lpast -lpiplib-gmp -lpiplibMP -lirconverter -lcandl -lfm -lponos
CFLAGS = $(INC) -O3 -std=c++11 $(LIBS)
SRCS = $(MYPREFIX)-scanner.c	$(MYPREFIX)-parser.c utils.cc 
MACFLAG = -isystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1

all: lex grammar axl

axl: lex grammar 
	$(CC) -o $(MYBIN) $(SRCS) $(CFLAGS)

manz: lex grammar
	$(CC) $(MACFLAG) -o $(MYBIN) $(MYPREFIX)-scanner.c	$(MYPREFIX)-parser.c utils.cc $(CFLAGS)

lex: scanner.yy
	$(LEX) -o $(MYPREFIX)-scanner.c scanner.yy 

grammar: grammar.y
	$(YACC) -d -k -o $(MYPREFIX)-parser.c  grammar.y 

clean:
	rm -f $(GENERATED)
