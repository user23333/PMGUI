
;Author: CSlime
;Github: https://github.com/cs1ime

	.code
_text SEGMENT

LoadMemLibraryX64 PROC
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	pushf

	mov rbp,rsp
	sub rsp,70h
	
	call SetupEnvironment
	cmp rax,0
	je LoadMemLibrary_ret

	mov [rbp-40h],rcx
	mov [rbp-58h],rdx

	xor rax,rax
	mov eax,[rcx+3Ch]
	add rcx,rax
	mov [rbp-30h],rcx
	call FirstSection
	mov [rbp-38h],rax


	call GetKernel32
	mov [rbp-18h],rax
	
	mov dword ptr[rbp-16],'triV'
	mov dword ptr[rbp-12],'Alau'
	mov dword ptr[rbp-8],'coll'
	mov dword ptr[rbp-4],0
	lea rdx,[rbp-16]
	mov rcx,rax
	call GetMemProcAddress
	mov [rbp-20h],rax

	call GetLoadLibrary
	mov [rbp-28h],rax

	xor rcx,rcx
	;mov rcx,600000h
	mov rax,[rbp-30h]
	xor rdx,rdx
	mov edx,[rax+50h]
	push rdx
	add rdx,50h
	mov r8,3000h
	mov r9,40h
	call WinAPI_EnterStack
	call qword ptr [rbp-20h]
	call WinAPI_ExitStack
	pop rsi
	add rsi,rax
	mov [rbp-50h],rsi
	mov [rbp-48h],rax

	;[rbp-18h]=kernel32  [rbp-20h]=VirtualAlloc  [rbp-28h]=LoadLibraryA  [rbp-30h]=inh  [rbp-38h]=ish  [rbp-40h]=DllData  [rbp-48h]=Dll  [rbp-50h]=RetCode_Buffer  [rbp-58h]=lpresverd
	mov rdi,rax
	mov rsi,[rbp-40h]
	mov rax,[rbp-30h]
	xor rcx,rcx
	mov ecx,[rax+54h]
	rep movsb

	mov cx,[rax+6]
	
	mov r9,[rbp-38h]
	mov r15,[rbp-40h]
	mov r14,[rbp-48h]
	;r15=dlldata r9=ish r14=dll
	;Map DllData to Memory
	LoadMemLibrary_loop1:
		push rcx

		xor rdi,rdi
		xor rsi,rsi
		xor rcx,rcx
		mov esi,[r9+14h]
		add rsi,r15
		mov edi,[r9+0Ch]
		add rdi,r14
		mov ecx,[r9+10h]
		rep movsb

		pop rcx
		add r9,28h
		loop LoadMemLibrary_loop1

	xor r8,r8
	xor r9,r9
	mov rax,[rbp-30h]
	mov r8d,[rax+90h]
	cmp r8d,0
	je LoadMemLibrary_fix_iid_end

	add r8,r14
	mov r9d,[rax+94h]
	add r9,r8
	;r8=iid r9=iidMaxPtr
	;fix IAT
	LoadMemLibrary_loop2:
		xor r10,r10
		xor r11,r11
		mov r10d,[r8+0Ch]
		cmp r10d,0
		;end of iid
		je LoadMemLibrary_loop_contine
		mov r11d,[r8+10h]
		cmp r11,0
		je LoadMemLibrary_loop_contine
		mov rax,[rbp-48h]
		add r10,rax
		add r11,rax
		;r10=DllName r11=FirstThunk

		push r8
		push r9
		push r11
		push r10
		
		mov rcx,r10
		call GetModuleHandle

		pop r10
		pop r11
		pop r9
		pop r8

		cmp rax,0
		je LoadMemLibrary_ret

		LoadMemLibrary_loop2_if1_end:
		mov rcx,rax
		LoadMemLibrary_loop2_loop1:
			mov r12,[r11]
			;r12=Value
			cmp r12,0
			je LoadMemLibrary_loop2_loop1_continue
			mov rax,8000000000000000h
			mov rbx,r12
			and rbx,rax
			jnz LoadMemLibrary_loop2_loop1_continue
			mov rax,[rbp-48h]
			lea rdx,[rax+r12+2]
			push rcx
			push r8
			push r9
			push r10
			push r11
			push r12
			call GetMemProcAddress
			pop r12
			pop r11
			pop r10
			pop r9
			pop r8
			pop rcx
			cmp rax,0
			je LoadMemLibrary_loop2_loop1_continue
			mov [r11],rax

			LoadMemLibrary_loop2_loop1_continue:
			add r11,8h
			cmp qword ptr[r11],0
			jne LoadMemLibrary_loop2_loop1
		LoadMemLibrary_loop_contine:
		add r8,20
		cmp r8,r9
		jb LoadMemLibrary_loop2

	LoadMemLibrary_fix_iid_end:
	;fix reloc
	mov r8,[rbp-30h]
	mov r9,[rbp-48h]
	xor r10,r10
	mov r10d,[r8+0B0h]
	cmp r10d,0
	je LoadMemLibrary_ret
	add r10,r9
	xor r11,r11
	mov r11d,[r8+0B4h]
	add r11,r10
	;r8=inh r9=Dll r10=ibr r11=ibrMaxPtr
	
	LoadMemLibrary_loop3:
		xor r12,r12
		xor r13,r13
		mov r12d,[r10]
		cmp r12d,0
		je LoadMemLibrary_loop3_end
		mov r13d,[r10+4]
		cmp r13d,8
		jb LoadMemLibrary_loop3_end
		lea r14,[r13+r10]
		lea r15,[r10+8]
		;r12=ibr->VirtualAddress r13=ibr->SizeOfBlock r14=NextIbrPtr r15=pRelocValues
		LoadMemLibrary_loop3_loop1:
			xor rax,rax
			mov ax,[r15]
			mov bx,ax
			and bx,0F000h
			cmp bx,0A000h
			jne LoadMemLibrary_loop3_loop1_contine
			and ax,0FFFh
			add rax,r9
			add rax,r12
			mov rbx,[rax]
			sub rbx,qword ptr[r8+30h]
			add rbx,r9
			mov [rax],rbx

			LoadMemLibrary_loop3_loop1_contine:
			add r15,2
			cmp r15,r14
			jb LoadMemLibrary_loop3_loop1

		LoadMemLibrary_loop3_contine:
		add r10,r13
		cmp r10,r11
		jb LoadMemLibrary_loop3
	LoadMemLibrary_loop3_end:
	xor rax,rax
	mov eax,[r8+28h]
	add rax,r9


	mov rdx,1
	mov rdx,1
	mov r8,[rbp-58h]
	mov rcx,[rbp-18h]
	call WinAPI_EnterStack
	call rax
	call WinAPI_ExitStack
	jmp LoadMemLibrary_ret_Rax
	LoadMemLibrary_ret:
	xor rax,rax
	xor rbx,rbx
	jmp LoadMemLibrary_ret_pop
	LoadMemLibrary_ret_Rax:
	mov rax,[rbp-48h]
	mov rbx,[rbp-50h]
	LoadMemLibrary_ret_pop:
	add rsp,70h

	
	popf
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	
	ret



