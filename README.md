# AirPods Windows 11 弹窗 · 评估版

基于 [SpriteOvO/AirPodsDesktop](https://github.com/SpriteOvO/AirPodsDesktop) 改进的 C++20 / Qt 6 原生 Windows 项目。保留 GPL-3.0-or-later 许可与上游版权声明。

- [改进与验证报告](review/REPORT.md)
- [离线交互效果稿](review/preview.html)：在浏览器打开，无需安装；使用模拟数据。
- [原生应用源码](airpods-desktop/)
- [上游版本及来源](airpods-desktop/UPSTREAM.md)
- [Windows 构建说明](airpods-desktop/Docs/Build.md)

已完成源码改进、连接状态逻辑测试及效果稿交互测试；[Windows CI 构建与自动测试](https://github.com/KevinDeng233/codex_playground/actions/runs/41268074739) 已通过并生成便携包。**AirPods Pro 3 / 9B43a 真机验证尚未完成。** 已获授权推送独立测试分支，main 未修改，未安装到用户电脑、未发布正式版本。

Windows 11 x64 开发环境中，可使用下列命令仅构建和测试：

```powershell
./airpods-desktop/Tools/Build-Evaluation.ps1 -QtRoot C:\Qt\6.8.3\msvc2022_64 -VcpkgRoot C:\vcpkg
```

前置条件：Visual Studio 2022 C++ 桌面工具、CMake 3.20+、Qt 6.8.3（含 Multimedia）、已 bootstrap 的 vcpkg。路径按实际安装位置填写。脚本不会安装、启动应用或设置开机自启。GUI 测试会创建测试窗口，需要可交互的 Windows 桌面。

仓库另含 Windows CI 定义，可手动触发，或由专用 `airpods/pro3-hardware-test` 分支的推送触发；测试分支已推送并成功运行 CI。

## 实机测试准备

见 [实机测试步骤](review/HARDWARE-TEST.md) 和 [只读 Windows 预检脚本](review/Test-WindowsPrerequisites.ps1)。

Windows CI 在构建与全部自动测试通过后导出便携 ZIP 和 SHA-256；该流程已成功运行，下载入口见 [构建结果](review/WINDOWS-BUILD-RESULT.md)。也可在本地构建测试通过后执行 `airpods-desktop/Tools/Package-Evaluation.ps1` 导出便携包。打包脚本不会重新运行测试，不应在构建或测试失败时使用。
