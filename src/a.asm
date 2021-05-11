section .text
global _start

_start:

nop
 

db 0x48,0x8D,0x25,0x00,0x10,0x00,0x00 

xor rax, rax
mov al, 11
mov rdi, r12
mov rsi, r13
syscall

xor rax, rax
mov al, 11
mov rdi, r10
mov rsi, r11
syscall

xor rax, rax
mov al, 11
mov rdi, r8
mov rsi, r9
syscall

xor rax, rax
mov al, 35
mov qword [rsp+8], 200000000
mov rdi, rsp
xor rsi, rsi
syscall  

int3

nop
nop
nop
nop
nop
nop
nop
nop
nop  
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop 
nop
nop





     
     
     
     
     
     
     
     
     



 
     



 
 
 


     
     
     
     
     
     
     
     


 


 
 
 
 
 

 
 
 
 
 

 