FirstSection:
	;rcx=inh
	xor rax,rax
	mov ax,[rcx+14h]
	lea rax,[rcx+18h+rax]
	ret

GetMemProcAddress:
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	pushf
	


	;rcx=ModuleBase  rdx=ProcName
	mov rbp,rsp
	sub rsp,300h

	mov r10,rcx
	mov r11,rdx
	;r10=ModuleBase   r11=ProcName
	cmp word ptr[rcx],'ZM'
	jne GetMemProcAddress_retZero
	xor rax,rax
	mov eax,dword ptr[rcx+3ch]
	cmp word ptr[rcx+rax],'EP'
	jne GetMemProcAddress_retZero
	lea r12,[rcx+rax]
	;r12=inh
	mov eax,[r12+88h]
	cmp eax,0
	je GetMemProcAddress_retZero
	lea r13,[r10+rax]
	;r13=ied
	
	mov eax,[r13+18h]
	inc eax
	mov [rbp-14h],eax
	;[rbp-14h]=funcnt
	xor rax,rax
	mov eax,dword ptr[r13+1Ch]
	lea r15,[rax+r10]
	mov eax,dword ptr[r13+24h]
	lea r14,[rax+r10]
	mov eax,dword ptr[r13+20h]
	lea r13,[rax+r10]
	;r15=FAT  r14=FOT  r13=FNT

	xor rsi,rsi
	;rsi=i
	GetMemProcAddress_loop1:
		xor rcx,rcx
		mov ecx,[r13+rsi*4]
		lea rcx,[r10+rcx]
		mov rdx,r11
		push rsi
		call strcmpi
		pop rsi
		cmp al,0
		je GetMemProcAddress_loop1_contine
		xor rax,rax
		mov ax,[r14+rsi*2]
		mov eax,[r15+rax*4]
		lea rdi,[r10+rax]
		;rdi=FunAddr

		push rcx
		mov rcx,rdi
		call CheckBoundImport
		pop rcx
		cmp rax,0
		jne GetMemProcAddress_loop1_BoundImport
		mov rax,rdi
		jmp GetMemProcAddress_ret
		
		GetMemProcAddress_loop1_BoundImport:
			mov r9,rdi
			mov r8,rax
			add r8,8
			sub rsp,r8
			

			mov rdi,rsp
			mov rsi,r9
			mov rcx,rax
			rep movsb
			mov dword ptr [rax+rsp],'lld.'
			mov byte ptr[rax+rsp+4],0
			add r9,rax
			inc r9
			mov rcx,rsp
			push r9
			push r8
			call GetModuleHandle
			pop r8
			pop r9

			cmp rax,0
			je GetMemProcAddress_loop1_BoundImport_ret

			mov rdx,r9
			mov rcx,rax
			push r8
			call GetMemProcAddress
			pop r8

			GetMemProcAddress_loop1_BoundImport_ret:
			add rsp,r8
			jmp GetMemProcAddress_ret
	GetMemProcAddress_loop1_contine:
		inc rsi
		cmp esi,[rbp-14h]
		
		jne GetMemProcAddress_loop1



	GetMemProcAddress_retZero:
	xor rax,rax
	GetMemProcAddress_ret:
	add rsp,300h
	popf
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx

	ret

	CheckBoundImport:
	;rcx=FunAddr
	push r8
	push r9
	push r10
	push rcx
	push rsi

	mov r8,rcx
	mov rcx,100
	xor rsi,rsi
	CheckBoundImport_loop1:
		xor r9,r9
		mov al,byte ptr[r8+rsi]
		cmp al,'.'
		jne check1
		mov r9,2
		jmp check_end
		check1:
		cmp al,'0'
		jb check2
		cmp al,'9'
		ja check2
		inc r9
		jmp check_end
		check2:
		cmp al,'a'
		jb check3
		cmp al,'z'
		ja check3
		inc r9
		jmp check_end
		check3:
		cmp al,'A'
		jb check_end
		cmp al,'Z'
		ja check_end
		inc r9
		check_end:
		cmp r9,0
		je CheckBoundImport_retZero
		cmp r9,1
		je CheckBoundImport_loop1_con
		cmp rsi,0
		je CheckBoundImport_loop1_con
		mov rax,rsi
		jmp CheckBoundImport_ret
		CheckBoundImport_loop1_con:
		inc rsi
		loop CheckBoundImport_loop1
	CheckBoundImport_retZero:
	xor rax,rax
	CheckBoundImport_ret:
	pop rsi
	pop rcx
	pop r10
	pop r9
	pop r8
	ret


