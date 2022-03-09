#ifndef MYGAME_VERSION_H
#define MYGAME_VERSION_H

#include <string>

namespace mygame
{
    namespace version
    {
        extern const int major;
        extern const int minor;
        extern const int patch;
        extern const std::string git_commit;
        extern const std::string git_date;
        extern const std::string PROJECT_NAME;
        extern const std::string CMAKE_SYSTEM;
        extern const std::string CMAKE_SYSTEM_PROCESSOR;
        extern const std::string CMAKE_HOST_SYSTEM;
        extern const std::string CMAKE_HOST_SYSTEM_PROCESSOR;
        extern const std::string CMAKE_C_COMPILER_ID;
        extern const std::string CMAKE_C_COMPILER_VERSION;
        extern const std::string CMAKE_CXX_COMPILER_ID;
        extern const std::string CMAKE_CXX_COMPILER_VERSION;
        extern const std::string CMAKE_GENERATOR;
        extern const std::string CMAKE_GENERATOR_PLATFORM;
        extern const int CMAKE_SIZEOF_VOID_P;
    }
} // namespace mygame

#endif
