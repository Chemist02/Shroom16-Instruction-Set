# Jump to main program.
jmp main

# Define recursive Fibonacci function. n is passed in as the number right below the top of the stack where n is the 
# number Fibonacci number we want to return. The value is returned at the same place the argument was passed in. Let 
# $g1 and $g2 be unchanged by this 
# function.
:recFib
	# Load argument into $g0, so that is has a value of n.
	lw $g0 $sp -1

	# Save original $fp.
	addi $sp $sp 1
	sw $fp $sp -1
	addi $fp $sp 0

	# Save original $ca, $g1, and $g2.
	addi $sp $sp 3
	sw $ca $fp 0
	sw $g1 $fp 1
	sw $g2 $fp 2

	# Check if $g0 is 0 or 1, if so return $g0.
	addi $g1 $zero 0
	addi $g2 $zero 1
	cmp $g3 $g0 $g1
	jeq $g3 recFibReturn
	cmp $g3 $g0 $g2
	jeq $g3 recFibReturn
	
	# Call with n - 1.
	addi $g1 $g0 -1

	addi $sp $sp 1
	sw $g1 $sp -1
	call recFib
	lw $g2 $sp -1
	
	# Call with n - 2.
	addi $g1 $g1 -1
	sw $g1 $sp -1
	call recFib
	lw $g1 $sp -1

	# $g3 now contains the next fib number.
	add $g3 $g1 $g2

	addi $sp $sp -1

	:recFibReturn

	# Load original $ca, $g1, and $g2.
	lw $ca $fp 0
	lw $g1 $fp 1
	lw $g2 $fp 2

	# Reset $sp and $fp.
	addi $sp $sp -3
	lw $fp $sp -1
	addi $sp $sp -1

	# Place return value where the original argument was passed.
	sw $g3 $sp -1

	# Return.
	jr $ca

:main
	addi $g0 $zero 10
	addi $sp $sp 1
	sw $g0 $sp -1
	call recFib
	lw $g0 $sp -1
	addi $sp $sp -1

	?out $g0
	?in $g0
