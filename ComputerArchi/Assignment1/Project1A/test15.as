	lw	0	1	inputnum1
	lw	0	2	input2
	nor	2	0	3
loop	add	1	2	1
	jalr	4	4
	beq	1	0	exit
	beq	0	0	loop
	noop
exit	halt	
inputnum1	.fill	5
input2	.fill	-1  //라벨 maximum 초과
