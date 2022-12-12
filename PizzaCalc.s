.text

main:
    #open stack
    addiu $sp,  $sp, -4 
    sw $ra, 0($sp) 

    l.s $f10, val1 #load pi into f31
    l.s $f9, val2 #load 2 into f30 to find radius 

    li $t4, 0  # t4 is temporarily the checker of whether or not head has been created, will later be the holder of the head node
    li $t6, 0 #t6 holds count


    #get all the pizza inputs
    _inputLoop:
        #use syscall to print the prompt 
        li $v0, 4 
        la $a0, prompt1
        syscall

        li $v0, 9 #syscall code for allocating heap memory
        li $a0, 64 #allocate 64  bytes of buffer
        syscall #memory address of first byte returned in v0
        move $t1, $v0# move buffer to $t1

        #use syscall to read user input string
        li $v0, 8 #string input
        move $a0, $t1 #load space of max 63 charactrs into addresss
        li $a1, 64 #max num of characters to read (64-1)
        move $t1, $a0 #save read string buffer address into t1
        syscall

        #remove enter character
        la $t2, ($t1) #store initial pointer int t2
        lb $t5, newline
        _removeLoop:
            lb $t3, ($t1)
            beq $t3, $t5, _removeNL
            addi $t1, $t1, 1
            b _removeLoop
        _removeNL:
            sb $zero, ($t1)
        
        move $t1, $t2
        #compare the pizza name to see if DONE
        la $t7, done #t7 holds done
        _compLoop:
            lb $t5, ($t7) #load first character from DONE
            lb $t3, ($t1) #load first character from the pizza name  
            bne $t5, $t3, _cmpne #if not equal, jump out of loop

            beq $t5, $zero, _cmpe #if you reach EOS and haven't jumped, strings are equal

            addi $t7, $t7, 1 #point to the next byte of DOEN
            addi $t1, $t1, 1 #point to the next byte of the pizza name
            j _compLoop

        #if equal, break out of the entire loop
         _cmpe:
            j _endILoop

        #if not equal, read the rest and go back to the top of the loop
        _cmpne:
            move $t1,  $t2
            #read diameter---------------
            li $v0, 4 #syscall for print string
            la $a0, prompt2
            syscall

            li $v0, 6 #read float
            syscall
            mov.s $f4, $f0 #move diameter to  t4

            #read price-----------------
            li $v0, 4 #syscall for print string
            la $a0, prompt3
            syscall

            li $v0, 6 #read float
            syscall

            mov.s $f5, $f0 #move price to f5

            #if price is zero, branch
            l.s $f6, zerof
            c.eq.s $f5, $f6
            bc1f _pricenz #if price isnt zero, break to _pricenz and calculate as normal

            l.s $f4, zerof
            j _finPrice
            #else
        
            #if price is zero, set ppd to zero
            _pricenz:
                
                div.s $f4, $f4, $f9 # divide diameter in half

                mul.s $f4, $f4, $f4 #square the radius
                mul.s $f4, $f4, $f10 # area = r^2 *pi

                div.s $f4, $f4, $f5  # pizza per dollar =  area / price

            _finPrice:
                mov.s $f12, $f4
        
            #print ppd
            ##syscall

            #dyanmically allocate enough space for 1 struct in memory 
            li $v0, 9 #syscall code for allocating heap memory
            li $a0, 12 #number of bytes to allocate -> string(8), float(4), pointer (8)
            syscall #memory address of first byte returned in v0

            sw $t1, 0($v0)  #save address of the pizza name into the first chunk of the struct
            s.s $f12, 4($v0) #store floating ppd into the seecond chunk 
            sw $zero, 8($v0) #store next address, which by default 0 or Null

            bnez $t4, _insertNode #if t4 is not equal to zero, the list has been started. Branch to _insertNode
            #if the linked list hasn't been started yet, start it and initialize this pizza struct as the head (t4)
            move $t4, $v0 #load the new struct address into head
            #la $a0, ran
            #li $v0, 4
            #syscall
            j _count
     
            #move $f12, 8($t4)
            #li $v0, 2
            #syscall
            

            _insertNode:
          
                move $a0, $t4  #current = head (a0)
                move $a1, $v0 #copy new struct pointer into a1

                l.s $f4, 4($a0) #load in ppd for current
                l.s $f5, 4($a1) #load in ppd for new

                c.le.s $f5, $f4 #if f5 <=f4, code = 1
                bc1t _while #if code ==1, bramch to while
              
                #otherwise, if new ppd is greater than current, then insert new before current
                move $t1, $t4 #t1 = head
                move $t4, $a1 # make the new head the new struct pointer
                sw $t1, 8($t4) #store the old head into the next of the new head 
    
                j _count

                _while:

                    lw $t0, 8($a0)#load current-> next into t0
                    beq $t0, $zero _break #break out of loop if current->next == NULL

                    addiu $sp,  $sp, -16  #t4,6,0 are locked
                    sw $ra, 0($sp) 
                    sw $t4, 4($sp)#store head in stack 
                    sw $t6, 8($sp)#store count in stack
                    sw $t0, 12($sp)#store "current, next"

                    jal comparator
                    #go to comparator, returns 1 if new is greater than current->next, returns 0 otherwise in v1
        
                    lw $ra, 0($sp) #restore return address
                    lw $t4, 4($sp) #restore head 
                    lw $t6, 8($sp) #restore counter
                    lw $t0, 12($sp) #restore "done"
                    addiu $sp, $sp, 16 #collapse stack
                    
                    beq $v0, $zero, _break #break out of the loop if new not > current->next

                    move $a0, $t0  #current = current-> next
                    
                    j _while
                _break:
                    sw $t0, 8($a1) #store current-> next as next for new
                    sw $a1, 8($a0) #store new pizza pointer as current-> next
                _count:
                    addi $t6, $t6, 1 #increment counter by 1


        j _inputLoop



    _endILoop:
        #for loop with count to print out linked list 
        li $t0, 0 #use t0 as i
     
        _printLoop:
            #if i == count, break 
            bge $t0, $t6, _breakPrint
            li $v0, 4 #syscall for print string 
            lw $a0, 0($t4) #load address of pizza name in a0
            syscall
            la $a0, spacer
            syscall
            li $v0, 2#syscall for print float
            l.s $f12, 4($t4)
            syscall
            li $v0, 4
            la $a0, newline
            syscall
            lw $t4, 8($t4) #head = head->next
            
            addi $t0, $t0, 1
            j _printLoop



        _breakPrint:

        lw $ra, 0($sp) 
        addiu $sp, $sp, 4

        li $v0, 0 #return exit success for main
        jr $ra
