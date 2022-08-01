#!/usr/bin/python3

import os
import re
import shutil as sh
from os.path import realpath, dirname, join
from dataclasses import dataclass

@dataclass
class Platform:
    name: str
    compiler_flag: str
    lib_dir: str

PLATFORMS = [
    Platform(name='x32', compiler_flag='-m32', lib_dir='lib32'),
    Platform(name='x64', compiler_flag='-m64', lib_dir='lib64'),
]

def get_version():
    with open('CMakeLists.txt', 'r') as f:
        cmakelists = f.read()

    match = re.search(
        'project\(\s*solinject.+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+).*\)',
        cmakelists
    )

    return match.group(1)

def main():
    project_dir = realpath(join(dirname(realpath(__file__)), '..'))
    os.chdir(project_dir)

    version = get_version()

    for platform in PLATFORMS:
        os.environ['CFLAGS'] = os.environ['CXXFLAGS'] = platform.compiler_flag

        build_dir = f'build-{platform.name}'
        install_prefix = join('install', 'all-platforms', f'solinject-v{version}', platform.lib_dir)

        sh.rmtree(build_dir, ignore_errors=True)
        sh.rmtree(install_prefix, ignore_errors=True)

        os.system(f'cmake -S . -B {build_dir} -DBUILD_TESTING=0')
        os.system(f'cmake --build {build_dir}')
        os.system(f'cmake --install {build_dir} --prefix {install_prefix}')

        sh.rmtree(build_dir, ignore_errors=True)

main()
