#############################
# RedHate
#############################

PREFIX		=  

TARGET		=  hello-dbg
OBJ		    =  test.o

TEXTURES 	=  textures
MODELS		=  models
ASSETS		=  
			  
LDLIBS		= -lm
CC		    =  gcc
CXX		    =  g++
LD		    =  gcc
MV		    =  mv
CP		    =  cp
ECHO		=  echo
RM 		    =  rm
AR		    =  ar
RANLIB   	=  ranlib
STRIP		=  strip
PNG2H		=  tools/png2texh
OBJ2H		=  tools/obj2hGL

INCLUDES	?= -I/usr/include
LIBS		?= -L/usr/lib
LDFLAGS		?= -B/usr/lib
CFLAGS   	?= -Wall -g -O2 $(INCLUDES) $(LIBS) -fPIC -no-pie
CXXFLAGS 	?= -Wall -g -O2 $(INCLUDES) $(LIBS) -fPIC -no-pie
WARNINGS	:= 
#-w

## colors for fun
BLACK1		= \033[1;30m
BLACK2		= \033[0;30m
RED1		= \033[1;31m
RED2		= \033[0;31m
GREEN1		= \033[1;32m
GREEN2		= \033[0;32m
YELLOW1		= \033[1;33m
YELLOW		= \033[0;33m
BLUE1		= \033[1;34m
BLUE2		= \033[0;34m
PURPLE1		= \033[1;35m
PURPLE2		= \033[0;35m
CYAN1		= \033[1;36m
CYAN2		= \033[0;36m
WHITE1		= \033[1;37m
WHITE2		= \033[0;37m
NOCOLOR		= \033[0m

.PHONY: all run clean

all: $(ASSETS) $(OBJ) $(RES) $(TARGET) $(TARGET_LIB)

run:  $(ASSETS) $(OBJ) $(RES) $(TARGET) $(TARGET_LIB) 
	@./$(TARGET)

clean: 
	@printf "$(RED1)[CLEANING]$(NOCOLOR)\n" 
	@rm $(OBJ) $(RES) $(TARGET) $(TARGET_LIB) $(ASSETS)

%.h: %.obj
	@printf "$(RED1)[OBJ2H]$(NOCOLOR) $(notdir $(basename $<)).h\n" 
	@cp $(basename $<).obj $(notdir $(basename $<)).obj
	@cp $(basename $<).mtl $(notdir $(basename $<)).mtl
	@$(OBJ2H) $(notdir $<) $(notdir $(basename $<))
	@rm $(notdir $(basename $<)).obj $(notdir $(basename $<)).mtl
	@mv $(notdir $(basename $<)).h $(MODELS)/$(notdir $(basename $<)).h
	
%.h: %.png
	@printf "$(RED1)[PNG2H]$(NOCOLOR) $(notdir $(basename $<)).h\n" 
	@$(PNG2H) $< $(notdir $(basename $<))
	@mv $(notdir $(basename $<)).h $(TEXTURES)/$(notdir $(basename $<)).h

%.o: %.cpp
	@printf "$(RED1)[CXX]$(NOCOLOR) $(notdir $(basename $<)).o\n" 
	@$(CXX) $(WARNINGS) -c $< $(CXXFLAGS) -o $(basename $<).o 

%.o: %.cxx
	@printf "$(RED1)[CXX]$(NOCOLOR) $(notdir $(basename $<)).o\n" 
	@$(CXX) $(WARNINGS) -c $< $(CFLAGS) -o $(basename $<).o 

%.o: %.c
	@printf "$(RED1)[CC]$(NOCOLOR) $(notdir $(basename $<)).o\n" 
	@$(CC) $(WARNINGS) -c $< $(CFLAGS) -o $(basename $<).o 

%.a:
	@printf "$(RED1)[CC]$(NOCOLOR) $(basename $(TARGET_LIB)).a\n" 
	@$(AR) -cru $(basename $(TARGET_LIB)).a $(OBJ)

$(TARGET): $(ASSETS) $(OBJ)
	@printf "$(RED1)[CC]$(NOCOLOR) $(TARGET)\n" 
	@$(CC) $(OBJ) -static $(LDLIBS) $(LDFLAGS) $(CXXFLAGS) -o $(TARGET)
	@$(STRIP) -s $(TARGET)
