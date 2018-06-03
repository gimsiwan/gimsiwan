	lw	0	1	one
	lw	0	5	five
	add	2	1	2
	add	3	1	3
loop	add	2	2	2
	add	3	1	3
	beq	3	5	2
	beq	0	0	loop
	noop
	halt
one	.fill	1
five	.fill	5
