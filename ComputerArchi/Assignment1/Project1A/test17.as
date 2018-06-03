	lw	0	1	inp_1
	lw	0	2	inp_2
	nor	2	0	3
loop	add	1	2	1
	jalr	4	4
	beq	1	0	exit
	beq	0	0	loop
	noop
exit	halt	
inp_1	.fill	5
inp_2	.fill	-1  //라벨에 문자, 숫자열 이외
