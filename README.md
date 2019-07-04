XXTEA encryption/decryption module for Lua
==========================================

[lua-xxtea] encrypts/decrypts binary strings with 128 bit keys (16 bytes long binary strings).
When a key is less than 16 bytes long, it is assumed to have trailing zeroes ('\0').

```Lua
local xxtea = require 'xxtea'

local str1 = 'Hello, world!'
local key = 'abc123'

local str2 = xxtea.encrypt(str1, key)
local str3 = xxtea.decrypt(str2, key)

assert(str1 ~= str2)
assert(str1 == str3)
```


Building and installing with LuaRocks
-------------------------------------

To build and install, run:

    luarocks make
    luarocks test

To install the latest release using [luarocks.org], run:

    luarocks install lua-xxtea


[lua-xxtea]: https://github.com/neoxic/lua-xxtea
[luarocks.org]: https://luarocks.org
