# Algorithm pseudocode from Wikipedia
# for each pixel (Px, Py) on the screen do
#    x0 := scaled x coordinate of pixel (scaled to lie in the Mandelbrot X scale (-2.00, 0.47))
#    y0 := scaled y coordinate of pixel (scaled to lie in the Mandelbrot Y scale (-1.12, 1.12))
#    x := 0.0
#    y := 0.0
#    iteration := 0
#    max_iteration := 1000
#    while (x^2 + y^2 = 2^2 AND iteration < max_iteration) do
#        xtemp := x^2 - y^2 + x0
#        y := 2*x*y + y0
#        x := xtemp
#        iteration := iteration + 1
#     
#
#    color := palette[iteration]
#    plot(Px, Py, color)


# Let registers above $g20 be undefined

# Define constants
;const SCREEN_WIDTH_X 32
;const SCREEN_WIDTH_Y 32
# These values are multiplied by 2^10=1024
;const MIN_X -2048
;const MAX_X 482
;const MIN_Y -1147
;const MAX_Y 1147
# If x plus y exceed 4, we know we diverge
;const BAIL_BOUND 4096
;const MAX_ITERATIONS 50

# Jump to the main program
jmp main

# Pass in via $g24. Divides result by 1024 via bit shifting, in place.
# Takes into account extra bits in $wr
# Does not alter any regs besides $g24
:correct_mult_result
	# Save reg values for restoration later to fufill contract
	addi $sp $sp 3
	sw $g0 $sp -1
	sw $g1 $sp -2
	sw $g2 $sp -3
	
	# $g2 will bw our stand in for $wr
	add $g2 $wr $zero

	# Shift $wr into $g24 until we run out of ones. Then shift the rest 
	# of the way until we do it 10 times. $g0 is out counter
	addi $g0 $zero 10
	
	:wr_shift_loop
		# If $wr is 0, bail
		cmp $g1 $g2 $zero
		beq $g1 bail_wr_shift_loop

		# Pull off last bit from $wr
		andi $g1 $g2 1
		# Shift regs
		srli $g24 $g24 1
		srli $g2 $g2 1
		# Apply last bit from $wr to first bit of $g24 (result reg)
		slli $g1 $g1 15
		or $g24 $g24 $g1

		# Loop until we shift 10 times
		addi $g0 $g0 -1
		cmp $g1 $g0 $zero
		bgt $g1 wr_shift_loop
	
	:bail_wr_shift_loop
	# Shift the rest of the way
	srl $g24 $g24 $g0
	
	# Restore reg values
	lw $g0 $sp -1
	lw $g1 $sp -2
	lw $g2 $sp -3
	addi $sp $sp -1
	jr $ca


# Expects real and imag in $g2 and $g3, respectively
# Pixel coords in $g0 and $g1
# Rudely alters reg values above $g20
:iterate_on_point
	# Save $ca
	addi $sp $sp 1
	sw $ca $sp -1

	# $g25 and $g26 are temp x and y, respectively
	add $g25 $zero $zero
	add $g26 $zero $zero

	# $g20 is iteration number
	add $g20 $zero $zero
	:top_of_iterate_loop
		# Check itr count
		# Use $g21 as cmp result
		addi $g21 $zero MAX_ITERATIONS
		cmp $g21 $g20 $g21
		jeq $g21 bail_and_clear
		
		# Do multiplications
		# $g22 x^2, $g23 is y^2
		# Answers are will be scaled by 1024^2 instead of 1024, so we need to shift to correct.
		mul $g22 $g25 $g25

		mul $g23 $g26 $g26

		
		
		# Check if exceeded bail bound (diverged)

		# Update temp x and y

		# Increment
		addi $g20 $g20 1
	jmp top_of_iterate_loop

	:bail_and_plot
		# If we get here, we reached max interation without diverging
		?pxset $g0 $g1 1
		jmp bail
	:bail_and_clear
		?pxset $g0 $g1 0
	:bail

	# Restore $ca
	lw $ca $sp -1
	addi $sp $sp -1 

	jr $ca

:main
	# Find x delta and y delta per pixel. Store these values in $g4 and $g5, respectively
	# SIZE = MAX_BOUND - MIN_BOUND; SIZE_PER_PIXEL = SIZE / NUM PIXELS
	addi $g20 $zero MIN_X
	addi $g21 $zero MAX_X
	addi $g22 $zero SCREEN_WIDTH_X
	sub $g4 $g21 $g20
	div $g4 $g4 $g22
	
	addi $g20 $zero MIN_Y
	addi $g21 $zero MAX_Y
	addi $g22 $zero SCREEN_WIDTH_Y
	sub $g5 $g21 $g20
	div $g5 $g4 $g22

	# Let $g0 refer to x pixel between 0 and SCREEN_WIDTH_X.
	addi $g0 $zero 0
	:x_px_loop
		# Let $g2 and $g3 refer to x and y in complex plane, where y is imaginary axis.
		addi $g2 $zero MIN_X
		mul $g20 $g4 $g0
		add $g2 $g2 $g20

		# Let $g1 refer to y pixel 0 and SCREEN_WIDTH_Y.
		addi $g1 $zero 0

		:y_px_loop
			addi $g1 $g1 1
			
			// Calculate y location
			addi $g3 $zero MIN_Y
			mul $g20 $g5 $g1
			add $g3 $g3 $g20

			
