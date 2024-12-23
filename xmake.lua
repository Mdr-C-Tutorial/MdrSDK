set_project("MdrOSSDK")

add_rules("mode.debug", "mode.release")
add_requires("zig", "nasm")

set_arch("i386")
set_languages("c23")
set_optimize("fastest")
set_warnings("all", "extra")
set_policy("run.autobuild", true)
set_policy("check.auto_ignore_flags", false)

add_cflags("-target x86-freestanding")
add_arflags("-target x86-freestanding")
add_ldflags("-target x86-freestanding")
add_cflags("-mno-sse", "-mno-sse2")

target("lib")
    set_kind("static")
    set_toolchains("@zig","nasm")
    set_default(false)
    add_includedirs("lib/include")
    add_files("lib/*.c")

target("source")
    add_deps("lib")
    set_kind("binary")
    set_toolchains("@zig","nasm")
    set_default(false)
    add_includedirs("lib/include")
    add_files("src/*.c")
    add_asflags("-f", "elf32")

    add_ldflags("-T","linker.ld")
    add_ldflags("-static")

target("MdrOSSDK")
    set_default(true)
    add_deps("source")

    on_build(function (target)
        import("core.project.project")
        local elffile = project.target("source")
        os.cp(elffile:targetfile(),"app.elf")
    end)
