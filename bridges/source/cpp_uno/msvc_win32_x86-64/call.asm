; -*- Mode: text; tab-width: 8; indent-tabs-mode: nil comment-column: 44; comment-start: ";; " comment-start-skip: ";; *" -*-

;;
;; This file is part of the LibreOffice project.
;;
;; This Source Code Form is subject to the terms of the Mozilla Public
;; License, v. 2.0. If a copy of the MPL was not distributed with this
;; file, You can obtain one at http://mozilla.org/MPL/2.0/.
;;

;; This is the function jumped to from the trampoline generated by
;; codeSnippet() in cpp2uno.cxx. Here we call cpp_vtable_call() which
;; then calls the actual UNO function.

;; The code snippet generated is called from "normal" C++ code which
;; has no idea that it is calling dynamically generated code.

;; The generated short code snippet is not covered by any function
;; table and unwind info, but that doesn't matter, as the instructions
;; in it are not really going to cause any exception. Once it jumps
;; here it is covered by a function table, and the calls further down
;; through cpp_vtable_call() can be unwound cleanly.

;; This is in a separate file for x86-64 as MSVC doesn't have in-line
;; assembly for x64.

;; Random web links and other documentation about low-level
;; implementation details for the C++/UNO bridge on x64 Windows kept
;; here:

;; Caolan's "Lazy Hackers Guide To Porting" is useful:
;; http://wiki.openoffice.org/wiki/Lazy_Hackers_Guide_To_Porting

;; As for details about the x64 Windows calling convention, register
;; usage, stack usage, exception handling etc, the official
;; documentation (?) on MSDN is a bit fragmented and split up into a
;; needlessly large number of short pages. But still:
;; http://msdn.microsoft.com/en-us/library/7kcdt6fy%28v=VS.90%29.aspx

;; Also see Raymond Chen's blog post:
;; http://blogs.msdn.com/b/oldnewthing/archive/2004/01/14/58579.aspx

;; This one is actually more readable: "Improving Automated Analysis
;; of Windows x64 Binaries": http://www.uninformed.org/?v=4&a=1

;; This one has a mass of information about different architectures
;; and compilers, and contains some details about the x64 Windows
;; calling convention in particular that Microsoft doesn't mention
;; above:
;; http://www.agner.org/optimize/calling_conventions.pdf

;; Random interesting discussion threads:
;; http://social.msdn.microsoft.com/Forums/en/vcgeneral/thread/300bd6d3-9381-4d2d-8129-e48b392c05d8

;; Ken Johnson's blog http://www.nynaeve.net/ has much interesting
;; information, for instance:
;; http://www.nynaeve.net/?p=11

typelib_TypeClass_FLOAT equ 10
typelib_TypeClass_DOUBLE equ 11

extern cpp_vtable_call: proc

.code

privateSnippetExecutor proc frame

    ;; Make stack frame. Re-align RSP at 16 bytes. We need just one
    ;; qword of stack for our own purposes: Where cpp_vtable_call()
    ;; will store the return value of the UNO callee. But we of course
    ;; must also allocate space for the functions we call (i.e., just
    ;; cpp_vtable_call()) to spill their register parameters.

    sub rsp, 40
    .allocstack (40)
    .endprolog

    ;; Call cpp_vtable_call() with 2 parameters:

    ;; 1 (rcx): nOffsetAndIndex (already put there in code generated by codeSnippet)
    ;; 2 (rdx): pointer to where to store return value, followed by our
    ;; return address (uninteresting to cpp_vtable_call()), followed
    ;; by our spilled register parameters, as stored above, followed
    ;; by the rest of our parameters, if any.

    lea rdx, 32[rsp]

    call cpp_vtable_call

    ;; cpp_vtable_call() returns the typelib_TypeClass type of the
    ;; return value of the called UNO function

    cmp rax, typelib_TypeClass_FLOAT
    je Lfloat

    cmp rax, typelib_TypeClass_DOUBLE
    je Lfloat

    mov rax, qword ptr 32[rsp]
    jmp Lepilogue

Lfloat:
    movsd xmm0, qword ptr 32[rsp]

Lepilogue:
    add rsp, 40
    ret
privateSnippetExecutor endp

end

; vim:set shiftwidth=4 softtabstop=4 expandtab:
