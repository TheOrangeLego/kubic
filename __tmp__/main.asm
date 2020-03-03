section .text
  global kubic_main

kubic_main:
  mov qword rax, 3
  shl rax, 1
  mov [rsi - 0], rax
  mov qword rax, 5
  shl rax, 1
  sub rax, [rsi - 0]
  mov [rsi - 0], rax
  mov qword rax, 2
  shl rax, 1
  imul rax, [rsi - 0]
  mov [rsi - 0], rax
  mov qword rax, 1
  shl rax, 1
  mov [rsi - 8], rax
  mov qword rax, 2
  shl rax, 1
  sub rax, [rsi - 8]
  mov [rsi - 8], rax
  mov qword rax, 3
  shl rax, 1
  mov [rsi - 16], rax
  mov qword rax, 3
  shl rax, 1
  mov [rsi - 24], rax
  mov rax, [rsi - 0]
  add rax, [rsi - 24]
  add rax, [rsi - 16]
  imul rax, [rsi - 8]
  mov [rsi - 8], rax
  mov qword rax, 2
  shl rax, 1
  add rax, [rsi - 8]
  ret
