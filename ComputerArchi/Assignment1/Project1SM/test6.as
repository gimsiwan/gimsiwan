	lw	0	1	one
	lw	0	2	ten
	lw	0	3	thirty
loop	add	2	1	2
	beq	2	3	end
	beq	0	0	loop
end	halt
one	.fill	1
ten	.fill	10
thirty	.fill	30
