# Third-party software notices

This is a modified GPL-3.0-or-later build of AirPodsDesktop by SpriteOvO.
Original source: https://github.com/SpriteOvO/AirPodsDesktop
Original commit: 56b04001f75044dbd15507303a9438c71e93c6e4
Tested application source: https://github.com/KevinDeng233/codex_playground/tree/9858bb03204b147bf11ab6bf0b20c402f8927422/airpods-desktop

Corresponding application source and build scripts are provided in the GitHub
source archives for this release. The native executable is unchanged from the
Windows CI run linked in the release notes. This release is not endorsed by Apple
or by the original authors.

Qt 6.8.3 is dynamically linked. Its open-source terms and notices apply to its
libraries and plugins, independently of this application's GPL license. Users may
replace the dynamically linked libraries with compatible modified builds and debug
such modifications. Relevant license texts are in licenses/.
Qt source: https://download.qt.io/archive/qt/6.8/6.8.3/single/
Qt third-party notices: https://doc.qt.io/qt-6.8/licenses-used-in-qt.html
Qt Multimedia / FFmpeg: https://doc.qt.io/qt-6.8/qtmultimedia-attribution-ffmpeg.html
FFmpeg source and licensing: https://ffmpeg.org/download.html and https://ffmpeg.org/legal.html

Other application dependencies are spdlog (including bundled fmt), cxxopts, cpr,
nlohmann/json, SingleApplication and magic_enum (MIT), and Boost.PFR / Boost.Stacktrace
(Boost Software License 1.0). The dependency versions and source revisions are
recorded in airpods-desktop/CMakeLists.txt and vcpkg.json in the source archive.
Networking dependencies may include curl (curl license), OpenSSL 1.1.1l (OpenSSL / SSLeay licenses) and
zlib (zlib license). Copies of their principal license texts are included alongside
this notice. Dependency notices do not relicense the application.

Inter and Noto Sans TC fonts retain their SIL Open Font License notices in
licenses/Inter.txt and licenses/NotoSansTC.txt.

Microsoft runtime DLLs remain subject to Microsoft's Visual C++ runtime terms:
https://visualstudio.microsoft.com/license-terms/
