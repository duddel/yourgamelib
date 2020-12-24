/*
Copyright (c) 2019-2020 Alexander Scholz

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
#ifndef YOURGAME_ASSETMANAGER_H
#define YOURGAME_ASSETMANAGER_H

#include <functional>
#include <cstdint> // std::uintptr_t
#include <typeinfo>
#include <map>
#include <string>

namespace yourgame
{
    class AssetManager
    {
    public:
        ~AssetManager()
        {
            // for all maps holding different pointer types...
            for (const auto &map : m_ptrs)
            {
                // ...delete all objects pointed to...
                for (const auto &ptr : map.second)
                {
                    if (ptr.second != 0)
                    {
                        // ...using the registered delete function
                        m_delFuncs[map.first](ptr.second);
                    }
                }
            }
        };

        template <class T>
        void put(std::string name, T *obj)
        {
            // insert delete function for objects of type T, if not present
            auto itDelFunc = m_delFuncs.find(typeid(T).hash_code());
            if (itDelFunc == m_delFuncs.end())
            {
                m_delFuncs[typeid(T).hash_code()] =
                    [](std::uintptr_t ptr) { delete reinterpret_cast<const T *>(ptr); };
            }

            // insert (or replace) obj into map for objects of type T
            auto itT = m_ptrs[typeid(T).hash_code()].find(name);
            if (itT != m_ptrs[typeid(T).hash_code()].end())
            {
                m_delFuncs[typeid(T).hash_code()](itT->second);
            }
            m_ptrs[typeid(T).hash_code()][name] = reinterpret_cast<std::uintptr_t>(obj);
        }

        template <class T>
        void put(std::string name, T obj)
        {
            static_assert(sizeof(T) + 1 == 0, "this class only works with pointer types");
        }

        template <class T>
        T *get(std::string name)
        {
            auto itMap = m_ptrs.find(typeid(T).hash_code());
            if (itMap != m_ptrs.end())
            {
                auto itT = itMap->second.find(name);
                if (itT != itMap->second.end())
                {
                    return reinterpret_cast<T *>(itT->second);
                }
            }
            return nullptr;
        }

        template <class T>
        void del(std::string name)
        {
            auto itMap = m_ptrs.find(typeid(T).hash_code());
            if (itMap != m_ptrs.end())
            {
                auto itT = itMap->second.find(name);
                if (itT != itMap->second.end())
                {
                    delete reinterpret_cast<T *>(itT->second);
                    itMap->second.erase(itT);
                }
            }
        }

    private:
        std::map<size_t, std::map<std::string, std::uintptr_t>> m_ptrs;
        std::map<size_t, std::function<void(std::uintptr_t)>> m_delFuncs;
    };
} // namespace yourgame

#endif
