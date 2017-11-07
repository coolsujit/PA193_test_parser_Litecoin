CC = gcc
default: all
LIBS = -lssl -lcrypto
MAIN = liteparser
all: 	  
	$(CC) -o $(MAIN) -g -Wall -Wextra liteparser.c $(LIBS)
clean:
	$(RM) *~ $(MAIN)
test:
	./$(MAIN)
