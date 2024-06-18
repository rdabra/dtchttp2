-- define toolchain
toolchain("myClang")

    -- mark as standalone toolchain
    set_kind("standalone")

    -- set toolset
    set_toolset("cc", "/opt/clang/bin/clang")
    set_toolset("cxx", "/opt/clang/bin/clang", "/opt/clang/bin/clang++")
    set_toolset("ld", "/opt/clang/bin/clang++", "/opt/clang/bin/clang")
    add_ldflags("-fuse-ld=mold") -- mold linker needs to be previously installed 
    set_toolset("sh", "/opt/clang/bin/clang++", "/opt/clang/bin/clang")
    set_toolset("ar", "ar")
    set_toolset("ex", "ar")
    set_toolset("strip", "strip")
    set_toolset("mm", "/opt/clang/bin/clang")
    set_toolset("mxx", "/opt/clang/bin/clang", "/opt/clang/bin/clang++")
    set_toolset("as", "/opt/clang/bin/clang")

    add_defines("MYCLANG")

    -- check toolchain
    on_check(function (toolchain)
        return import("lib.detect.find_tool")("clang")
    end)

    -- on load
    on_load(function (toolchain)

        -- get march
        local march = is_arch("x86_64", "x64") and "-m64" or "-m32"

        -- init flags for c/c++
        toolchain:add("cxflags", march)
        toolchain:add("ldflags", march)
        toolchain:add("shflags", march)
        toolchain:add("includedirs", "/opt/clang/include")
        toolchain:add("linkdirs", "/opt/clang/lib")

        -- init flags for objc/c++  (with ldflags and shflags)
        toolchain:add("mxflags", march)

        -- init flags for asm
        toolchain:add("asflags", march)
    end)
