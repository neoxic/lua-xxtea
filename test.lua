local xxtea = require 'xxtea'

local function randstr(n)
	local t = {}
	for i = 1, math.random(0, n) do
		t[i] = string.char(math.random(0, 255))
	end
	return table.concat(t)
end

math.randomseed(os.time())

for i = 1, 1000 do
	local str = randstr(10000)
	local key = randstr(16)
	assert(xxtea.decrypt(xxtea.encrypt(str, key), key) == str)
end