GetKernel32:
	push rbp
	mov rbp,rsp
	sub rsp,30h

	mov dword ptr[rbp-16],'nrek'
	mov dword ptr[rbp-12],'23le'
	mov dword ptr[rbp-8],'lld.'
	mov dword ptr[rbp-4],0
	lea rcx,[rbp-16]
	call GetModule

	add rsp,30h
	pop rbp
	ret
GetLoadLibrary:
	push rbp
	mov rbp,rsp
	sub rsp,30h

	call GetKernel32
	mov rcx,rax
	mov dword ptr[rbp-16],'daoL'
	mov dword ptr[rbp-12],'rbiL'
	mov dword ptr[rbp-8],'Ayra'
	mov dword ptr[rbp-4],0
	lea rdx,[rbp-16]
	call GetMemProcAddress

	add rsp,30h
	pop rbp
	ret

GetModuleHandle:
	;rcx=ModuleName
	push rcx
	call GetModule
	pop rcx
	cmp rax,0
	jne GetModuleHandle_ret

	push rcx
	
	call GetLoadLibrary
	
	pop rcx

	call WinAPI_EnterStack
	call rax
	call WinAPI_ExitStack

	GetModuleHandle_ret:
	ret

WinAPI_EnterStack:
	lea r11,[rsp+8]
	and rsp,0fffffffffffffff0h 
	push r11
	push r11
	sub rsp,30h
	jmp qword ptr[r11-8]
