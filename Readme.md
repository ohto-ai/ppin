# ppin

跨平台的钉图工具

[circleci]: https://app.circleci.com/pipelines/github/Ohto-Ai/ppin
[issues]: https://github.com/Ohto-Ai/ppin/issues
[stargazers]: https://github.com/Ohto-Ai/ppin/stargazers
[pulls]: https://github.com/Ohto-Ai/ppin/pulls
[commit-activity]: https://github.com/Ohto-Ai/ppin/pulse
[contributors]: https://github.com/Ohto-Ai/ppin/contributors
[circleci:badge]: https://img.shields.io/circleci/project/github/Ohto-Ai/ppin/master

[license:badge]: https://img.shields.io/github/license/Ohto-Ai/ppin?logo=github
[issues:badge]: https://img.shields.io/github/issues/Ohto-Ai/ppin?logo=github
[stargazers:badge]: https://img.shields.io/github/stars/Ohto-Ai/ppin?logo=github
[pulls:badge]: https://img.shields.io/github/issues-pr/Ohto-Ai/ppin?logo=github&color=0088ff
[contributors:badge]: https://img.shields.io/github/contributors/Ohto-Ai/ppin?logo=github
[commit-activity:badge]: https://img.shields.io/github/commit-activity/m/Ohto-Ai/ppin?logo=github
[repository]: https://github.com/Ohto-Ai/ppin


[win-link]: https://github.com/Ohto-Ai/ppin/actions/workflows/windows.yml "WindowsAction"
[win-badge]: https://github.com/Ohto-Ai/ppin/actions/workflows/windows.yml/badge.svg  "Windows"

[ubuntu-link]: https://github.com/Ohto-Ai/ppin/actions/workflows/ubuntu.yml "UbuntuAction"
[ubuntu-badge]: https://github.com/Ohto-Ai/ppin/actions/workflows/ubuntu.yml/badge.svg "Ubuntu"

[macos-link]: https://github.com/Ohto-Ai/ppin/actions/workflows/macos.yml "MacOSAction"
[macos-badge]: https://github.com/Ohto-Ai/ppin/actions/workflows/macos.yml/badge.svg "MacOS"

[ios-link]: https://github.com/Ohto-Ai/ppin/actions/workflows/ios.yml "IOSAction"
[ios-badge]: https://github.com/Ohto-Ai/ppin/actions/workflows/ios.yml/badge.svg "IOS"

| [Windows][win-link]| [Ubuntu][ubuntu-link]|[MacOS][macos-link]|[IOS][ios-link]|
|---------------|---------------|-----------------|-----------------|
| ![win-badge]  | ![ubuntu-badge] | ![macos-badge]   |![ios-badge]   |

[![CodeFactor](https://www.codefactor.io/repository/github/ohto-ai/ppin/badge)](https://www.codefactor.io/repository/github/ohto-ai/ppin)

[![License][license:badge]](/LICENSE)
[![Issues][issues:badge]][issues]  
[![Stargazers][stargazers:badge]][stargazers]
[![Pulls][pulls:badge]][pulls]  
[![Contributors][contributors:badge]][contributors]
[![Commit Activity][commit-activity:badge]][commit-activity]  

![image](https://user-images.githubusercontent.com/46275725/124393919-7620e200-dd2f-11eb-836a-bed2e901fe1f.png)

![image](https://user-images.githubusercontent.com/46275725/124363051-8cfe0080-dc6b-11eb-88ff-1ccd912dc979.png)

![image](https://user-images.githubusercontent.com/46275725/124393703-550bc180-dd2e-11eb-8bf2-004bcdc68391.png)

## Usage

### 程序可以将图像悬浮在桌面上，支持bmp/png/jpg以及gif动图

- 程序启动参数`ppin -i=<图像路径>`，可以将本地bmp/jpg/png/gif图像悬浮
- 程序无参启动时，如果没有已启动实例，进程会悬浮一张02的图像
- 程序无参启动时，如果已存在实例，则会检查剪切板的图像并悬浮
- 可向程序已经打开的窗口拖入图像来将此图像悬浮，网络图像以及本地不可悬浮的文件会显示禁止符号
- 其他使用细节见悬浮菜单和托盘菜单

### 快捷键
- <kbd>Ctrl</kbd> + <kbd>O</kbd> 读取图像
- <kbd>Ctrl</kbd> + <kbd>C</kbd> 截取画面
- <kbd>Ctrl</kbd> + <kbd>L</kbd> 锁定图像
- <kbd>Ctrl</kbd> + <kbd>M</kbd> 图像居中
- <kbd>Ctrl</kbd> + <kbd>W</kbd> 关闭图像
- <kbd>Ctrl</kbd> + <kbd>D</kbd> 克隆图像

## License

This project is licensed under the terms of the [MIT License](/LICENSE).

## FAQ

[![Email](https://img.shields.io/badge/mail-zhu.thatboy@outlook.com-blue.svg?&style=for-the-badge)](mailto:zhu.thatboy@outlook.com?subject=Feedback&body=This%20is%20a%20test%20feedback.)

