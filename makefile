	all: Problem4PtA Problem4PtB Problem4PtC

Problem4PtA:
	gcc prob4/Problem4_PartA.c -O3 -o Prob4PtA
Problem4PtB:
	gcc prob4/Problem4_PartB.c -O3 -o Prob4PtB -lm
Problem4PtC:
	gcc prob4/Problem4_PartC.c -O3 -o Prob4PtC -lpthread
clean:
	rm Prob4PtA Prob4PtB Prob4PtC