WinAPI_ExitStack:
	pop r11
	add rsp,38h
	pop rsp
	jmp r11

GetModule:
	;rcx=ModuleName
	push rbp
	mov rbp,rsp
	sub rsp,80h
	
	mov r13,rcx
	;r13=ModuleName
	mov rax,gs:[60h]
	mov rax,[rax+18h]
	lea r10,[rax+10h]
	mov rax,[rax+18h]
	mov r11,rax
	;r10=first
	;r11=query
	GetModule_loop1:
		mov r12,[r11+60h]
		cmp r12,0
		je GetModule_loop1_contine
		;r12=DllName
		xor rcx,rcx
		mov cx,word ptr[r11+58h]
		cmp cx,0
		je GetModule_loop1_contine
		cmp cx,60h
		ja GetModule_loop1_contine
		lea rdi,[rbp-70h]
		;rdi=AnsiStrBuffer
		mov byte ptr[rdi+rcx],0
		GetModule_loop1_loop1:
			mov ax,word ptr[r12+rcx*2-2]
			mov byte ptr[rdi+rcx-1],al
			loop GetModule_loop1_loop1

		mov rcx,rdi
		mov rdx,r13
		call strcmpi
		cmp al,1
		je GetModule_ret
	GetModule_loop1_contine:
		mov r11,[r11+8]
		cmp r10,r11
		jne GetModule_loop1

	GetModule_retZero:
		add rsp,80h
		pop rbp
		xor rax,rax
		ret
	GetModule_ret:
		mov rax,[r11+30h]
		add rsp,80h
		pop rbp
		ret

tosmall:
	cmp r8b,'A'
	jb tosmall_if1
	cmp r8b,'Z'
	ja tosmall_if1
	;change to small
	or r8b,20h
	tosmall_if1:
	ret


strcmpi:
	;rcx=str1  rdx=str2
	push rcx
	push rdx
	call lstrlenA
	mov r8,rax
	mov rcx,rdx
	call lstrlenA
	mov r9,rax
	pop rdx
	pop rcx


	cmp r8,r9
	jne strcmpi_retZero
	mov rsi,rcx
	xor rdi,rdi
	mov rcx,r8
	strcmpi_loop1:
		mov r8b,[rsi+rdi]
		call tosmall
		mov al,r8b
		mov r8b,[rdx+rdi]
		call tosmall
		inc rdi
		mov bl,r8b
		cmp al,bl
		jne strcmpi_retZero
		loop strcmpi_loop1
	xor rax,rax
	inc al
	ret
	strcmpi_retZero:
		xor rax,rax
		ret
lstrlenA:
	;rcx=str
	mov rax,rcx
	xor rcx,rcx
	dec rcx
	lstrlenA_loop1:
		inc rcx
		cmp byte ptr[rax+rcx],0
		jne lstrlenA_loop1
	;dec rcx
	mov rax,rcx
	ret

SetupEnvironment:
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	pushf


	sub rsp,50h

	mov dword ptr[rsp],'nrek'
	mov dword ptr[rsp+4],'23le'
	mov dword ptr[rsp+8],'lld.'
	mov dword ptr[rsp+0Ch],0
	mov rcx,rsp
	call GetModule
	cmp rax,0
	jne SetupEnvironment_ret_true
	mov dword ptr[rsp],'ldtn'
	mov dword ptr[rsp+4],'ld.l'
	mov dword ptr[rsp+8],'l'
	mov dword ptr[rsp+0Ch],0
	mov rcx,rsp
	call GetModule
	cmp rax,0
	je SetupEnvironment_ret_false

	mov dword ptr[rsp],'LrdL'
	mov dword ptr[rsp+4],'Ddao'
	mov dword ptr[rsp+8],'ll'
	mov dword ptr[rsp+0Ch],0
	mov rcx,rax
	mov rdx,rsp
	call GetMemProcAddress
	cmp rax,0
	je SetupEnvironment_ret_false

	mov word ptr[rsp],'k'
	mov word ptr[rsp+2],'e'
	mov word ptr[rsp+4],'r'
	mov word ptr[rsp+6],'n'
	mov word ptr[rsp+8],'e'
	mov word ptr[rsp+0Ah],'l'
	mov word ptr[rsp+0Ch],'3'
	mov word ptr[rsp+0Eh],'2'
	mov word ptr[rsp+10h],'.'
	mov word ptr[rsp+12h],'d'
	mov word ptr[rsp+14h],'l'
	mov word ptr[rsp+16h],'l'
	mov word ptr[rsp+18h],0

	mov word ptr[rsp+20h],24
	mov word ptr[rsp+22h],26
	mov dword ptr[rsp+24h],0
	mov qword ptr[rsp+28h],rsp

	mov dword ptr[rsp+40h],8
	
	xor rcx,rcx
	lea rdx,[rsp+40h]
	lea r8,[rsp+20h]
	lea r9,[rsp+30h]
	call WinAPI_EnterStack
	call rax
	call WinAPI_ExitStack

	cmp rax,0
	jne SetupEnvironment_ret_false

	SetupEnvironment_ret_true:
	mov rax,1
	jmp SetupEnvironment_ret
	SetupEnvironment_ret_false:
	xor rax,rax
	SetupEnvironment_ret:
	add rsp,50h
	popf
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	ret


