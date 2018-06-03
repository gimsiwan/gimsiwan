	lw	0	1	index1
	lw	0	2	index2
	sw	0	1	temp
	add	0	2	1
	lw	0	2	temp
	noop
	halt
index1	.fill	10
index2	.fill	20
temp	.fill	0
