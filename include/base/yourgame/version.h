/*
Copyright (c) 2019-2023 Alexander Scholz

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#ifndef YOURGAME_VERSION_H
#define YOURGAME_VERSION_H

#include <string>

namespace yourgame
{
    namespace version
    {
        extern const int major;
        extern const int minor;
        extern const int patch;
        extern const std::string verstr;
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
} // namespace yourgame

#endif
