# SConstruct
import os

env = Environment(  # pyright: ignore[reportUndefinedVariable]
    CXX='g++',
    CXXFLAGS=['-std=c++20'],
    CPPPATH=['src'],
)

# debug=1 on CLI → debug build, otherwise release
if ARGUMENTS.get('debug', '0') == '1':  # pyright: ignore[reportUndefinedVariable]
    env.Append(CXXFLAGS=['-g', '-O0'])
else:
    env.Append(CXXFLAGS=['-O3'])

engine_sources = [
    'src/runapch.cpp',
    'src/Core/Application.cpp',
    'src/Core/Log.cpp',
    'src/Core/ResourceManager.cpp',
    'src/Graphics/Window.cpp',
    'src/Graphics/Renderer.cpp',
    'src/Graphics/Shader.cpp',
    'src/Graphics/Texture.cpp',
    'src/Graphics/SpriteSheet.cpp',
    'src/Graphics/SpriteBatch.cpp',
    'src/Graphics/TileMap.cpp',
]

engine = env.SharedLibrary(
    target='Runa2Engine',
    source=engine_sources,
    CPPDEFINES=['RUNA2_ENGINE_EXPORTS'],
)

game = env.Program(
    target='Runa2',
    source=['src/main.cpp'],
    LIBS=['Runa2Engine', 'spdlog', 'SDL3', 'SDL3_image', 'yaml-cpp'],
)

# post‑build DLL copy (very rough Windows example)
def copy_dlls(target, source, env):
    import shutil
    exe_dir = os.path.dirname(str(game[0]))
    for dll in ['Runa2Engine.dll', 'SDL3.dll', 'SDL3_image.dll', 'spdlog.dll']:
        if os.path.exists(dll):
            shutil.copy(dll, exe_dir)
    return 0

env.AlwaysBuild(env.Command('copy_dlls', game, copy_dlls))
