# SPDX-FileCopyrightText: 2026 Vasilis Narain
#
# SPDX-License-Identifier: MIT

#!/bin/sh
docker run --rm -v "$PWD:/project" kibble-env cmake --workflow --preset host-tests
