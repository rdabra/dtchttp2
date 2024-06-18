-- define toolchain
toolchain("myGcc")

    -- mark as standalone toolchain
    set_kind("standalone")

    -- set toolset
    set_toolset("cc", "/opt/gcc/bin/gcc")
    set_toolset("cxx", "/opt/gcc/bin/gcc", "/opt/gcc/bin/g++")
    set_toolset("ld", "/opt/gcc/bin/g++", "/opt/gcc/bin/gcc")
--    add_ldflags("-fuse-ld=mold") -- mold linker needs to be previously installed 
    set_toolset("sh", "/opt/gcc/bin/g++", "/opt/gcc/bin/gcc")
    set_toolset("ar", "ar")
    set_toolset("ex", "ar")
    set_toolset("strip", "strip")
    set_toolset("mm", "/opt/gcc/bin/gcc")
    set_toolset("mxx", "/opt/gcc/bin/gcc", "/opt/gcc/bin/g++")
    set_toolset("as", "/opt/gcc/bin/gcc")

    add_defines("MYCLANG")

    -- check toolchain
    on_check(function (toolchain)
        return import("lib.detect.find_tool")("gcc")
    end)

    -- on load
    on_load(function (toolchain)

        -- get march
        local march = is_arch("x86_64", "x64") and "-m64" or "-m32"

        -- init flags for c/c++
        toolchain:add("cxflags", march)
        toolchain:add("ldflags", march)
        toolchain:add("shflags", march)
        toolchain:add("includedirs", "/opt/gcc/lib/gcc/x86_64-pc-linux-gnu/14.0.1/include")
        toolchain:add("includedirs", "/opt/gcc/include")
        toolchain:add("includedirs", "/opt/gcc/lib/gcc/x86_64-pc-linux-gnu/14.0.1/include-fixed")
        toolchain:add("linkdirs", "/opt/gcc/lib/gcc/x86_64-pc-linux-gnu/14.0.1")
        toolchain:add("linkdirs", "/opt/gcc/lib64")


        -- init flags for objc/c++  (with ldflags and shflags)
        toolchain:add("mxflags", march)

        -- init flags for asm
        toolchain:add("asflags", march)
    end)
