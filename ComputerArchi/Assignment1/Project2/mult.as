	lw	0	2	mcand
	lw	0	3	mplier
	lw	0	4	one
	add	0	0	1
loop	nor	4	0	5	
	nor	5	3	5	
	jalr	7	7
	beq	5	0	jump
	add	4	4	4
	lw	0	6	max
	beq	4	6	2
	beq	0	0	loop
	noop
	halt
jump	add	0	2	5
	lw	0	6	one
	beq	6	4	back
loop2	add	5	5	5
	add	6	6	6
	beq	6	4	back
	beq	0	0	loop2
back	add	1	5	1
	jalr	7	5
mcand	.fill	32766
mplier	.fill	10383
one	.fill	1
max	.fill	32768