LoadMemLibraryX64   ENDP

db 'LoadMemLibrarySignEnd'
GetMemProcAddressX64 PROC

	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	pushf
	


	;rcx=ModuleBase  rdx=ProcName
	mov rbp,rsp
	sub rsp,300h

	mov r10,rcx
	mov r11,rdx
	;r10=ModuleBase   r11=ProcName
	cmp word ptr[rcx],'ZM'
	jne GetMemProcAddress_retZero
	xor rax,rax
	mov eax,dword ptr[rcx+3ch]
	cmp word ptr[rcx+rax],'EP'
	jne GetMemProcAddress_retZero
	lea r12,[rcx+rax]
	;r12=inh
	mov eax,[r12+88h]
	cmp eax,0
	je GetMemProcAddress_retZero
	lea r13,[r10+rax]
	;r13=ied
	
	mov eax,[r13+18h]
	inc eax
	mov [rbp-14h],eax
	;[rbp-14h]=funcnt
	xor rax,rax
	mov eax,dword ptr[r13+1Ch]
	lea r15,[rax+r10]
	mov eax,dword ptr[r13+24h]
	lea r14,[rax+r10]
	mov eax,dword ptr[r13+20h]
	lea r13,[rax+r10]
	;r15=FAT  r14=FOT  r13=FNT

	xor rsi,rsi
	;rsi=i
	GetMemProcAddress_loop1:
		xor rcx,rcx
		mov ecx,[r13+rsi*4]
		lea rcx,[r10+rcx]
		mov rdx,r11
		push rsi
		call strcmpi
		pop rsi
		cmp al,0
		je GetMemProcAddress_loop1_contine
		xor rax,rax
		mov ax,[r14+rsi*2]
		mov eax,[r15+rax*4]
		lea rdi,[r10+rax]
		;rdi=FunAddr

		push rcx
		mov rcx,rdi
		call CheckBoundImport
		pop rcx
		cmp rax,0
		jne GetMemProcAddress_loop1_BoundImport
		mov rax,rdi
		jmp GetMemProcAddress_ret
		
		GetMemProcAddress_loop1_BoundImport:
			mov r9,rdi
			mov r8,rax
			add r8,8
			sub rsp,r8
			

			mov rdi,rsp
			mov rsi,r9
			mov rcx,rax
			rep movsb
			mov dword ptr [rax+rsp],'lld.'
			mov byte ptr[rax+rsp+4],0
			add r9,rax
			inc r9
			mov rcx,rsp
			push r9
			push r8
			call GetModuleHandle
			pop r8
			pop r9

			cmp rax,0
			je GetMemProcAddress_loop1_BoundImport_ret

			mov rdx,r9
			mov rcx,rax
			push r8
			call GetMemProcAddressX64
			pop r8

			GetMemProcAddress_loop1_BoundImport_ret:
			add rsp,r8
			jmp GetMemProcAddress_ret
	GetMemProcAddress_loop1_contine:
		inc rsi
		cmp esi,[rbp-14h]
		
		jne GetMemProcAddress_loop1



	GetMemProcAddress_retZero:
	xor rax,rax
	GetMemProcAddress_ret:
	add rsp,300h
	popf
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx

	ret

	CheckBoundImport:
	;rcx=FunAddr
	push r8
	push r9
	push r10
	push rcx
	push rsi

	mov r8,rcx
	mov rcx,100
	xor rsi,rsi
	CheckBoundImport_loop1:
		xor r9,r9
		mov al,byte ptr[r8+rsi]
		cmp al,'.'
		jne check1
		mov r9,2
		jmp check_end
		check1:
		cmp al,'0'
		jb check2
		cmp al,'9'
		ja check2
		inc r9
		jmp check_end
		check2:
		cmp al,'a'
		jb check3
		cmp al,'z'
		ja check3
		inc r9
		jmp check_end
		check3:
		cmp al,'A'
		jb check_end
		cmp al,'Z'
		ja check_end
		inc r9
		check_end:
		cmp r9,0
		je CheckBoundImport_retZero
		cmp r9,1
		je CheckBoundImport_loop1_con
		cmp rsi,0
		je CheckBoundImport_loop1_con
		mov rax,rsi
		jmp CheckBoundImport_ret
		CheckBoundImport_loop1_con:
		inc rsi
		loop CheckBoundImport_loop1
	CheckBoundImport_retZero:
	xor rax,rax
	CheckBoundImport_ret:
	pop rsi
	pop rcx
	pop r10
	pop r9
	pop r8
	ret


	GetModuleHandle:
	;rcx=ModuleName
	push rcx
	call GetModule
	pop rcx
	cmp rax,0
	jne GetModuleHandle_ret

	push rcx
	
	call GetLoadLibrary
	
	pop rcx

	call WinAPI_EnterStack
	call rax
	call WinAPI_ExitStack

	GetModuleHandle_ret:



	ret
