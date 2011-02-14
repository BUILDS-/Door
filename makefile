SRCS		:= monitor.cpp
OBJS		:= $(SRCS:.cpp=.o)
CXXFLAGS	:= -ggdb -Wall -pedantic
INCPATHS	:= -I/usr/include/mysql
LIBPATHS	:= -L/usr/lib
LIBS		:= -lmysqlclient -lSDL -lSDL_image -lSDL_ttf
EXE		:= Door

$(EXE): $(OBJS)
	$(CXX) $(OBJS) $(LIBPATHS) $(LIBS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCPATHS) -c $< -o $@

