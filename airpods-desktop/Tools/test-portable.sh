#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p Build/Portable
"${CXX:-g++}" -std=c++20 -Wall -Wextra -Werror -pedantic -I . \
    Tests/Portable/ConnectionTelemetryTests.cpp -o Build/Portable/ConnectionTelemetryTests
Build/Portable/ConnectionTelemetryTests
