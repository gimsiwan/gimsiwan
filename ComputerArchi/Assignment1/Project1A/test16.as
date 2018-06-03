	lw	0	1	1input
	lw	0	2	2input
	nor	2	0	3
loop	add	1	2	1
	jalr	4	4
	beq	1	0	exit
	beq	0	0	loop
	noop
exit	halt	
1input	.fill	5
2input	.fill	-1  //label not start with letter

