/*
Copyright (c) 2019-2024 Alexander Scholz

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
    namespace util
    {
        /**
        \brief utility to store and request pointers, and to delete dynamically allocated objects, by name

        use like so:
        - create an object with `new`
        - use insert() to store the pointer by name
            - if it exists, previous object is deleted and the pointer is replaced
        - use get() to request the pointer by name
        - use destroy() to delete the object and remove the pointer by name

        \attention type T is always the base type, not the pointer type (T*)
        \attention never delete objects manually with `delete` if they have been stored with insert()
        */
        class AssetManager
        {
        public:
            ~AssetManager() { clear(); };

            /**
            \brief insert pointer. if it exists (same type T and same name), previous object is deleted and the pointer is replaced. rejects nullptrs.

            \attention obj must be a pointer type
            \attention only insert pointer to object, that has been created with `new`

            \tparam T object type
            \param name name
            \param obj pointer
            \return false, if obj was nullptr, true otherwise
            */
            template <class T>
            bool insert(std::string name, T *obj)
            {
                if (!obj)
                {
                    return false;
                }

                // insert delete function for objects of type T, if not present
                auto itDelFunc = m_delFuncs.find(typeid(T).hash_code());
                if (itDelFunc == m_delFuncs.end())
                {
                    m_delFuncs[typeid(T).hash_code()] =
                        [](std::uintptr_t ptr)
                    { delete reinterpret_cast<const T *>(ptr); };
                }

                // insert (or replace) obj into map for objects of type T
                auto itT = m_ptrMaps[typeid(T).hash_code()].find(name);
                if (itT != m_ptrMaps[typeid(T).hash_code()].end())
                {
                    m_delFuncs[typeid(T).hash_code()](itT->second);
                }
                m_ptrMaps[typeid(T).hash_code()][name] = reinterpret_cast<std::uintptr_t>(obj);

                return true;
            }

            /**
            \brief dummy for compile-time check (static_assert) of non-pointer type

            \tparam T object type
            \param name name
            \param obj pointer
            */
            template <class T>
            void insert(std::string name, T obj)
            {
                static_assert(sizeof(T) + 1 == 0, "this class only works with pointer types");
            }

            /**
            \brief request pointer by name

            \tparam T object type
            \param name name
            \return pointer. nullptr, if no pointer of type T* and name exists
            */
            template <class T>
            T *get(std::string name)
            {
                auto itMap = m_ptrMaps.find(typeid(T).hash_code());
                if (itMap != m_ptrMaps.end())
                {
                    auto itT = itMap->second.find(name);
                    if (itT != itMap->second.end())
                    {
                        return reinterpret_cast<T *>(itT->second);
                    }
                }
                return nullptr;
            }

            /**
            \brief returns number of stored pointers of type T*

            \tparam T object type
            \return number of stored pointers of type T*
            */
            template <class T>
            size_t numOf()
            {
                auto itMap = m_ptrMaps.find(typeid(T).hash_code());
                if (itMap != m_ptrMaps.end())
                {
                    return itMap->second.size();
                }
                return 0;
            }

            /**
            \brief destroy (delete) object, remove pointer

            \tparam T object type
            \param name name
            */
            template <class T>
            void destroy(std::string name)
            {
                auto itMap = m_ptrMaps.find(typeid(T).hash_code());
                if (itMap != m_ptrMaps.end())
                {
                    auto itT = itMap->second.find(name);
                    if (itT != itMap->second.end())
                    {
                        m_delFuncs[itMap->first](itT->second);
                        itMap->second.erase(itT);
                    }
                }
            }

            /**
            \brief destroy (delete) all objects of type T, remove all pointers of type T*

            \tparam T object type
            */
            template <class T>
            void destroy()
            {
                auto itMap = m_ptrMaps.find(typeid(T).hash_code());
                if (itMap != m_ptrMaps.end())
                {
                    for (const auto &ptr : itMap->second)
                    {
                        m_delFuncs[itMap->first](ptr.second);
                    }
                    m_ptrMaps.erase(itMap);
                }
            }

            /** \brief clear content. delete all objects, remove all pointers */
            void clear()
            {
                // for all maps holding different pointer types...
                for (const auto &map : m_ptrMaps)
                {
                    // ...delete all objects pointed to...
                    for (const auto &ptr : map.second)
                    {
                        // ...using the registered delete function
                        m_delFuncs[map.first](ptr.second);
                    }
                }
                m_ptrMaps.clear();
            }

        private:
            std::map<size_t, std::map<std::string, std::uintptr_t>> m_ptrMaps;
            std::map<size_t, std::function<void(std::uintptr_t)>> m_delFuncs;
        };
    } // namespace util
} // namespace yourgame

#endif
