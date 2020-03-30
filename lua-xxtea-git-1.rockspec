package = 'lua-xxtea'
version = 'git-1'
source = {
	url = 'git://github.com/neoxic/lua-xxtea.git',
}
description = {
	summary = 'XXTEA encryption/decryption module for Lua',
	license = 'MIT',
	homepage = 'https://github.com/neoxic/lua-xxtea',
	maintainer = 'Arseny Vakhrushev <arseny.vakhrushev@me.com>',
}
dependencies = {
	'lua >= 5.1',
}
build = {
	type = 'builtin',
	modules = {
		xxtea = {
			sources = {
				'xxtea.c',
			},
		},
	},
}
