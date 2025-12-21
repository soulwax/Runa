# File: SConstruct
import os, subprocess
from SCons.Script import Action

def ensure_git_dep(name, url, ref='master'):
    path = os.path.join('vendor', name)
    if not os.path.isdir(path):
        print(f'Fetching {name} into {path}...')
        subprocess.check_call([
            'git', 'clone', '--depth', '1', '--branch', ref, url, path
        ])
    else:
        print(f'Updating {name} in {path}...')
        subprocess.check_call(['git', '-C', path, 'fetch', '--depth', '1', 'origin', ref])
        if ref.startswith('v') or ref.startswith('V'):
            subprocess.check_call(['git', '-C', path, 'reset', '--hard', ref])
        else:
            subprocess.check_call(['git', '-C', path, 'reset', '--hard', f'origin/{ref}'])
    return path

# ---------------------------------------------------------------------------
# Fetch deps into ./vendor
# ---------------------------------------------------------------------------

sdl3_dir       = ensure_git_dep('SDL',       'https://github.com/libsdl-org/SDL.git',       'main')
sdl3_image_dir = ensure_git_dep('SDL_image', 'https://github.com/libsdl-org/SDL_image.git', 'main')
yamlcpp_dir    = ensure_git_dep('yaml-cpp',  'https://github.com/jbeder/yaml-cpp.git',      'master')
spdlog_dir     = ensure_git_dep('spdlog',    'https://github.com/gabime/spdlog.git',        'v1.15.0')

# ---------------------------------------------------------------------------
# Project / toolchain
# ---------------------------------------------------------------------------

# Build options (plain `scons` → release/windows/64)
mode     = ARGUMENTS.get('mode', 'release')      # debug / release
platform = ARGUMENTS.get('platform', 'windows')  # windows / linux / macos ...
bits     = ARGUMENTS.get('bits', '64')           # 32 / 64

print(f'Building mode={mode} platform={platform} bits={bits}')

env = Environment(        # provided by SCons
    tools=['mingw'],
    CXX='g++',
    CXXFLAGS=['-std=c++20'],
    CPPPATH=[
        'src',
        os.path.join(spdlog_dir, 'include'),
        os.path.join(yamlcpp_dir, 'include'),
        os.path.join(sdl3_dir, 'include'),
        os.path.join(sdl3_image_dir, 'include'),
    ],
    # add LIBPATH when you actually build SDL/yaml-cpp as libs
)

# Platform-specific defines
if platform == 'windows':
    env.Append(CPPDEFINES=['RUNA2_PLATFORM_WINDOWS'])
elif platform == 'linux':
    env.Append(CPPDEFINES=['RUNA2_PLATFORM_LINUX'])

# Bits
if bits == '32':
    env.Append(CPPDEFINES=['RUNA2_32BIT'])
elif bits == '64':
    env.Append(CPPDEFINES=['RUNA2_64BIT'])

# mode → flags
if mode == 'debug':
    env.Append(CXXFLAGS=['-g', '-O0'])
elif mode == 'release':
    env.Append(CXXFLAGS=['-O3'])

third_party_libs = ['SDL3', 'SDL3_image', 'yaml-cpp', 'spdlog']

# ---------------------------------------------------------------------------
# Engine shared library (Runa2Engine)
# ---------------------------------------------------------------------------

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

engine_env = env.Clone()
engine_env.Append(CPPDEFINES=['RUNA2_ENGINE_EXPORTS'])

engine = engine_env.SharedLibrary(
    target='Runa2Engine',
    source=engine_sources,
    LIBS=third_party_libs,
)

# ---------------------------------------------------------------------------
# Game executable (Runa2)
# ---------------------------------------------------------------------------

game_env = env.Clone()
game_env.Append(LIBS=['Runa2Engine', 'spdlog'] + third_party_libs)

game = game_env.Program(
    target='Runa2',
    source=['src/main.cpp'],
)

# ---------------------------------------------------------------------------
# Post‑build DLL copying
# ---------------------------------------------------------------------------

def copy_dlls(target, source, env):
    import shutil
    exe_path = str(game[0])
    exe_dir = os.path.dirname(exe_path)

    dll_candidates = [
        'Runa2Engine.dll',
        'SDL3.dll',
        'SDL3_image.dll',
        'spdlog.dll',
        'libRuna2Engine.so',
        'libSDL3.so',
        'libSDL3_image.so',
        'libspdlog.so',
    ]

    for name in dll_candidates:
        if os.path.exists(name):
            shutil.copy(name, exe_dir)
    return 0

copy_action = Action(copy_dlls, "Copying DLLs to executable directory")
env.AddPostAction(game, copy_action)
env.AlwaysBuild(game)
