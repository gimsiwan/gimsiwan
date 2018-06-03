	lw	0	1	one
	lw	0	5	five
	add	0	0	2
loop	lw	2	3	one
	add	4	3	4
	add	2	1	2	
	beq	2	5	2
	beq	0	0	loop
	noop
	halt
one	.fill	1
two	.fill	2
three	.fill	3
four	.fill	4
five	.fill	5
