/*
Copyright (c) 2019-2022 Alexander Scholz

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

#include <cassert>
#include "yourgame/util/assetmanager.h"

struct A
{
    static size_t numObj;
    int m_a;
    A(int a) : m_a(a) { A::numObj++; }
    ~A() { A::numObj--; }
};
size_t A::numObj = 0;

int main()
{
    yourgame::util::AssetManager assman;

    // nullptr insert
    assman.insert<A>("A1", nullptr);
    assert(assman.numOf<A>() == 0);

    // insert A* (A1)
    assman.insert<A>("A1", new A(1));
    assert(A::numObj == 1);
    assert(assman.numOf<A>() == 1);

    // overwrite A1
    assert(assman.get<A>("A1")->m_a == 1);
    assman.insert<A>("A1", new A(11));
    assert(assman.get<A>("A1")->m_a == 11);
    assert(A::numObj == 1);
    assert(assman.numOf<A>() == 1);

    // insert more A*
    assman.insert<A>("A2", new A(2));
    assman.insert<A>("A3", new A(3));
    assert(A::numObj == 3);
    assert(assman.numOf<A>() == 3);

    // get
    assert(assman.get<A>("A1")->m_a == 11);
    assert(assman.get<A>("A2")->m_a == 2);
    assert(assman.get<A>("A3")->m_a == 3);
    assert(assman.get<A>("no A") == nullptr);
    assert(assman.get<float>("F99") == nullptr);

    // numOf()
    assert(assman.numOf<float>() == 0);

    // destroy
    assman.destroy<A>("A2");
    assman.destroy<A>("A99"); // does not exist
    assert(A::numObj == 2);
    assert(assman.numOf<A>() == 2);
    assman.destroy<float>("F99"); // does not exist
    assert(assman.numOf<float>() == 0);

    // destroy all
    assman.destroy<A>();
    assert(A::numObj == 0);
    assert(assman.numOf<A>() == 0);
    assman.destroy<float>();
    assert(assman.numOf<float>() == 0);

    // clear
    assman.insert<A>("A1", new A(1));
    assman.insert<A>("A2", new A(2));
    assman.insert<A>("A3", new A(3));
    assert(A::numObj == 3);
    assert(assman.numOf<A>() == 3);
    assman.clear();
    assert(A::numObj == 0);
    assert(assman.numOf<A>() == 0);

    // destruction
    {
        yourgame::util::AssetManager assman2;
        assman2.insert<A>("A1", new A(1));
        assman2.insert<A>("A2", new A(2));
        assman2.insert<A>("A3", new A(3));
        assert(A::numObj == 3);
    }
    // assman2 has been destructed, all As must be deleted
    assert(A::numObj == 0);

    return 0;
}
