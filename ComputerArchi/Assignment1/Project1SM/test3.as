	lw	0	2	num1
	lw	0	3	num2
	lw	0	1	one
	nor	2	0	2
	sw	1	2	num2
	lw	0	2	num3
	add	2	3	1
	halt
one	.fill	1	
num1	.fill	111	
num2	.fill	120
num3	.fill	0
