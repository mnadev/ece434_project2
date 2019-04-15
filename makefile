all: Problem1 Problem2 Problem3 Problem4PtA Problem4PtB Problem4PtC

Problem1:
	gcc prob1/f1.c -O3 -o Prob1
Problem2:
	gcc prob2/prob2.c -O3 -o Prob2
Problem3:
	gcc prob3/Problem3.c -O3 -o Prob3
Problem4PtA:
	gcc prob4/Problem4_PartA.c -O3 -o Prob4PtA
Problem4PtB:
	gcc prob4/Problem4_PartB.c -O3 -o Prob4PtB -lm
Problem4PtC:
	gcc prob4/Problem4_PartC.c -O3 -o Prob4PtC -lpthread
clean:
	rm Prob1 Prob2 Prob3 Prob4PtA Prob4PtB Prob4PtC