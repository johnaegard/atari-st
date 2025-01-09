    SECTION TEXT

start:
    MOVEA.L #source, a0  ; Load address of source into A0
    MOVEA.L #destination, a1  ; Load address of destination into A1
    MOVE.W #2400, D0     ; Loop counter (2400 iterations)

    ; Record the start time
    MOVE.L ($4BA), D7    ; Get system timer (high precision, system-specific)
    MOVE.L D7, start_time

copy_loop:
    MOVE.W (A0)+, (A1)+  ; Copy 2 bytes from A0 to A1
    SUBQ.W #1, D0        ; Decrement loop counter
    BNE.S copy_loop      ; Repeat until D0 == 0

    ; Record the end time
    MOVE.L ($4BA), D7    ; Get system timer again
    MOVE.L D7, end_time

    ; Calculate runtime
    SUB.L start_time, end_time ; end_time - start_time

    ; Print the runtime
    MOVEA.L #runtime_msg, A0  ; Load the address of the runtime message
    MOVE.L end_time, D1  ; Load the calculated runtime into D1
    JSR print_number     ; Call a subroutine to print the runtime value
    JSR print_newline    ; Print a newline character

    BRA stop             ; End program

print_number:
    ; Subroutine to print the value in D1
    MOVEA.L #num_buffer, A1   ; Buffer to store the number as ASCII
    MOVEQ #10, D2        ; Base 10 divisor
    CLR.W D3             ; Clear digit counter

print_number_loop:
    DIVU D2, D1          ; Divide D1 by 10
    ADDQ.W #48, D2       ; Convert remainder to ASCII ('0' = 48)
    MOVE.B D2, -1(A1)    ; Store ASCII digit in buffer (reverse order)
    ADDQ.W #1, D3        ; Increment digit counter
    CMP.W #0, D1         ; Check if quotient is 0
    BNE.S print_number_loop ; Repeat if not 0

    ; Output the digits
    MOVE.B (A1)+, D2     ; Get digit
    BEQ.S print_number_done ; Stop if null terminator
    TRAP #1              ; TOS output (write character)
    BRA.S output_digits  ; Loop for next digit

print_number_done:
    RTS

print_newline:
    MOVEQ #10, D1        ; ASCII code for newline
    TRAP #1              ; TOS output (write character)
    RTS

runtime_msg:
    DC.B "Runtime (ticks): ", 0

num_buffer:
    DS.B 12              ; Buffer for up to 10 digits + null terminator

source:
    DC.B 0, 1            ; Example source data (2 bytes, repeated as needed)
destination:
    DS.B 4800            ; Reserve 4800 bytes (2400 x 2) for destination

start_time:
    DS.L 1               ; Storage for start time
end_time:
    DS.L 1               ; Storage for end time

stop:
    RTS                  ; Return from program
