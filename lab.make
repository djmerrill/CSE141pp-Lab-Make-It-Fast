CMD_LINE_ARGS=--engine all-core --mat-small 96 --mat-large 1024 --iterations 20
include $(ARCHLAB_ROOT)/compile.make

%.gprof: %.exe gmon.out
	prof %.exe > %.gprof

.PHONY: run-submission
run-submission: default

%.exe : %.o ../lab_files/main.o ../lab_files/reference.o
	$(CXX) -fopenmp $(LDFLAGS) $^ -o $@

