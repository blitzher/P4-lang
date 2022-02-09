
CC = clang++
OUT= build

build:
	%rm -rf $(OUT)
	%mkdir $(OUT)
	$(CC) src/main.cpp -o $(OUT)/main

run: build
	./$(OUT)/main

