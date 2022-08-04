#!/usr/bin/python3

# SPDX-License-Identifier: LGPL-3.0-or-later

# solinject - C++ Dependency Injection header-only library
# Copyright (C) 2022  SemperSolus0x3d
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
