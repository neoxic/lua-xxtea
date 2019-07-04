/*
** Copyright (C) 2019 Arseny Vakhrushev <arseny.vakhrushev@gmail.com>
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

#include <stdint.h>
#include <string.h>
#include <lauxlib.h>

#define MODNAME "lua-xxtea"
#define VERSION "0.1.1"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

EXPORT int luaopen_xxtea(lua_State *L);

/*******************************************************
** Corrected Block TEA (XXTEA) implementation for Lua **
** https://en.wikipedia.org/wiki/XXTEA                **
*******************************************************/

#define DELTA 0x9e3779b9
#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z)))

static void encrypt(uint32_t *v, unsigned n, const uint32_t *k) {
	unsigned p, q = 6 + 52 / n, e;
	uint32_t y, z = v[n - 1], sum = 0;
	while (q--) {
		sum += DELTA;
		e = (sum >> 2) & 3;
		for (p = 0; p < n - 1; ++p) {
			y = v[p + 1];
			z = v[p] += MX;
		}
		y = v[0];
		z = v[n - 1] += MX;
	}
}

static void decrypt(uint32_t *v, unsigned n, const uint32_t *k) {
	unsigned p, q = 6 + 52 / n, e;
	uint32_t y = v[0], z, sum = q * DELTA;
	while (q--) {
		e = (sum >> 2) & 3;
		for (p = n - 1; p > 0; --p) {
			z = v[p - 1];
			y = v[p] -= MX;
		}
		z = v[n - 1];
		y = v[0] -= MX;
		sum -= DELTA;
	}
}

static void adjust(uint32_t *v, unsigned n) {
	union { int i; char c; } t;
	union { uint32_t u; char c[4]; } u;
	t.i = 1;
	if (t.c) return; /* Little-endian machine */
	while (n--) {
		u.u = *v;
		memcpy(v++, u.c, 4);
	}
}

static int f_encrypt(lua_State *L) {
	size_t slen, klen, dlen;
	const char *str = luaL_checklstring(L, 1, &slen);
	const char *key = luaL_checklstring(L, 2, &klen);
	char *dbuf, kbuf[16];
	uint32_t *v, *k;
	unsigned n = (slen + 3) / 4 + 1;
	luaL_argcheck(L, slen, 1, "empty data");
	luaL_argcheck(L, klen <= 16, 2, "key cannot be more than 16 bytes long");
	memcpy(dbuf = lua_newuserdata(L, dlen = n * 4), str, slen);
	memset(dbuf + slen, 0, dlen - slen);
	memcpy(kbuf, key, klen);
	memset(kbuf + klen, 0, 16 - klen);
	adjust(v = (uint32_t *)dbuf, n);
	adjust(k = (uint32_t *)kbuf, 4);
	v[n - 1] = slen;
	encrypt(v, n, k);
	adjust(v, n);
	lua_pushlstring(L, dbuf, dlen);
	return 1;
}

static int f_decrypt(lua_State *L) {
	size_t slen, klen, dlen;
	const char *str = luaL_checklstring(L, 1, &slen);
	const char *key = luaL_checklstring(L, 2, &klen);
	char *dbuf, kbuf[16];
	uint32_t *v, *k;
	unsigned n = slen / 4;
	luaL_argcheck(L, slen >= 8 && slen == n * 4, 1, "malformed data");
	luaL_argcheck(L, klen <= 16, 2, "key cannot be more than 16 bytes long");
	memcpy(dbuf = lua_newuserdata(L, slen), str, slen);
	memcpy(kbuf, key, klen);
	memset(kbuf + klen, 0, 16 - klen);
	adjust(v = (uint32_t *)dbuf, n);
	adjust(k = (uint32_t *)kbuf, 4);
	decrypt(v, n, k);
	dlen = v[n - 1];
	adjust(v, n);
	if (dlen < slen - 7 || dlen > slen - 4) luaL_error(L, "data integrity check failed");
	lua_pushlstring(L, dbuf, dlen);
	return 1;
}

static const luaL_Reg l_xxtea[] = {
	{"encrypt", f_encrypt},
	{"decrypt", f_decrypt},
	{0, 0}
};

int luaopen_xxtea(lua_State *L) {
#if LUA_VERSION_NUM < 502
	luaL_register(L, lua_tostring(L, 1), l_xxtea);
#else
	luaL_newlib(L, l_xxtea);
#endif
	lua_pushliteral(L, MODNAME);
	lua_setfield(L, -2, "_NAME");
	lua_pushliteral(L, VERSION);
	lua_setfield(L, -2, "_VERSION");
	return 1;
}
