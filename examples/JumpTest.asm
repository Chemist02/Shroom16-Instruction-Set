jmp A
?end
:A
ori $g0 $zero 5
cmp $g1 $zero $g0
jlt $g1 B
?end
:B
ori $g24 $zero 100
jeq $g1 C
?in $g8
:C
