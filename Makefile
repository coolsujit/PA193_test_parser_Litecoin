CC = gcc
default: all
LIBS = -lssl -lcrypto
MAIN = liteparser
all: 	  
	$(CC) -o $(MAIN) -g -Wall -Wextra -std=c99 liteparser.c $(LIBS)
clean:
	$(RM) *~ $(MAIN)
test:
	./$(MAIN) block_current
test1:
	./$(MAIN) block_w1
test2:
	./$(MAIN) block_w2
test3:
	./$(MAIN) block_w3
test4:
	./$(MAIN) block_w4
