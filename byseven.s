.text

main:
    #open stack frame
    addiu $sp,  $sp, -4   
    sw $ra, 0($sp) 

    #print prompt
    #note: v0 holds return
    li $v0, 4 #syscall for print string
    la $a0, prompt #load address of string to print into a0
    syscall

    #read an int N from the user
    li $v0, 5# syscall for read int, Note that v0 contrains the returned int
    syscall
    
    move $t0, $v0 #int N = input;

    li $t1, 1 #int i = 0;
    li $t2, 7 #int multiplier = 7;

#while(i<N) 
_loop:
    bgt $t1, $t0, _endloop #if i <= n, jump to end of the loop

    #print out the multiples of 7
    mul $t3, $t1, $t2  # t3 = i *multiplier

    li $v0, 1 #syscall for print int
    move $a0, $t3 #load t3 into args
    syscall #print i*3

    li $v0, 4 #syscall for print string
    la $a0, newline #load in address for new line
    syscall #print new line

    addi $t1, $t1, 1 #i = i++

    j _loop #jump to top of loop

_endloop:
    #Reset stack and return
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    # return 0;
    move $v0, $0
    jr $ra
.data
newline: 
    .asciiz "\n"
prompt: 
    .asciiz "Please enter a number:"
buffer:
    .space 4

