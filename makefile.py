import powermake

def on_build(config: powermake.Config):
    config.c_compiler = powermake.compilers.CompilerGNU("/usr/lib/emscripten/emcc")
    config.linker = powermake.linkers.LinkerGNU("/usr/lib/emscripten/emcc")

    config.add_flags("-Wall", "-Wextra")

    files = powermake.get_files("src/*.c")

    objects = powermake.compile_files(config, files)

    config.add_shared_libs("websocket.js")

    powermake.link_files(config, objects)


def on_install(config: powermake.Config, location: str | None):
    if location is None:
        location = "./http_public/"
    powermake.default_on_install(config, location)

powermake.run("websocket.js", build_callback=on_build, install_callback=on_install)