GetKernel32:
	push rbp
	mov rbp,rsp
	sub rsp,30h

	mov dword ptr[rbp-16],'nrek'
	mov dword ptr[rbp-12],'23le'
	mov dword ptr[rbp-8],'lld.'
	mov dword ptr[rbp-4],0
	lea rcx,[rbp-16]
	call GetModule

	add rsp,30h
	pop rbp
	ret
GetLoadLibrary:
	push rbp
	mov rbp,rsp
	sub rsp,30h

	call GetKernel32
	mov rcx,rax
	mov dword ptr[rbp-16],'daoL'
	mov dword ptr[rbp-12],'rbiL'
	mov dword ptr[rbp-8],'Ayra'
	mov dword ptr[rbp-4],0
	lea rdx,[rbp-16]
	call GetMemProcAddressX64

	add rsp,30h
	pop rbp
	ret
WinAPI_EnterStack:
	lea r11,[rsp+8]
	and rsp,0fffffffffffffff0h 
	push r11
	push r11
	sub rsp,30h
	jmp qword ptr[r11-8]
WinAPI_ExitStack:
	pop r11
	add rsp,38h
	pop rsp
	jmp r11

GetModule:
	;rcx=ModuleName
	push rbp
	mov rbp,rsp
	sub rsp,80h
	
	mov r13,rcx
	;r13=ModuleName
	mov rax,gs:[60h]
	mov rax,[rax+18h]
	lea r10,[rax+10h]
	mov rax,[rax+18h]
	mov r11,rax
	;r10=first
	;r11=query
	GetModule_loop1:
		mov r12,[r11+60h]
		cmp r12,0
		je GetModule_loop1_contine
		;r12=DllName
		xor rcx,rcx
		mov cx,word ptr[r11+58h]
		cmp cx,0
		je GetModule_loop1_contine
		cmp cx,60h
		ja GetModule_loop1_contine
		lea rdi,[rbp-70h]
		;rdi=AnsiStrBuffer
		mov byte ptr[rdi+rcx],0
		GetModule_loop1_loop1:
			mov ax,word ptr[r12+rcx*2-2]
			mov byte ptr[rdi+rcx-1],al
			loop GetModule_loop1_loop1

		mov rcx,rdi
		mov rdx,r13
		call strcmpi
		cmp al,1
		je GetModule_ret
	GetModule_loop1_contine:
		mov r11,[r11+8]
		cmp r10,r11
		jne GetModule_loop1

	GetModule_retZero:
		add rsp,80h
		pop rbp
		xor rax,rax
		ret
	GetModule_ret:
		mov rax,[r11+30h]
		add rsp,80h
		pop rbp
		ret

