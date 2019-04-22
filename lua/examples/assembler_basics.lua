local a = assembler.create()

local label1 = a:createLabel("label1") 
local label2 = a:createLabel("label2")

-- GAS
-- base = rax, index = rbx, disp = 3
local ref1 = a.rax+0x03+a.rbx
local ref2 = a.rbx*1 + 1 + 2
local ref3 = a.rax+a.rbx+0x03
-- base = rax, index = rbx, disp = 3, label = label1
local ref4 = a.rbx*1 + 1 + 2 + a.rax + label1

a:mov(a.dword_ptr(a.rax), 1)
a:lea(a.rax, a.qword_ptr(a.rax*2))
a:mov(a.rcx, label2) -- Usage before binding.
a:mov(a.eax, 1)
a:lea(a.rax, a.qword_ptr(label1))
a:mov(a.edx, 2)
a:bindLabel(label1)
a:sub(a.edx, a.eax)
a:cmp(a.edx, 0)
a:ja(label1) -- Usage after binding.
a:lea(a.rax, a.qword_ptr(a.rax*2))
a:mov(a.rcx, label1) -- Usage before binding.
a:mov(a.eax, 1)
a:bindLabel(label2)
a:lea(a.rax, a.qword_ptr(label1))
a:mov(a.edx, 2)
a:sub(a.edx, a.eax)
a:cmp(a.edx, 0)

-- If we are not debugging we dont copy it to rip.
local rip = IS_DEBUGGING and rip or 0x00400000
local bytes = a:make(rip, true --[[ dump output ]])
if IS_DEBUGGING then
	for i = 1, #bytes do
		byte_ptr[rip + (i - 1)] = bytes:byte(i)
	end
end
