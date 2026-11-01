适用于 **Windows 11 x64** 的 AirPods 弹窗便携版，基于 [AirPodsDesktop](https://github.com/SpriteOvO/AirPodsDesktop) 改进，采用 GPL-3.0-or-later 开源许可。此为改进版预发行版本，并非上游官方发行版。

### 功能

- 显示 Windows 配对设备名称、连接状态、左右耳和充电盒电量。
- 分开处理蓝牙连接与电量数据，避免将“暂时收不到电量”误报为“已断开”。
- 连接时弹窗，保留设备动画、深浅主题和中文界面。
- 缺失电量显示“—”；真实 0% 与无数据分别处理。

### 已验证

Windows x64 云端构建与自动测试通过：[构建记录](https://github.com/KevinDeng233/codex_playground/actions/runs/41268074739)。本版使用该次构建的原始程序二进制，发布时仅补充说明和许可文件。

用户实测 **AirPods Pro 3 / 固件 9B43a**：设备名、连接状态、左右耳电量正常；耳机放回充电盒后，充电盒电量正常显示。

充电盒电量不保证在耳机取出或合盖后持续可读。“—”表示暂无有效数据，不代表没电。其他型号、蓝牙适配器、睡眠唤醒和多设备干扰场景尚未全面验收。暂不支持 Windows ARM64。

### 下载和使用

1. 下载下方 `AirPodsDesktop-v0.6.1-pro3.1-win11-x64-portable.zip` 并解压整个目录。
2. 在 Windows 蓝牙设置中配对 AirPods，再运行 `AirPodsDesktop.exe` 并绑定设备。
3. 首次询问开机启动或低延迟功能时，可选择“否”。无需安装 Visual Studio 或 Qt 开发环境。

这是便携包，没有安装器；应用仍会保存用户设置与日志。已有 AirPodsDesktop 用户应先备份原有设置。程序没有代码签名；遇到系统拦截请先核对来源和 SHA-256，不必关闭系统安全功能。

`SHA256SUMS.txt` 提供下载文件校验值。对应应用源码随本标签的 Source code 归档提供；上游来源及许可证也包含在包内。
