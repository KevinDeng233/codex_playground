# Windows 便携版构建结果

2026-11-01，已按用户授权完成独立分支上传与云端构建。

- 分支：`airpods/pro3-hardware-test`，main 未修改。
- 构建源码提交：`9858bb03204b147bf11ab6bf0b20c402f8927422`。
- [GitHub Actions 运行 #1](https://github.com/KevinDeng233/codex_playground/actions/runs/41268074739)：Success，总耗时 9 分 41 秒。
- Windows 2022 / Visual Studio 2022 x64 / Qt 6.8.3。
- 构建脚本的 CMake 编译与 CTest 成功退出，打包脚本随后成功执行。
- 便携包产物：`airpods-evaluation-win11-x64`，GitHub 显示大小 70.6 MB。
- 自动测试日志：`windows-evaluation-test-logs`，GitHub 显示大小 3.57 KB。

没有生成安装器，没有在用户电脑安装或运行应用，没有发布正式 Release。

## 下载和首次测试

1. 登录自己的 GitHub 账户，打开上方成功的运行记录。
2. 滚动到 Artifacts，下载 `airpods-evaluation-win11-x64`。
3. 解压外层产物 ZIP，得到应用便携 ZIP 及 `.sha256` 文件，再解压应用 ZIP。
4. 先在 Windows 11 蓝牙设置中配对 AirPods Pro 3，再运行解压目录中的 `AirPodsDesktop.exe`。不需要 VS Code、Visual Studio、Qt 开发环境或管理员权限。
5. 首次询问开机启动和低延迟功能时均选“否”，在程序内绑定 AirPods。
6. 先反馈三项结果：能否启动并绑定、设备名称是否正确、左右耳及充电盒是否获得电量。读取不到的槽位应为“—”，不是 0%。

便携应用沿用上游的用户设置和日志存储方式，解压运行不等于完全不写入用户配置。如果已经装有 AirPodsDesktop，请先告知，再安排配置隔离。

## 证据边界

已验证 Windows 构建和 CI 自动测试；尚未验证用户的 Windows 11 蓝牙适配器或 Pro 3 / 9B43a 固件。当前环境的 GitHub API 被网络策略拒绝，详细日志及产物下载要求登录；本次成功状态、耗时与两个产物名称/大小来自 GitHub 公开运行摘要。未声称已经在此云端下载或解压检查 Windows 产物，也未虚报具体执行用例数。

应用 ZIP 的校验值以随包 `.sha256` 文件为准。GitHub 页面上的 artifact digest 用于产物整体，不应直接当成解压后应用 ZIP 的 SHA-256。
