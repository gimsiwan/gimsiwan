	lw	0	1	input1
	lw	0	8	input2
	nor	2	0	3
loop	add	1	8	1
	jalr	4	4
	beq	1	0	exit
	beq	0	0	loop
	noop
exit	halt	
input1	.fill	5
input2	.fill	-1  //unvalid register
