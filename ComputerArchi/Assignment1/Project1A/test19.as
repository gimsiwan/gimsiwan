	lw	0	1	input1
	lw	0	2	input2
	lw	0	5	32768
	nor	2	0	3
loop	add	1	2	1
	jalr	4	4
	beq	1	0	exit
	beq	0	0	loop
	noop
exit	halt	
input1	.fill	5
input2	.fill	-1 //offsetField Range over
