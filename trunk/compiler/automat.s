# vim: ft=gas:fdm=marker:
.extern fprintf
.extern getchar

## {{{ DEBUG makro
.macro DEBUG premenna
  push \premenna
  call write_unsigned_int
  add $4, %esp
  call writeln
.endm
## }}}

## {{{ global
.global automat_initialize
.global automat_finalize
.global automat_step
## }}}

.set SIZE_INT, 4

.text
## {{{ read_token
## precita token do current_token, pripadne tam zapise -1 na eof
## parametre: void
## vrati: void

read_token:
  call getchar
  mov %eax, current_token
  ## dbg
  push %eax
  push $read_token_debug
  push stderr
  call fprintf
  add $12, %esp
  add $1, input_pos
  ## end of dbg
  ret
## }}}

## automat funguje nasledovne

## {{{ automat_initialize
##  push_state(0)
##  push_symbol_meta(junk)
##  read_token()
automat_initialize:
  push $automat_initialize_debug
  push stderr
  call fprintf
  add $8, %esp
  # inicializacia semantiky
  call semantika_initialize_global_data
  movl %eax, semantika_data
  # inicializacia state stacku a push_state(0)
  movl $state_stack_end, state_ptr
  sub $SIZE_INT, state_ptr
  mov state_ptr, %eax
  movl $0, 0(%eax)
  # inicializacia meta stacku
  movl $meta_stack_begin, meta_ptr
  mov meta_ptr, %eax
  movl $0, 0(%eax)
  add $SIZE_INT, meta_ptr
  # inicializacia stack counteru
  movl $1, stack_items_cnt
  call read_token
  ret
## }}}

## {{{ lookup_action
## parametre:
##  int  stav
##  int  id_symbolu
## returns: ptr na riadok tabulky alebo 0
## changes: neobmedzene
lookup_action:
  mov %esp, %ebx
  push $action_table
  push $action_entry_size
  push $action_table_size 
  push 8(%ebx)  # druhy parameter
  push 4(%ebx)  # prvy parameter
  call bsearch2
  add $20, %esp
  ret
## }}}

## {{{ lookup_goto
## parametre:
##  int  stav
##  int  id_symbolu
## returns: ptr na riadok tabulky alebo 0
## changes: neobmedzene
lookup_goto:
  mov %esp, %ebx
  push $goto_table
  push $goto_entry_size
  push $goto_table_size 
  push 8(%ebx)  # druhy parameter
  push 4(%ebx)  # prvy parameter
  call bsearch2
  add $20, %esp
  ret
## }}}

## {{{ automat_step (TODO)
automat_step:
    push $automat_step_debug
    push stderr
    call fprintf
    add $8, %esp

##  s = top_state()
##  x = current_token()
##  action = lookup_action(s, x)
    push current_token
    mov state_ptr, %ecx
    push 0(%ecx)
    call lookup_action
    add $8, %esp

    push %eax
    push current_token
    mov state_ptr, %ecx
    push 0(%ecx)
    push $automat_lookup_debug
    push stderr
    call fprintf
    add $16, %esp
    pop %eax

    cmp $0, %eax
    jz recoverable_error

step_cont2:
    ## action is 8(%eax)
##  {{{ if (action is shift) TODO:
    #DEBUG 8(%eax) # action

    cmpl $SHIFT, 8(%eax)
    jnz action_not_shift
##  {
      push %eax
        push 12(%eax)
        push $automat_shift_debug
        push stderr
        call fprintf
        add $12, %esp
      pop %eax

      add $1, stack_items_cnt
      cmpl $STACK_SIZE, stack_items_cnt
      jz shift_memory_error
      
      push %eax  ## odlozime eax
##    new_metadata = init_terminal_metadata(x)
      push current_token
      push semantika_data
      call get_metadata_terminal_symbol
      add $8, %esp
##    push_symbol_meta(new_metadata)
      mov meta_ptr, %ecx
      mov %eax, 0(%ecx)
      addl $SIZE_INT, meta_ptr
##    new_state = action[2]
##    push_state(new_state)
      subl $SIZE_INT, state_ptr
      ## vratime si povodne eax
      pop %eax
      mov state_ptr, %edx
      mov 12(%eax), %ecx
      mov %ecx, 0(%edx)
## read next token from input
      call read_token

      mov $1, %eax
      ret
shift_memory_error:  ## not enough memory to shift, fail
      mov $47, %eax
      ret
##  } else 
## }}}
action_not_shift:
    cmpl $REDUCE, 8(%eax)
    jnz action_not_reduce
##  {{{ if (action is reduce) {
      push %eax
        push 12(%eax)
        push $automat_reduce_debug
        push stderr
        call fprintf
        add $12, %esp
      pop %eax    

      push %eax
##    calculate rule_ptr
      tcal:
      mov 12(%eax), %eax
      mov $rule_entry_size, %ecx
      mul %ecx
      mov $rule_table, %ecx
      add %eax, %ecx
      mov 0(%ecx), %ecx
      ## ecx now holds rule_ptr
      pop %eax      

      push %ecx

        push %eax ## odlozime si metadata
        push %ecx ## odlozime si ptr na rule
        ##  ideme posunut stacky
        mov 4(%ecx), %eax
        mov $SIZE_INT, %ecx
        mul %ecx
        mov meta_ptr, %edx
        sub %eax, %edx
        pop %ecx
        pop %eax
        ## push meta_ptr
        push %edx
      ## push rule_ptr  (this is +8 bytes from rule_ptr start)
      add $12, %ecx
      push %ecx
      sub $4, %ecx
      push 0(%ecx)
      sub $8, %ecx
      ## push rhs_size
      push 4(%ecx)
      ## push semantic_number = rule[0]
      push 0(%ecx)
