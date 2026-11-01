# AirPods Windows 11 弹窗 · 评估版

基于 [SpriteOvO/AirPodsDesktop](https://github.com/SpriteOvO/AirPodsDesktop) 改进的 C++20 / Qt 6 原生 Windows 项目。保留 GPL-3.0-or-later 许可与上游版权声明。

- [改进与验证报告](review/REPORT.md)
- [离线交互效果稿](review/preview.html)：在浏览器打开，无需安装；使用模拟数据。
- [原生应用源码](airpods-desktop/)
- [上游版本及来源](airpods-desktop/UPSTREAM.md)
- [Windows 构建说明](airpods-desktop/Docs/Build.md)

当前完成了源码改进、连接状态逻辑测试及效果稿交互测试。**尚未通过 Windows 构建及 AirPods 真机验证，没有可宣称验收通过的 Windows 安装包。** 未安装到用户电脑，未运行应用，未推送或发布。

Windows 11 x64 开发环境中，可使用下列命令仅构建和测试：

```powershell
./airpods-desktop/Tools/Build-Evaluation.ps1 -QtRoot C:\Qt\6.8.3\msvc2022_64 -VcpkgRoot C:\vcpkg
```

前置条件：Visual Studio 2022 C++ 桌面工具、CMake 3.20+、Qt 6.8.3（含 Multimedia）、已 bootstrap 的 vcpkg。路径按实际安装位置填写。脚本不会安装、启动应用或设置开机自启。GUI 测试会创建测试窗口，需要可交互的 Windows 桌面。

仓库另含 Windows CI 定义，可手动触发，或由专用 `airpods/pro3-hardware-test` 分支的推送触发；本次尚未推送或触发 CI。

## 实机测试准备

见 [实机测试步骤](review/HARDWARE-TEST.md) 和 [只读 Windows 预检脚本](review/Test-WindowsPrerequisites.ps1)。

Windows CI 在构建与全部自动测试通过后导出便携 ZIP 和 SHA-256；当前尚未实际运行该流程。也可在本地构建测试通过后执行 `airpods-desktop/Tools/Package-Evaluation.ps1` 导出便携包。打包脚本不会重新运行测试，不应在构建或测试失败时使用。
