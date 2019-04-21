local a = assembler.create()

local label1 = a:createLabel("label1") 

-- GAS
-- base = rax, index = rbx, disp = 3
local ref1 = a.rax+0x03+a.rbx
local ref2 = a.rbx*1 + 1 + 2
local ref3 = a.rax+a.rbx+0x03
-- base = rax, index = rbx, disp = 3, label = label1
local ref4 = a.rbx*1 + 1 + 2 + a.rax + label1

a:mov(a.dword_ptr(a.rax), 1)
a:lea(a.rax, a.qword_ptr(a.rax*2))
a:mov(a.rcx, label1) -- Usage before binding.
a:mov(a.eax, 1)
a:lea(a.rax, a.qword_ptr(label1))
a:mov(a.edx, 2)
a:bindLabel(label1)
a:sub(a.edx, a.eax)
a:cmp(a.edx, 0)
a:ja(label1) -- Usage after binding.

local bytes = a:make(0x00400000, true)
printhexdump(bytes)
