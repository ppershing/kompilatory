# vim: ft=gas:
#ASSEMBLER BASIC I/O LIBRARY
#CREATED BY PETER PERESINI
/* test */
.global write_unsigned_int
.global write_signed_int
# parameters: long (int),  changes: nothing,   uses: write; returns nothing
.global writeln  
# parameters: nothing,  changes: nothing,   uses: write; returns nothing
.global getchar
# getchar, parameters: nothing, changes:eax, return eax
.global ungetc
# parameters long (byte to unget), changes: nothing; return nothing;
.global read_spaces
# parameters: none, changes: nothing; return nothing;
.global read_int
# parematers: none, changes: eax
#.global strlen
# parameters: long ptr, changes: eax
.global write_str
# parameters: long prt, changes: none
.global read_line
# parameters: long ptr, changes: eax (number of written bytes)
.global str_cmp
# parametest long ptr a,b  , changes: eax (0=equal,1=first is greater,-1=second is greater)

#missing - read_string, write_hex

.data 
cisla: .string "0123456789-"
newline: .byte 10
data_buffer: .byte 0
data_buffer_used: .long 0

.text 
# {{{ write_[un]signed_int
write_signed_int:
     pushl %eax
     pushl %ebx
     pushl %ecx
     pushl %edx

     mov %esp,%ebx
     add $20,%ebx
     mov (%ebx),%eax

     cmp $0,%eax
     jge 1f     
     neg %eax    
     pushl %eax
     
     pushl $1
     pushl $(cisla+10)
     pushl $1
     call write
     add $12,%esp
     popl %eax
   1:
     mov $1,%ebx 
     call vypis_cislo_
     
     popl %edx
     popl %ecx
     popl %ebx
     popl %eax
    ret


write_unsigned_int:
     pushl %eax
     pushl %ebx
     pushl %ecx
     pushl %edx

     mov %esp,%ebx
     add $20,%ebx
     mov (%ebx),%eax
     mov $1,%ebx

     call vypis_cislo_

     popl %edx
     popl %ecx
     popl %ebx
     popl %eax
    ret


vypis_cislo_:
     cmp $0,%ebx
     jnz 1f
     cmp $0,%eax
     jz end_vypis
1:   mov $0,%ebx
     mov $0,%edx
     mov $10,%ecx
     div %ecx
     push %edx
     call vypis_cislo_
     pop %edx
     mov $(cisla),%ecx
     add %edx,%ecx
     pushl $1
     pushl %ecx
     pushl $1
     call write
     add $12,%esp
end_vypis:  ret
# }}}

# {{{ writeln
writeln:
     push %eax
     push %ebx
     push %ecx
     push %edx
     
     pushl $1
     pushl $(newline)
     pushl $1
     call write
     add $12,%esp
     pop %edx
     pop %ecx
     pop %ebx
     pop %eax
     ret
# }}}

# {{{ getchar
getchar:
   push %ebx
   push %ecx
   push %edx
   
   cmp $0,data_buffer_used
   jnz  read_char_buf
   push $1
   push $data_buffer
   push $0   
   call read
   add $12,%esp
   cmp $0,%eax
   jg 47f
   mov $-1,%eax

   pop %edx
   pop %ecx
   pop %ebx
   ret

47:mov $0,%eax
   mov (data_buffer),%al
   pop %edx
   pop %ecx
   pop %ebx
   ret

read_char_buf:
    xor %eax,%eax # clear
    mov (data_buffer),%al
    movl $0,data_buffer_used
    
    pop %edx
    pop %ecx
    pop %ebx
    ret
# }}}

#{{{ ungetc
ungetc:
     movl $1,data_buffer_used
     push %eax
     movb 8(%esp),%al
     movb %al,data_buffer
     pop %eax
     ret
#}}}

# {{{ read_spaces

read_spaces:
    push %eax
    push %ebx
    push %ecx
    push %edx

  1:call getc
    cmp $32,%eax
    jle 1b
    
    push %eax
    call ungetc
    add $4,%esp

    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
    ret

# }}}

# {{{ read_int
read_int:
     push %ebx
     push %ecx
     push %edx
     call read_spaces

     mov $0,%ebx  #ebx, bude znamienko
     call getc
     mov $0,%ecx
     cmp $'-',%eax
     jz 1f  # mame minus
     push %eax
     call ungetc
     add $4,%esp
     jmp 2f
   1:mov $1,%ebx
   
   2:push %ecx
     call getc
     pop %ecx
     cmp $'0',%eax
     jl 3f
     cmp $'9',%eax
     jg 3f
     # nacitali sme do eax znak
     sub $'0',%eax
     push %eax
     mov %ecx,%eax
     mov $10,%ecx
     mul %ecx
     mov %eax,%ecx
     pop %eax
     add %eax,%ecx    
     jmp 2b        
   3:
     push %eax
     call ungetc
     add $4,%esp
     # zmanienko
     cmp $1,%ebx
     jnz 4f
     neg %ecx
   4:mov %ecx,%eax
   
     pop %edx
     pop %ecx
     pop %ebx
    ret
# }}}
 
# {{{ strlen 
/*strlen:
    push %ecx
    mov $1,%ecx
    mov 8(%esp),%eax
  2:cmpb $0,(%eax)
    jz 1f
    inc %eax
    inc %ecx
    jmp 2b       
  1:mov %ecx,%eax
    pop %ecx
    ret*/
# }}}

# {{{ write_str
write_str:
    push %eax
    push %ebx
    push %ecx
    push %edx

    push 20(%esp)
    call strlen
    add $4,%esp
    dec %eax
    push %eax
    push 24(%esp)
    push $1
    call write
    add $12,%esp    

    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
    ret
# }}}   


# {{{ read_line
read_line:
    push %ebx
    push %ecx
    push %edx
    mov 16(%esp),%ebx  # ulozime ptr na buffer
    mov $0,%ecx        # num of written bytes
  1:call getc
    cmp $-1,%eax
    jz 2f
    mov %al,(%ebx)
    inc %ecx
    inc %ebx
    cmp $10,%eax
    jz 2f
    jmp 1b
  2:movb $0,(%ebx)
    inc %ecx
    mov %ecx,%eax

    pop %edx
    pop %ecx
    pop %ebx
    ret
# }}}

# {{{ str_cmp
str_cmp:
   push %ebx
   push %ecx
   push %edx
   mov 16(%esp),%ebx
   mov 20(%esp),%ecx

#   push %ebx
#   call write_str
#   pop %edx
#   push %ecx
#   call write_str
#   pop %ecx

   
   mov $0,%edx
   
1:# push %ebx
  # call write_signed_int
  # call writeln
  #pop %ebx
  # push %ecx
  # call write_signed_int
  # call writeln
  # pop %ecx


   mov (%ebx),%dl
   cmp %dl,(%ecx)
   ja 2f
   jb 3f
   cmpb $0,%dl
   je 5f
 #  jmp 5f
   inc %ebx
   inc %ecx
   jmp 1b
   
5: mov $0,%eax
   jmp 4f

2: mov $1,%eax
   jmp 4f
3: mov $-1,%eax
  jmp 4f
4: 
#   push %eax
#   call write_signed_int
#   call writeln
#   call writeln
#   pop %eax
  
   pop %edx
   pop %ecx
   pop %ebx
   ret

# }}} 
