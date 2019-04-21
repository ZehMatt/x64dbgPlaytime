include("assembler_defs.lua")

local function makeOperandRegister(name, id, size)
	return { object = assembler.OP_REGISTER, name = name, id = id, size = size }
end

local function makeOperandImm(imm)
	return { object = assembler.OP_IMM, val = imm }
end

local function makeOperandLabel(id)
	return { object = assembler.OP_LABEL, id = id }
end

local function makeOperandMemory(size, regBase, regIndex, disp32, scale, labelId)
	return { object = assembler.OP_MEMORY, size = size, base = regBase, index = regIndex, disp32 = disp32, scale = scale, labelId = labelId }
end

local function splitString(s, delimiter)
	res = {}
	for match in (s .. delimiter):gmatch("(.-)" .. delimiter) do
		table.insert(res, match)
	end
	return res
end

local function initInstructions(a)
	-- Setup instruction functions.
	for mnemonicId, data in pairs(assembler.INSTRUCTION_TABLE) do
		local mnemonics = data[1]
		local operands = data[2]
		local encoding = data[3]
		-- TODO: Create better validation routines.
		-- We only use mnemonic for now
		mnemonics = splitString(mnemonics, "/")
		for subId, mnemonic in pairs(mnemonics) do
			if a[mnemonic] == nil then
				a[mnemonic] = function(s, op1, op2, op3, op4)
					s:emit(mnemonicId, op1, op2, op3, op4, subId)
					return s
				end
			end
		end
	end
end

local function initRegisters(a)
	a.rax = makeOperandRegister("rax", 1, 64)
	a.eax = makeOperandRegister("eax", 2, 32)
	a.ax = makeOperandRegister("ax", 3, 16)
	a.ah = makeOperandRegister("ah", 4, 8)
	a.al = makeOperandRegister("al", 5, 8)

	a.rbx = makeOperandRegister("rbx", 6, 64)
	a.ebx = makeOperandRegister("ebx", 7, 32)
	a.bx = makeOperandRegister("bx", 8, 16)
	a.bh = makeOperandRegister("bh", 9, 8)
	a.bl = makeOperandRegister("bl", 10, 8)

	a.rcx = makeOperandRegister("rcx", 11, 64)
	a.ecx = makeOperandRegister("ecx", 12, 32)
	a.cx = makeOperandRegister("cx", 13, 16)
	a.ch = makeOperandRegister("ch", 14, 8)
	a.cl = makeOperandRegister("cl", 15, 8)

	a.rdx = makeOperandRegister("rdx", 16, 64)
	a.edx = makeOperandRegister("edx", 17, 32)
	a.dx = makeOperandRegister("dx", 18, 16)
	a.dh = makeOperandRegister("dh", 19, 8)
	a.dl = makeOperandRegister("dl", 20, 8)

	a.rdi = makeOperandRegister("rdi", 21, 64)
	a.edi = makeOperandRegister("edi", 22, 32)
	a.di = makeOperandRegister("di", 23, 16)
	a.dil = makeOperandRegister("dil", 24, 8)

	a.rsi = makeOperandRegister("rsi", 25, 64)
	a.esi = makeOperandRegister("esi", 26, 32)
	a.si = makeOperandRegister("si", 27, 16)
	a.sil = makeOperandRegister("sil", 28, 8)

	a.rbp = makeOperandRegister("rbp", 29, 64)
	a.ebp = makeOperandRegister("ebp", 30, 32)
	a.bp = makeOperandRegister("bp", 31, 16)
	a.bpl = makeOperandRegister("bpl", 32, 8)

	a.rsp = makeOperandRegister("rsp", 33, 64)
	a.esp = makeOperandRegister("esp", 34, 32)
	a.sp = makeOperandRegister("sp", 35, 16)
	a.spl = makeOperandRegister("spl", 36, 8)

	a.rip = makeOperandRegister("rip", 37, 64)
	a.eip = makeOperandRegister("eip", 38, 32)

	a.r8 = makeOperandRegister("r8", 39, 64)
	a.r8d = makeOperandRegister("r8d", 40, 32)
	a.r8w = makeOperandRegister("r8w", 41, 16)
	a.r8b = makeOperandRegister("r8b", 42, 8)

	a.r9 = makeOperandRegister("r9", 43, 64)
	a.r9d = makeOperandRegister("r9d", 44, 32)
	a.r9w = makeOperandRegister("r9w", 45, 16)
	a.r9b = makeOperandRegister("r9b", 46, 8)

	a.r10 = makeOperandRegister("r10", 47, 64)
	a.r10d = makeOperandRegister("r10d", 48, 32)
	a.r10w = makeOperandRegister("r10w", 49, 16)
	a.r10b = makeOperandRegister("r10b", 50, 8)

	a.r11 = makeOperandRegister("r11", 51, 64)
	a.r11d = makeOperandRegister("r11d", 52, 32)
	a.r11w = makeOperandRegister("r11w", 53, 16)
	a.r11b = makeOperandRegister("r11b", 54, 8)

	a.r12 = makeOperandRegister("r12", 55, 64)
	a.r12d = makeOperandRegister("r12d", 56, 32)
	a.r12w = makeOperandRegister("r12w", 57, 16)
	a.r12b = makeOperandRegister("r12b", 58, 8)

	a.r13 = makeOperandRegister("r13", 59, 64)
	a.r13d = makeOperandRegister("r13d", 60, 32)
	a.r13w = makeOperandRegister("r13w", 61, 16)
	a.r13b = makeOperandRegister("r13b", 62, 8)

	a.r14 = makeOperandRegister("r14", 63, 64)
	a.r14d = makeOperandRegister("r14d", 64, 32)
	a.r14w = makeOperandRegister("r14w", 65, 16)
	a.r14b = makeOperandRegister("r14b", 66, 8)

	a.r15 = makeOperandRegister("r15", 67, 64)
	a.r15d = makeOperandRegister("r15d", 68, 32)
	a.r15w = makeOperandRegister("r15w", 69, 16)
	a.r15b = makeOperandRegister("r15b", 70, 8)
