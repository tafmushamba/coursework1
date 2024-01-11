CC = g++
CFLAGS = -std=c++11

SRCS = version1.2.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = library_system

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXEC) $(OBJS)
