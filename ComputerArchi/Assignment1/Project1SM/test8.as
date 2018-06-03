	lw	0	1	one
	lw	0	2	minus
	lw	0	3	addr
	jalr	3	5
loop	beq	0	0	loop
goto	noop
	halt	
one	.fill	1
minus	.fill	-1
addr	.fill	goto
