#!/bin/bash
set -e
# 工程根目录
WORK_DIR="$HOME/lvgl/watch"
BUILD_DIR="${WORK_DIR}/build"
BIN_DIR="${WORK_DIR}/bin"
TARGET="${BIN_DIR}/main"

# 进入工程目录，失败直接退出
cd "${WORK_DIR}" || exit 1

# 判断可执行文件存在则删除
if [ -f "${TARGET}" ]; then
    echo "main可执行文件存在,进行删除"
    rm -f "${TARGET}"
fi

if [ -d "${BUILD_DIR}" ]; then
    echo "build目录已存在,清空旧构建缓存"
    rm -rf "${BUILD_DIR}"
fi
echo "创建全新build目录"
mkdir -p "${BUILD_DIR}" || exit 1

# 进入编译目录
cd "${BUILD_DIR}" || exit 1

# 编译
cmake ..
make -j128

# 返回根目录运行程序
cd "${WORK_DIR}" || exit 1
echo "编译完成，启动程序..."
./bin/main
