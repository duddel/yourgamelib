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
#include "doctest.h"
#include "yourgame/yourgame.h"

struct A
{
    static size_t numObj;
    int m_a;
    A(int a) : m_a(a) { A::numObj++; }
    ~A() { A::numObj--; }
};
size_t A::numObj = 0;

TEST_CASE("AssetManager")
{
    yourgame::util::AssetManager assman;
    assman.insert<A>("A1", new A(1));
    assman.insert<A>("A2", new A(2));
    assman.insert<A>("A3", new A(3));
    REQUIRE(assman.numOf<A>() == 3);
    REQUIRE(A::numObj == 3);

    SUBCASE("numOf()")
    {
        CHECK(assman.numOf<A>() == 3);
        CHECK(assman.numOf<float>() == 0);
        CHECK(A::numObj == 3);
    }

    SUBCASE("insert nullptr")
    {
        assman.insert<A>("A4", nullptr);
        CHECK(assman.numOf<A>() == 3);
    }

    SUBCASE("insert A*")
    {
        assman.insert<A>("A4", new A(4));
        CHECK(A::numObj == 4);
        CHECK(assman.numOf<A>() == 4);
    }

    SUBCASE("overwrite")
    {
        assman.insert<A>("A1", new A(11));
        CHECK(assman.get<A>("A1")->m_a == 11);
        CHECK(A::numObj == 3);
        CHECK(assman.numOf<A>() == 3);
    }

    SUBCASE("get()")
    {
        CHECK(assman.get<A>("A1")->m_a == 1);
        CHECK(assman.get<A>("A2")->m_a == 2);
        CHECK(assman.get<A>("A3")->m_a == 3);
        CHECK(assman.get<A>("no A") == nullptr);
        CHECK(assman.get<float>("F99") == nullptr);
    }

    SUBCASE("destroy()")
    {
        assman.destroy<A>("A2");
        assman.destroy<A>("A99"); // does not exist
        CHECK(A::numObj == 2);
        CHECK(assman.numOf<A>() == 2);
        assman.destroy<float>("F99"); // does not exist
        CHECK(assman.numOf<float>() == 0);
    }

    SUBCASE("destroy() all")
    {
        assman.destroy<A>();
        CHECK(A::numObj == 0);
        CHECK(assman.numOf<A>() == 0);
        assman.destroy<float>();
        CHECK(assman.numOf<float>() == 0);
    }

    SUBCASE("clear()")
    {
        assman.clear();
        CHECK(A::numObj == 0);
        CHECK(assman.numOf<A>() == 0);
    }
}
