.DEFAULT_GOAL := do

CC = clang
CXX = clang++

CPPFLAGS += -Wall -Wextra -pedantic -MMD -MP -MF .dep/$(basename $(notdir $@)).d
CFLAGS += -std=c99 -march=native -O2
CXXFLAGS += -std=c++17 -march=native -O2

main: main.o aes-ni.o

.dep:
	mkdir -p $@

.PHONY: do
do: .dep main

ifeq ($(findstring clean,$(MAKECMDGOALS))$(findstring help,$(MAKECMDGOALS)),)
-include $(shell [ -d .dep ] && ls -1 .dep/*.d)
endif
