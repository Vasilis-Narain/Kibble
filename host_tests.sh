#!/bin/sh
# SPDX-FileCopyrightText: 2026 Vasilis Narain
#
# SPDX-License-Identifier: MIT

HOST_DIR="$(pwd -W 2>/dev/null || pwd)"
MSYS_NO_PATHCONF=1 docker run --rm -v "$HOST_DIR:/project" kibble-env cmake --workflow --preset host-tests