end

local ASSEMBLER_META = {}
ASSEMBLER_META.__index = ASSEMBLER_META

function ASSEMBLER_META:init()
	self.Objects = {}
	self.Labels = {}
	initRegisters(self)
	initInstructions(self)
end

function ASSEMBLER_META:createLabel(name)
	local id = #self.Labels + 1
	local label = {
		id = id,
		name = name,
		index = -1,
		address = -1,
	}
	table.insert(self.Labels, label)
	return makeOperandLabel(id)
end

function ASSEMBLER_META:bindLabel(label)
	local labelData = self.Labels[label.id]
	if labelData == nil then
		error("Label not created within assembler")
		return
	end
	labelData.index = #self.Objects + 1
	local obj = {
		object = assembler.OBJECT_LABEL,
		id = label.id
	}
	table.insert(self.Objects, obj)
end

local function fixupOperand(op)
	if op ~= nil and type(op) == "number" then
		op = makeOperandImm(op)
	end
	return op
end

function ASSEMBLER_META:emit(mnemonicId, op1, op2, op3, op4, subId)
	op1 = fixupOperand(op1)
	op2 = fixupOperand(op2)
	op3 = fixupOperand(op3)
	op4 = fixupOperand(op4)
	local instr = {
		object = assembler.OBJECT_INSTRUCTION,
		mnemonicId = mnemonicId,
		subId = subId,
		op1 = op1,
		op2 = op2,
		op3 = op3,
		op4 = op4,
		address = -1,	-- We only get an offset after encoding.
	}
	table.insert(self.Objects, instr)
end

local function encodeOperandRegister(a, base, fixups, idx, data, op)
	return op.name
end

local function encodeOperandImm(a, base, fixups, idx, data, op)
	return tostring(op.val)
end

local function encodeOperandLabel(a, base, fixups, idx, data, op)
	local res
	local labelData = a.Labels[op.id]
	if labelData.address ~= -1 then
		res = string.format("%08X", labelData.address)
	else
		-- Temporary.
		res = "0x123456"
		table.insert(fixups, idx)
	end
	return res
end

