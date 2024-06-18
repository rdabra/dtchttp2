add_rules("mode.debug", "mode.release")
includes("my-xmake/toolchains/myClang.lua")
set_toolchains("myClang")
set_languages("cxx20")

add_requires("linux-tools", {configs = {bpftool = true}})
add_requires("libbpf",{system=true})
add_requires("libelf",{system=true})
add_requires("libzmf",{system=true})

-- fix error: libbpf: map 'my_pid_map': unsupported map linkage static. for bpftool >= 7.2.0
-- we cannot add `"-fvisibility=hidden"` when compiling *.bpf.c
-- @see https://github.com/libbpf/bpftool/issues/120
set_symbols("none")

includes("my-xmake/rules/co_re-bpf.lua")
local current_dir = os.scriptdir()
target("dtchttp2")
   add_rules("co_re-bpf")
   set_kind("binary")
   add_files("*.c","*.cpp")
   add_packages("linux-tools", "libbpf","libelf","libzmf", "linux-headers")
   set_license("GPL-2.0")
   add_includedirs(current_dir)