.end main

#compares two structs, sets to 0 if current-> next_>pdd < new->ppd
comparator: 
#current struct address is in a0, new struct address is in a1
    li $v0, 0

    lw $t1, 8($a0) #load current-> next into t1
    l.s $f4, 4($t1) #load current->next->pdd into f4
    l.s $f5, 4($a1)  #load new->pdd into f5

    c.eq.s $f4, $f5  #if f4 == f5, code=1
    bc1t _ppdeq #jump to string comparator if ppd are equal 
   
    c.lt.s $f5, $f4 #if new->ppd <currnet->next->ppd, return 1 
    bc1t _ret1
    jr $ra 
    
    #if equal, compare strings
    _ppdeq:
        lw $t0, 0($t1) #load current->next->name into t0
        lw $t1, 0($a1) #load new->name into t1 
    
        _strCmp:
            lb $t2, ($t0) #load first character from current 
            lb $t3, ($t1) #load first character from new   
    
            bne $t2, $t3, _strCmpNE #if the characters aren't equal branch out 
            beq $t2, $zero, _ret0 #if you reach EOS and haven't jumped, strings are equal        
        
            addi $t1, $t1, 1 #point to the next byte of new name
            addi $t0, $t0, 1 #point to the next byte current name
            j _strCmp

    #if new name != current name, comapre the current bytes 
        _strCmpNE:
            sgt $t2, $t3, $t2 #if new name > current name, t2 = 1, else t2 = 0
            bne $t2, $zero, _ret1 #if new name > current name, ret 1 
           #li $v0, 4
            #la $a0, lol
            #syscall
            beq $t2, $zero, _ret0 #else, ret 0

        _ret1:
            #li $v0, 4
            #la $a0, lol
            #syscall
            li $v0, 1 
            jr $ra 
        _ret0:
            #li $v0, 4
           # la $a0, done
           # syscall
            li $v0, 0
            jr $ra  
    


.data
prompt1: 
    .asciiz "Please enter a pizza name:"
prompt2: 
    .asciiz "Please enter the pizza's diameter:"
prompt3:
    .asciiz "Please enter the pizza's price:"
buffer:
    .space 64
done:
    .asciiz "DONE"
newline:
    .asciiz "\n"
spacer:
    .asciiz " "
lol:
    .asciiz "lol"

val1: .float 3.14
val2: .float 2.00
zerof: .float 0.00

