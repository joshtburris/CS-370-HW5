CXXFLAGS=-Wall -Wextra -Wextra -Wpedantic -Werror -Wfatal-errors
CXX=gcc
FILE=Scheduler.c
OUT_EXE=Scheduler
assignment=Burris-Joshua-HW5
MAKEFILE_LIST=README.txt Makefile

all: $(FILES)
	$(CXX) -o $(OUT_EXE) $(FILE)

tar:
	tar -cvv $(MAKEFILE_LIST) *.c *.h >$(assignment).tar

zip:
	zip -r $(assignment).zip $(MAKEFILE_LIST) *.c *.h

clean:
	rm -f *.o $(OUT_EXE)
