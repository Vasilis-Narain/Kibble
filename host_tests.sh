# SPDX-FileCopyrightText: 2026 Vasilis Narain
#
# SPDX-License-Identifier: MIT

#!/bin/sh
MSYS_NO_PATHCONV=1 docker run --rm -v "$(pwd -W):/project" handheld-env sh -c "cmake --preset host-tests && cmake --build --preset host-tests --clean-first && ctest --preset host-tests"
