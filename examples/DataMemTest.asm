addi $g0 $zero 2
addi $g1 $zero -1000
sw $g1 $g0 1
lw $g2 $g0 1
?out $g2
?in $g4