##    new_metadata = semantic_reduce(semantic_number, rhs_size, rule_ptr, meta_ptr
      push semantika_data
      call get_metadata_reduce
      add $24, %esp
      pop %ecx
      ### konecne mame zavolane reduce

      push %eax ## odlozime si metadata
      push %ecx ## odlozime si ptr na rule
      ##  ideme posunut stacky
      mov 4(%ecx), %eax
      sub $1, %eax  
      mov $SIZE_INT, %ecx
      mul %ecx
      ### %eax ma dlzku rhs pravidla, tolkoto-1 chceme zredukovat zo stackov (-1 aby sme nemuseli
      ### inkrementovat pri pushi)
      add %eax, state_ptr
      sub %eax, meta_ptr

      pop %ecx
      pop %eax
##    push_symbol_meta(new_metadata)
      sub $4, meta_ptr
      mov meta_ptr, %edx
      mov %eax, 0(%edx)
      add $4, meta_ptr

##    t = top_state()
      mov state_ptr, %eax
      xxx:
      mov 4(%eax), %eax
      ## eax teraz drzi hodnotu top_state

## chcem rule_data->lhs
      mov %eax, %edx
      push %ecx
      push %edx

##    new_state = lookup_goto(t, rule_data->lhs)
      push 8(%ecx)
      push %edx
      call lookup_goto
      add $8, %esp
### %eax je ptr na goto_table_entry

      pop %edx
      pop %ecx
### {{{ begin of debug
      push %eax
        push %eax
        push 8(%ecx)
        push %edx
        push $automat_goto_lookup_debug
        push stderr
        call fprintf
        add $20, %esp
      pop %eax
## }}}

##    if (new_state == -1) ret -2 // unrecoverable error, this should newer happen (?? really ??)
      cmp $0, %eax
      jz unrecoverable_error
##    push_state(new_state)
      mov 8(%eax), %ecx
      mov state_ptr, %edx
      mov %ecx, 0(%edx)

      mov $1, %eax
      ret
##  } else
## }}}
action_not_reduce:
    cmpl $ACCEPT, 8(%eax)
    jnz unrecoverable_error
##  {{{ if (action is accept) { TODO:
##    ret 1  // done
    mov $0, %eax
    ret
##  } else {
## }}}
recoverable_error:
##    parsing_error - what to do?????
## end of file??
    cmp $-1, current_token
    jnz recoverable_cont
### return -1 (EOF)
    mov $-1, %eax
    ret
recoverable_cont:
    push input_pos
    push current_token
    push $recoverable_error_notify
    push stderr
    call fprintf
    add $16, %esp
    call read_token
    mov $42, %eax
    ret
##  }
##
unrecoverable_error:
    mov $47, %eax
    ret
## }}}

## {{{ automat_finalize (TODO)
automat_finalize:
  push $automat_finalize_debug
  push stderr
  call fprintf
  add $8, %esp
  mov $0, %eax
  mov %esp, %ebp
  mov 4(%ebp), %ecx
  cmp $0, %ecx
  jnz finalize_nonaccept

  mov meta_ptr, %ecx
  push 0(%ecx)
  push semantika_data
  call semantika_accept
  add $8, %esp

finalize_cont:  
  # zakonci semantiku
  push semantika_data
  call semantika_finalize
  add $4, %esp
  ret

finalize_nonaccept:
  push $finalize_nonaccept_notify
  push stderr
  call fprintf
  add $8, %esp
  mov $1, %eax
  jmp finalize_cont
## }}}

### {{{ strings
.data
read_token_debug: .string ">>> Reading from input char %d\n"
automat_step_debug: .string ">>> Making one step in automat\n"
automat_lookup_debug: .string ">>> Table lookup for %d %d is %x\n"
automat_shift_debug: .string ">>> Shifting, new state %d\n"
automat_reduce_debug: .string ">>> Reducing, using rule %d\n"
automat_goto_lookup_debug: .string ">>> Goto lookup for %d %d is %x\n"
recoverable_error_notify:
.string "!!! There was an error during processing char '%d' , input pos %d. Skipping character ...\n"
finalize_nonaccept_notify:
.string "!!! Because of errors, semantika_accept is not called\n"
automat_initialize_debug: .string ">>> Initializing ...\n";
automat_finalize_debug: .string ">>> Finalizing ...\n";
### }}}


###### {{{ BSS DATA SECTION
.bss
.set STACK_SIZE, 1000

input_pos: .int 0
semantika_data: .int 0
current_token: .int 0
tmp: .int 0


### stacks
state_ptr: .int 0
meta_ptr: .int 0
stack_items_cnt: .int 0

state_stack_begin:
.fill STACK_SIZE, SIZE_INT, 0
state_stack_end:

meta_stack_begin:
.fill STACK_SIZE, SIZE_INT, 0
meta_stack_end:
#### }}}
