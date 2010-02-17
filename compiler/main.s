# vim: ft=gas:
.global main

.extern printf
.extern automat_initialize
.extern automat_finalize
.extern automat_step

.text
main: ######## zaciatok programu
    ##### initialize
    mov $0, %ebx
    ##### run automaton
    call automat_initialize

main_loop:
    ### return values: 1 - step ok
    ### 47 - fatal unrecoverable error
    ### 42 - recoverable error
    ### -1 - EOF
    ### 0 - OK
    call automat_step
## check ok
    cmp $1, %eax
    jz main_loop
## check recoverble error
    cmp $42, %eax
    jz recoverable
## if unrecoverable, fail otherwise do a clean end
    cmp $47, %eax
    jz print_non_recoverable_error

main_end:
    ##### finalize automat, print all data
    or seen_error, %eax
    push %eax
    call automat_finalize
    add $4, %esp
    ##### exit with error code from finalizing
    push %eax
    call exit

recoverable:
    movl $1, seen_error
    jmp main_loop

print_non_recoverable_error:
    ##### write error
    push $non_recoverable_error
    call printf
    add $4, %esp
    ##### finalize automat
    push %eax
    call automat_finalize
    add $4, %esp
    ##### exit with error code 47
    push $47
    call exit

.data

seen_error:
  .int 0
non_recoverable_error:
 .string "!!! There was non-recoverable error (not enough stack or internal error). Sorry.\n"
