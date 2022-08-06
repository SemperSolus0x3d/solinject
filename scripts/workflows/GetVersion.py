import re

def get_version():
    with open('CMakeLists.txt', 'r') as f:
        cmakelists = f.read()

    match = re.search(
        'project\(\s*solinject.+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+).*\)',
        cmakelists
    )

    return match.group(1)

print(f'::set-output name=version::{get_version()}')
