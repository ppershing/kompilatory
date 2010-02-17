# vim: set ft=gas: fdm=marker:
.global bsearch2

## {{{ DEBUG makro
.macro DEBUG premenna
.endm
#  push \premenna
#  call write_unsigned_int
#  add $4, %esp
#  call writeln
#.endm
## }}}

### {{{ bsearch2
# parameters:
## 4 int prvy int na lookup
## 8 int druhy int na lookup
## 12 int velkost tabulky
## 16 int velkost riadku tabulky
## 20 int ptr na zaciatok tabulky
## returns ptr na riadok tabulky alebo NULL

## ptr pravidla v tabulke
## parametre:
##  int: cislo stavu
##  int: cislo symbolu
## vracia: 0 on error alebo ptr na pravidlo
## TODO: check na prazdnu tabulku

bsearch_debug: .string ">>> Running bsearch ...\n"
bsearch2:
#  push $bsearch_debug
#  call printf
#  add $4, %esp
  # %ebx je base pointer, %ecx je left, %edx je right
  mov %esp, %ebx
  push $0         ## -4(%ebx) je tmp premenna
  mov $0, %ecx
  mov 12(%ebx), %edx

bsearch_action_loop:
 /* call writeln
    dbg
    DEBUG %ecx
    DEBUG %edx
    edbg */

  mov %edx, %eax
  sub %ecx, %eax
  cmp $1, %eax
  jle bsearch_action_finish
  shr $1, %eax
  add %ecx, %eax
/*
  DEBUG %eax
*/
  push %eax
  push %ecx
  push %edx
  #### teraz treba porovnat tabulku s riadkov zacinajucim na action_table + action_entry_size*eax
  mov 16(%ebx), %ecx
  mul %ecx

  add 20(%ebx), %eax

##
  /*
  DEBUG 0(%eax)
  DEBUG 4(%eax)
  DEBUG 4(%ebx)
  DEBUG 8(%ebx)
  */
##
 # DEBUG 0(%ebx)
 # DEBUG -4(%ebx)
  movl $0, -4(%ebx)
bsearch_compare1:
  mov 4(%ebx), %ecx
  cmp 0(%eax), %ecx
  jl bsearch_finish_comp
  jz bsearch_compare2
  movl $1, -4(%ebx)
  jmp bsearch_finish_comp
bsearch_compare2:
  mov 8(%ebx), %ecx
  cmp 4(%eax), %ecx
  jl bsearch_finish_comp
  movl $1, -4(%ebx)
  jmp bsearch_finish_comp

bsearch_finish_comp:  
  pop %edx
  pop %ecx
  pop %eax

  cmp $0, -4(%ebx)
  jz 1f
  mov %eax, %ecx
  jmp 2f
1:
  mov %eax, %edx
2:
  jmp bsearch_action_loop
bsearch_action_finish:
  /*
  call writeln
  call writeln
  call writeln
  call writeln
  call writeln
  */
  mov %ecx, %eax
  mov 16(%ebx), %ecx
  mul %ecx
  add 20(%ebx), %eax

  mov 4(%ebx), %ecx
  cmp %ecx, 0(%eax)
  jnz bsearch_failed
  mov 8(%ebx), %ecx
  cmp %ecx, 4(%eax)
  jnz bsearch_failed
  add $4, %esp
  ret  

bsearch_failed:
  mov $0, %eax
  add $4, %esp
  ret
## }}}

