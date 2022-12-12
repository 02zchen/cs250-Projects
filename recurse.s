.text

main:
    #open stack frame
    addiu $sp,  $sp, -12  
    sw $ra, 0($sp) 

    li $t0, 0 #set t0 to 0 for recurse
    li $t1, 3 #set t1 to 3 as the multiplier for reurse

    #print prompt
    #note: v0 holds return
    li $v0, 4 #syscall for print string
    la $a0, prompt #load address of string to print into a0
    syscall

    #read an int N from the user
    li $v0, 5# syscall for read int, Note that v0 contrains the returned int
    syscall

    move $a0, $v0 #int N = input;

    sw $t0, 4($sp)
    sw $t1, 8($sp)

    jal recurse

    lw $t0, 4($sp)
    lw $t1, 8($sp)

    move $a0, $v0 #put the result of recursion into a0
    li $v0, 1 #syscall command for print int
    syscall #print out the result of v0, currently store in a0

    li $v0, 4 #syscall command for print string
    la $a0, newline
    syscall 
    
    lw $ra 0($sp) #restore sp
    addiu $sp, $sp, 12  #collapse stack

    li $v0, 0 #return exit success for main
    jr $ra
.end main



recurse:
    addiu $sp, $sp, -8 #open stack for two words
    sw $ra, 4($sp) #save return address
    sw $s0, 0($sp) #save $s0

    #base case: if $a0 == 0, set $v0=2, jump to clean
   
    seq $t0, $a0, 0 #if a0 == 0, t0 is set to 1
    beqz $t0, _else # if a !=0 (t0 ==0), jump to else
    
    #if a0 == 0, then set v0 to 2
    li $v0, 2 #set v0 to 2
    j _clean #jump to clean


    _else:
        #all other cases
        addi $a0, $a0, -1 # n = n-1
        move $s0, $a0 #save this instance of n-1 to s0
        jal recurse # jumps to recurse and leaves PC right after this jal in PC
        mul $s0, $s0, $t1 #multiply s0 by 3
        add $v0, $v0, $s0 # v- = v0 + s0
        addi $v0, $v0, 1 # v0 = v0 +1


_clean: 
    
    lw $s0, 0($sp) # restore $s0
    lw $ra, 4($sp) #restore $ra
    addi $sp, $sp, 8 #collapse frame

    jr $ra #return, value in $v0


.data
newline: 
    .asciiz "\n"
prompt: 
    .asciiz "Please enter a number:"
buffer:
    .space 4