local function encodeOperandMemory(a, fixups, idx, data, op)
	local res = ""
	
	-- Decoration
	if op.size == 8 then
		res = "byte ptr ["
	elseif op.size == 16 then
		res = "word ptr ["
	elseif op.size == 32 then
		res = "dword ptr ["
	elseif op.size == 64 then
		res = "qword ptr ["
	end
	
	local continued = false
	-- Registers
	if op.base ~= nil then
		res = res .. encodeOperandRegister(a, base, fixups, idx, data, op.base)
		continued = true
	end
	if op.index ~= nil then
		if continued then res = res .. "+" end
		res = res .. encodeOperandRegister(a, base, fixups, idx, data, op.index)
		continued = true
	end
	if op.scale ~= nil and op.scale ~= 1 then
		if continued then res = res .. "*" end
		res = res .. tostring(op.scale)
		continued = true
	end
	
	-- Disp
	if op.disp32 ~= nil then
		if continued then res = res .. "+" end
	end
	
	-- Label
	if op.label ~= nil then
		if continued then res = res .. "+" end
		res = res .. encodeOperandLabel(a, base, fixups, idx, data, op.label)
	end
	
	res = res .. "]"
	return res
end

local function encodeOperand(a, fixups, idx, data, op)
	if op.object == assembler.OP_REGISTER then
		return encodeOperandRegister(a, base, fixups, idc, data, op)
	elseif op.object == assembler.OP_IMM then
		return encodeOperandImm(a, base, fixups, idx, data, op)
	elseif op.object == assembler.OP_MEMORY then
		return encodeOperandMemory(a, base, fixups, idx, data, op)
	elseif op.object == assembler.OP_LABEL then
		return encodeOperandLabel(a, base, fixups, idx, data, op)
	end
end

local function encodeInstruction(a, base, fixups, idx, data)
	
	local instrData = assembler.INSTRUCTION_TABLE[data.mnemonicId]
	local mnemonics = splitString(instrData[1], "/")
	local mnemonic = mnemonics[data.subId]
	local instrStr = mnemonic
	
	if data.op1 ~= nil then
		instrStr = instrStr .. " " .. encodeOperand(a, fixups, idx, data, data.op1)
	end
	if data.op2 ~= nil then
		instrStr = instrStr .. ", " .. encodeOperand(a, fixups, idx, data, data.op2)
	end
	if data.op3 ~= nil then
		instrStr = instrStr .. ", " .. encodeOperand(a, fixups, idx, data, data.op3)
	end
	if data.op4 ~= nil then
		instrStr = instrStr .. ", " .. encodeOperand(a, fixups, idx, data, data.op4)
	end

	local data = assembler.encode(base, instrStr)
	
	return { data = data, readable = instrStr }
	
end

function ASSEMBLER_META:make(base, dumpOutput)
	
	local buffers = {}
	local dumps = {}
	local fixups = {}
	local currentOffset = 0
	
	for idx, data in pairs(self.Objects) do
		if data.object == assembler.OBJECT_INSTRUCTION then
			local res = encodeInstruction(self, base + currentOffset, fixups, idx, data)
			currentOffset = currentOffset + #res.data
			table.insert(buffers, res.data)
			if dumpOutput == true then
				table.insert(dumps, res.readable)
			end
		elseif data.object == assembler.OBJECT_LABEL then
			local labelData = self.Labels[data.id]
			labelData.address = base + currentOffset
			table.insert(buffers, "") -- Need to keep it aligned, 0 buffer.
			if dumpOutput == true then
				table.insert(dumps, labelData.name .. ":")
			end
		end
	end
	
	-- Every label should be bound now, creating fixups.
	for _, fixup in pairs(fixups) do
		-- Update buffer.
		local res = encodeInstruction(self, base + currentOffset, fixups, idx, self.Objects[fixup])
		buffers[fixup] = res.data
		if dumpOutput == true then
			dumps[fixup] = res.readable
		end
	end
	
	if dumpOutput == true then
		for _,v in pairs(dumps) do
			print(v)
		end
	end
	
	-- Combine all buffers into one output.
	local data = table.concat(buffers)
	return data

end

function ASSEMBLER_META:clear()
	self.Objects = {}
	self.Labels = {}
end

function assembler.create()
	local res = {}
	setmetatable(res, ASSEMBLER_META)
	res:init()
	return res
end