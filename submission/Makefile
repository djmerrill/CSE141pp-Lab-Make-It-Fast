include ../lab.make

default: code.out $(EXTRA_TARGETS)

%.gprof: %.exe gmon.out
	gprof %.exe > %.gprof

.PHONY: run-submission
run-submission: default

%.exe : %.o ../lab_files/main.o ../lab_files/reference.o
	$(CXX) $^ -fopenmp $(LDFLAGS) -o $@ 
