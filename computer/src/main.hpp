#include "git.hpp"

#include <lak/architecture.hpp>
#include <lak/macro_utils.hpp>

#define APP_VERSION GIT_TAG "-" GIT_HASH
#define APP_NAME    "Type Impact Monitor " STRINGIFY(LAK_ARCH) " " APP_VERSION
