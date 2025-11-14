#!/bin/bash

# 构建demo密钥环程序脚本

echo "开始构建demo密钥环程序..."

# 创建构建目录
mkdir -p build_demo
cd build_demo

# 使用CMake配置项目
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "编译程序..."
make -j$(nproc)

# 检查是否编译成功
if [ $? -eq 0 ]; then
    echo "构建成功！"
    echo "可执行文件位于: build_demo/demo_keyring"
    echo ""
    echo "使用方法:"
    echo "  ./demo_keyring 0 <当前密码>    # 清空密钥环"
    echo "  ./demo_keyring 1 <新密码>      # 恢复密钥环"
    echo ""
    echo "示例:"
    echo "  ./demo_keyring 0 mypassword    # 清空密钥环（需要当前密码）"
    echo "  ./demo_keyring 1 newpassword   # 恢复密钥环（设置新密码）"
else
    echo "构建失败！"
    exit 1
fi