tosmall:
	cmp r8b,'A'
	jb tosmall_if1
	cmp r8b,'Z'
	ja tosmall_if1
	;change to small
	or r8b,20h
	tosmall_if1:
	ret


strcmpi:
	;rcx=str1  rdx=str2
	push rcx
	push rdx
	call lstrlenA
	mov r8,rax
	mov rcx,rdx
	call lstrlenA
	mov r9,rax
	pop rdx
	pop rcx


	cmp r8,r9
	jne strcmpi_retZero
	mov rsi,rcx
	xor rdi,rdi
	mov rcx,r8
	strcmpi_loop1:
		mov r8b,[rsi+rdi]
		call tosmall
		mov al,r8b
		mov r8b,[rdx+rdi]
		call tosmall
		inc rdi
		mov bl,r8b
		cmp al,bl
		jne strcmpi_retZero
		loop strcmpi_loop1
	xor rax,rax
	inc al
	ret
	strcmpi_retZero:
		xor rax,rax
		ret
lstrlenA:
	;rcx=str
	mov rax,rcx
	xor rcx,rcx
	dec rcx
	lstrlenA_loop1:
		inc rcx
		cmp byte ptr[rax+rcx],0
		jne lstrlenA_loop1
	;dec rcx
	mov rax,rcx
	ret




GetMemProcAddressX64 ENDP

GetMemModuleHandleX64 PROC
	;rcx=ModuleName
	push rbp
	mov rbp,rsp
	sub rsp,80h
	
	mov r13,rcx
	;r13=ModuleName
	mov rax,gs:[60h]
	mov rax,[rax+18h]
	lea r10,[rax+10h]
	mov rax,[rax+18h]
	mov r11,rax
	;r10=first
	;r11=query
	GetModule_loop1:
		mov r12,[r11+60h]
		cmp r12,0
		je GetModule_loop1_contine
		;r12=DllName
		xor rcx,rcx
		mov cx,word ptr[r11+58h]
		cmp cx,0
		je GetModule_loop1_contine
		cmp cx,60h
		ja GetModule_loop1_contine
		lea rdi,[rbp-70h]
		;rdi=AnsiStrBuffer
		mov byte ptr[rdi+rcx],0
		GetModule_loop1_loop1:
			mov ax,word ptr[r12+rcx*2-2]
			mov byte ptr[rdi+rcx-1],al
			loop GetModule_loop1_loop1

		mov rcx,rdi
		mov rdx,r13
		call strcmpi
		cmp al,1
		je GetModule_ret
	GetModule_loop1_contine:
		mov r11,[r11+8]
		cmp r10,r11
		jne GetModule_loop1

	GetModule_retZero:
		add rsp,80h
		pop rbp
		xor rax,rax
		ret
	GetModule_ret:
		mov rax,[r11+30h]
		add rsp,80h
		pop rbp
		ret

	tosmall:
	cmp r8b,'A'
	jb tosmall_if1
	cmp r8b,'Z'
	ja tosmall_if1
	;change to small
	or r8b,20h
	tosmall_if1:
	ret


strcmpi:
	;rcx=str1  rdx=str2
	push rcx
	push rdx
	call lstrlenA
	mov r8,rax
	mov rcx,rdx
	call lstrlenA
	mov r9,rax
	pop rdx
	pop rcx


	cmp r8,r9
	jne strcmpi_retZero
	mov rsi,rcx
	xor rdi,rdi
	mov rcx,r8
	strcmpi_loop1:
		mov r8b,[rsi+rdi]
		call tosmall
		mov al,r8b
		mov r8b,[rdx+rdi]
		call tosmall
		inc rdi
		mov bl,r8b
		cmp al,bl
		jne strcmpi_retZero
		loop strcmpi_loop1
	xor rax,rax
	inc al
	ret
	strcmpi_retZero:
		xor rax,rax
		ret
lstrlenA:
	;rcx=str
	mov rax,rcx
	xor rcx,rcx
	dec rcx
	lstrlenA_loop1:
		inc rcx
		cmp byte ptr[rax+rcx],0
		jne lstrlenA_loop1
	;dec rcx
	mov rax,rcx
	ret


GetMemModuleHandleX64 ENDP


_text ENDS

	END