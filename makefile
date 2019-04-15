all: Problem4PtA Problem4PtB

Problem4PtA:
	gcc prob4/Problem4_PartA.c -O3 -o Prob4PtA
Problem4PtB:
	gcc prob4/Problem4_PartB.c -O3 -o Prob4PtB -lm
clean:
	rm Prob4PtA Prob4PtB