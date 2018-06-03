	lw	0	1	save
	lw	0	2	one
	add	1	2	1
	add	1	2	1
	add	1	2	1
	jalr	1	2
addr	noop
loop	noop
	beq	0	0	loop
	halt
save	.fill	addr
one	.fill	1
