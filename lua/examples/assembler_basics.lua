local a = assembler.create()

local label1 = a:createLabel("label1") 

a:mov(a.rcx, label1) -- Usage before binding.
a:mov(a.eax, 1)
a:mov(a.edx, 2)
a:bindLabel(label1)
a:sub(a.edx, a.eax)
a:cmp(a.edx, 0)
a:ja(label1) -- Usage after binding.

local bytes = a:make(0x00400000, true)
printhexdump(bytes)
