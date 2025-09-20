#!/usr/bin/env bash
set -euo pipefail

# 스크립트 파일이 위치한 디렉토리의 절대 경로를 찾습니다.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# 상위 디렉토리의 build 폴더를 SEAL 라이브러리 경로로 지정합니다.
SEAL_BUILD_DIR="${SCRIPT_DIR}/../externals/MS-SEAL/build"

echo "Found SEAL build directory at: ${SEAL_BUILD_DIR}"

cmake -S . -B build -DCMAKE_PREFIX_PATH="${SEAL_BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build build
exec build/bin/test_file