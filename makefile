CC       = gcc
OBJ      = AllTests.o CuTest.o StrUtil.o
LINKOBJ  = AllTests.o CuTest.o StrUtil.o
BIN      = test

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN)

AllTests.o: AllTests.c
	$(CC) -c AllTests.c -o AllTests.o

CuTest.o: CuTest.c
	$(CC) -c CuTest.c -o CuTest.o

StrUtil.o: StrUtil.c
	$(CC) -c StrUtil.c -o StrUtil.o