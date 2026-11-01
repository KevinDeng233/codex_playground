# Windows 11 / AirPods 实机测试

Windows 构建与 CI 自动测试已通过，[下载本次便携包](https://github.com/KevinDeng233/codex_playground/actions/runs/41268074739)。本次目标设备为 AirPods Pro 3 / 9B43a，硬件兼容性仍待实际运行验证。打包脚本已在 Windows CI 成功执行；预检脚本只随包提供，尚未在用户电脑执行。不要用上游发行版或 HTML 效果稿替代本次修改版。

## 第一步：收集必要信息

确认 AirPods 具体型号、Windows 11 的 x64 / ARM64 架构、是否已有 Visual Studio 2022 C++ / Qt 6.8.3 / vcpkg。当前源码仅支持 x64。

可下载 `Test-WindowsPrerequisites.ps1` 到测试电脑，在其所在目录打开普通 PowerShell：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Test-WindowsPrerequisites.ps1
```

这个命令只对该进程允许运行脚本，不修改永久执行策略。脚本只读系统与设备状态，仅写入当前目录的 `airpods-preflight.json`，不会安装、配对、连接设备或启动程序。不要求管理员权限；部分设备查询受系统限制时，会记录 `query-unavailable`，不能据此判断蓝牙损坏。

如果 AirPods 被完全改名，不再含 AirPods，可使用：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Test-WindowsPrerequisites.ps1 -DeviceName '你的耳机名'
```

回传信息限于 Windows 版本/架构、蓝牙服务及枚举结果、耳机名称与 PnP 状态、构建工具存在性。不会收集蓝牙 MAC、设备实例 ID、序列号、账户名或凭据。设备名仍可能含姓名，可在分享前自行替换。`pnpStatus: OK` 不代表已连接；`bluetoothRadioCount: 0` 也不能排除采用其他服务实现的蓝牙硬件。

## 第二步：得到经过构建与测试的修改版

已有开发工具：按根目录 README 使用 `Tools/Build-Evaluation.ps1` 构建并运行 CTest，先检查其是否成功结束。脚本不安装或启动应用，生成文件位于 `airpods-desktop/Build/Evaluation/Binary/`。

没有开发工具：需要先在 Windows 构建机生成便携包（本次已完成）。仓库含 Windows CI（手动触发或专用测试分支推送触发），在构建和自动测试成功后导出带 SHA-256 的便携包；需要将本次源码带到可运行该流水线的仓库并实际执行，不能用旧版官方 EXE 替代。当前构建已经成功，打开上方链接，在 Artifacts 中下载 `airpods-evaluation-win11-x64`。GitHub 可能要求登录。下载的是产物外层 ZIP，解压后还有应用便携 ZIP 与对应 SHA-256 文件；再把应用 ZIP 解压到固定测试目录，直接启动其中的 EXE。

交付到测试电脑时必须复制整个部署目录中的运行依赖，不能只复制 EXE。测试运行程序如 `*Tests.exe`、调试符号及构建文件不属于最终便携包；许可证和字体许可证应随包保留。不要运行安装器。

可以用预检脚本验证文件齐全并记录 EXE 的 SHA-256：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Test-WindowsPrerequisites.ps1 -AppDirectory 'C:\AirPods-Evaluation'
```

这只是检查部分关键文件存在，不能替代实际加载 DLL、程序启动或蓝牙验证。

## 第三步：启动测试版

1. 关闭已经运行的 AirPodsDesktop / MagicPods 等电量监听应用，减少干扰；不卸载它们。
2. 在 Windows 蓝牙设置中完成正常配对。确认其他同型号 AirPods 暂时不在附近；测试电脑可关闭其他设备上的自动切换干扰。
3. 手动启动便携目录中的修改版 `AirPodsDesktop.exe`。首次使用若询问开机启动及低延迟功能，均选“否”；在应用内绑定要测试的 AirPods。
4. 上游应用会保存用户设置和日志，虽然无需安装器，但不是完全不落盘的沙盒程序。它沿用 AirPodsDesktop 的用户配置位置；如果电脑已有该应用的设置，先告知，以便单独隔离，避免覆盖原来的配置。
5. 首次运行若提示缺 DLL、窗口不出现或启动失败，记录错误全文并停止后续验收，不反复更改蓝牙驱动或安装不明组件。

## 验收记录

逐项记录实际结果；没有做的保留“未测试”，读不到电量不记成 0%。

| 操作 | 预期 | 实际结果 |
| --- | --- | --- |
| 连接 AirPods | 弹窗出现，名称与 Windows 设置一致 | 未测试 |
| 连接后暂未广播 | 显示已连接、等待电量，三列缺失显示 — | 未测试 |
| 打开充电盒并等待广播 | 左耳、右耳、盒子显示可获得的电量；充电状态有标记 | 未测试 |
| 合上盒盖、佩戴播放 | 观察广播停止后仍连接；不能把无广播误报为断开 | 未测试 |
| Windows 明确断开 | 显示已断开、保留设备名、清空电量 | 未测试 |
| 重新连接 | 再次弹窗，不沿用上一次电量直到收到新广播 | 未测试 |
| 放入一只耳机充电 | 左右耳槽位不交换，缺失数据与充电标记合理 | 未测试 |
| Windows 关闭再打开蓝牙 | 故障提示合理，恢复后能重新获得状态 | 未测试 |
| 睡眠 / 唤醒后重连 | 程序可恢复，不崩溃或显示旧数据 | 未测试 |
| 系统浅色 / 深色及缩放 | 名称、状态行、三列电量不重叠或裁切 | 未测试 |

电量通常为粗粒度估算，不应以 iPhone 的个位数读数作为必须完全相等的标准。连接状态与 Windows 对照；播放音频只用于确认实际声音输出，Windows 蓝牙“已连接”本身不等同音频路由已切换。

反馈请给出：型号、Windows build、蓝牙适配器型号、修改版 EXE SHA-256、失败操作、预期与实际，以及遮去姓名和其他个人信息后的截图。不需要发送完整系统日志或蓝牙抓包；若需要应用日志，再针对具体问题收集。

退出时从托盘菜单关闭应用。不要选择安装、开机启动或下载正式版更新。当前阶段未要求你安装到系统中。

## 本次设备与构建安排

用户报告设备：AirPods Pro 3，固件 `9B43a`。电脑只有 VS Code，尚未安装 Visual Studio C++ 工具链、Qt 或 vcpkg，因此优先使用 Windows 云端构建，不要求在测试电脑上安装开发工具。

已核查源码将产品标识 `0x2027` 映射为 AirPods Pro 3，且具有专用动画。新增两项 Qt 回归测试覆盖左右广播、分路电量、充电标记、0% 与未知电量区分，以及名称和动画选择；数据为人工构造，尚未在 Windows 执行，也不是 `9B43a` 广播抓包。

已完成用户明确授权的远程操作：将当前项目源码与构建脚本上传到 `KevinDeng233/codex_playground` 的 `airpods/pro3-hardware-test` 分支（不修改 main）；该分支的 push 触发 Windows 2022 GitHub Actions，安装构建机依赖、构建、运行测试，通过后导出便携 ZIP 与 SHA-256。不安装到用户电脑，不创建正式 Release。

此前 GitHub API 请求返回 Forbidden；推送预检查曾因缺少上传授权被自动审批拒绝。用户随后已明确批准上传至独立测试分支并触发构建。本次构建已通过：https://github.com/KevinDeng233/codex_playground/actions/runs/41268074739。源码将遵循该仓库的可见性；构建也可能占用账户的 GitHub Actions 配额。远程构建成功后仍需在用户的 Windows 11 上检查实际架构、蓝牙驱动与 `9B43a` 电量广播。
