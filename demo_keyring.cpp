#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>

// 直接包含libsecret头文件
#include <libsecret/secret.h>
#include <gio/gio.h>

// 密钥环操作函数
bool setKeyringPassword(const std::string& currentPassword, const std::string& newPassword) {
    GError *err = nullptr;
    SecretService *service = nullptr;
    GDBusConnection *bus = nullptr;
    SecretValue *currentValue = nullptr;
    SecretValue *newPassValue = nullptr;
    GVariant *ret = nullptr;
    
    const char *PasswordSecretValueContentType = "text/plain";
    const char *LoginKeyringPath = "/org/freedesktop/secrets/collection/login";

    bool success = false;

    std::cout << "开始设置密钥环密码..." << std::endl;
    std::cout << "当前密码: " << (currentPassword.empty() ? "[空密码]" : "***") << std::endl;
    std::cout << "新密码: " << (newPassword.empty() ? "[空密码]" : "***") << std::endl;

    do {
        // 获取secret服务
        service = secret_service_get_sync(SECRET_SERVICE_OPEN_SESSION, nullptr, &err);
        if (service == nullptr) {
            std::cerr << "错误：无法获取secret服务: " << err->message << std::endl;
            break;
        }

        // 获取默认密钥环集合
        SecretCollection *collection = secret_collection_for_alias_sync(service,
                                                                        SECRET_COLLECTION_DEFAULT,
                                                                        SECRET_COLLECTION_NONE,
                                                                        NULL,
                                                                        &err);
        if (err != nullptr) {
            std::cerr << "错误：无法获取默认密钥环集合: " << err->message << std::endl;
            break;
        }
        if (collection == nullptr) {
            std::cout << "提示：默认密钥环集合不存在" << std::endl;
            break;
        }

        // 创建密码值对象
        currentValue = secret_value_new(currentPassword.c_str(), currentPassword.length(), PasswordSecretValueContentType);
        newPassValue = secret_value_new(newPassword.c_str(), newPassword.length(), PasswordSecretValueContentType);

        // 获取DBus连接
        bus = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &err);
        if (bus == nullptr) {
            std::cerr << "错误：无法获取会话总线: " << err->message << std::endl;
            break;
        }

        // 调用密钥环密码修改接口
        ret = g_dbus_connection_call_sync(bus,
                                          "org.gnome.keyring",
                                          "/org/freedesktop/secrets",
                                          "org.gnome.keyring.InternalUnsupportedGuiltRiddenInterface",
                                          "ChangeWithMasterPassword",
                                          g_variant_new("(o@(oayays)@(oayays))",
                                                        LoginKeyringPath,
                                                        secret_service_encode_dbus_secret(service, currentValue),
                                                        secret_service_encode_dbus_secret(service, newPassValue)),
                                          nullptr,
                                          G_DBUS_CALL_FLAGS_NONE,
                                          G_MAXINT,
                                          nullptr,
                                          &err);
        if (err != nullptr) {
            std::cerr << "错误：无法修改密钥环密码: " << err->message << std::endl;
            break;
        }

        success = true;
        std::cout << "密钥环密码设置成功！" << std::endl;

    } while (false);

    // 清理资源
    if (err != nullptr) g_error_free(err);
    if (service != nullptr) g_object_unref(service);
    if (bus != nullptr) g_object_unref(bus);
    if (currentValue != nullptr) secret_value_unref(currentValue);
    if (newPassValue != nullptr) secret_value_unref(newPassValue);
    if (ret != nullptr) g_variant_unref(ret);

    return success;
}

// 清空密钥环密码
bool emptyKeyringPassword(const std::string& currentPassword) {
    std::cout << "=== 执行清空密钥环操作 ===" << std::endl;
    return setKeyringPassword(currentPassword, "");
}

// 恢复密钥环密码
bool restoreKeyringPassword(const std::string& newPassword) {
    std::cout << "=== 执行恢复密钥环操作 ===" << std::endl;
    return setKeyringPassword("", newPassword);
}

// 显示使用说明
void showUsage(const char *programName) {
    std::cout << "密钥环操作演示程序" << std::endl;
    std::cout << "用法: " << programName << " <模式> [密码]" << std::endl;
    std::cout << std::endl;
    std::cout << "模式参数:" << std::endl;
    std::cout << "  0 - 清空密钥环密码（将当前密码设置为空）" << std::endl;
    std::cout << "  1 - 恢复密钥环密码（将空密码恢复为指定密码）" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << " 0 mypassword    # 清空密钥环（需要当前密码）" << std::endl;
    std::cout << "  " << programName << " 1 newpassword   # 恢复密钥环（设置新密码）" << std::endl;
    std::cout << std::endl;
    std::cout << "注意：" << std::endl;
    std::cout << "  - 此操作需要适当的系统权限" << std::endl;
    std::cout << "  - 清空操作需要知道当前密码" << std::endl;
    std::cout << "  - 恢复操作需要提供新密码" << std::endl;
}

int main(int argc, char *argv[]) {
    // 检查命令行参数
    if (argc < 2) {
        showUsage(argv[0]);
        return 1;
    }
    
    // 解析模式参数
    int mode = -1;
    try {
        mode = std::stoi(argv[1]);
    } catch (const std::exception &e) {
        std::cerr << "错误：无效的模式参数 '" << argv[1] << "'" << std::endl;
        showUsage(argv[0]);
        return 1;
    }
    
    // 获取密码参数
    std::string password;
    if (argc >= 3) {
        password = argv[2];
    } else {
        // 如果没有提供密码，使用空密码
        std::cout << "提示：未提供密码参数，将使用空密码" << std::endl;
        password = "";
    }
    
    // 根据模式执行相应操作
    bool success = false;
    
    switch (mode) {
        case 0:
            success = emptyKeyringPassword(password);
            break;
        case 1:
            success = restoreKeyringPassword(password);
            break;
        default:
            std::cerr << "错误：无效的操作模式。请使用 0（清空）或 1（恢复）" << std::endl;
            showUsage(argv[0]);
            return 1;
    }
    
    if (success) {
        std::cout << std::endl << "操作成功完成！" << std::endl;
        return 0;
    } else {
        std::cerr << std::endl << "操作失败！" << std::endl;
        return 1;
    }
}