#ifndef CONFIG_HPP
#define CONFIG_HPP

// #define MOCK_PG_CONN

// This catches both 32/64 bit
#ifdef _WIN32

#define SANCHO_OS_WINDOWS
#define SANCHO_OS "windows"

#elif defined __unix__

#define SANCHO_OS_UNIX
#define SANCHO_OS "unix"

#elif defined __APPLE__

#define SANCHO_OS_MAC
#define SANCHO_OS "mac"

#elif defined __FreeBSD__

#define SANCHO_OS_FREEBSD
#define SANCHO_OS "freebsd"

#else

#define SANCHO_OS_UNKNOWN
#define SANCHO_OS "unknown"

#endif

namespace san {
namespace config {
const std::string current_version = "0.0.1";
}
} // namespace san

#endif
