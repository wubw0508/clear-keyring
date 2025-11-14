# 密钥环操作演示程序

这是一个基于GNOME密钥环的演示程序，可以清空或恢复密钥环密码。

## 功能说明

- **清空密钥环** (模式 0): 将当前密钥环密码设置为空密码
- **恢复密钥环** (模式 1): 将空密钥环密码恢复为指定密码

## 构建方法

1. 确保系统已安装必要的开发库：
   ```bash
   sudo apt-get install libsecret-1-dev libglib2.0-dev libgio2.0-dev cmake build-essential
   ```

2. 构建程序：
   ```bash
   chmod +x build_demo.sh
   ./build_demo.sh
   ```

## 使用方法

### 基本语法
```bash
./demo_keyring <模式> [密码]
```

### 模式说明

- **模式 0 (清空密钥环)**: 
  ```bash
  ./demo_keyring 0 <当前密码>
  ```
  将当前密钥环密码设置为空密码。需要提供当前密码。

- **模式 1 (恢复密钥环)**:
  ```bash
  ./demo_keyring 1 <新密码>
  ```
  将空密钥环密码恢复为指定密码。需要提供新密码。

### 示例

1. 清空密钥环（当前密码为"mypassword"）:
   ```bash
   ./demo_keyring 0 mypassword
   ```

2. 恢复密钥环（设置新密码为"newpassword"）:
   ```bash
   ./demo_keyring 1 newpassword
   ```

3. 查看帮助信息:
   ```bash
   ./demo_keyring
   ```

## 注意事项

1. **权限要求**: 此操作需要适当的系统权限，可能需要以管理员身份运行。

2. **密码安全**: 
   - 清空操作需要知道当前密码
   - 恢复操作需要提供新密码
   - 建议在生产环境中使用强密码

3. **风险提示**: 
   - 清空密钥环密码可能会影响依赖密钥环的应用程序
   - 请确保了解操作后果后再执行

4. **测试环境**: 建议先在测试环境中验证功能

## 技术实现

程序使用libsecret库直接与GNOME密钥环交互，通过DBus调用密钥环的密码修改接口：
- `org.gnome.keyring.InternalUnsupportedGuiltRiddenInterface.ChangeWithMasterPassword`

## 文件说明

- `demo_keyring.cpp` - 演示程序源代码
- `CMakeLists_demo.txt` - CMake构建配置
- `build_demo.sh` - 构建脚本
- `README_demo.md` - 使用说明文档

## 依赖库

- libsecret-1-dev
- glib-2.0-dev
- gio-2.0-dev
- cmake
- build-essential