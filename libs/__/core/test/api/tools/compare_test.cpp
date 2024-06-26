/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "compare_test.hpp"

#include <core/compare.hpp>

#include <array>
#include <cmath>
#include <limits>
#include <list>
#include <map>
#include <unordered_set>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::compare_test);

namespace sight::core::tools::ut
{

// Simple class to test the comparison of containers
struct double_t
{
    double_t()
    = default;

    explicit double_t(double _value) :
        m_value(_value)
    {
    }

    const double m_value {0.0};

    // This is required to be able to compare the objects
    inline bool operator==(const double_t& _other) const
    {
        return is_equal(m_value, _other.m_value);
    }
};

//------------------------------------------------------------------------------

void compare_test::setUp()
{
}

//------------------------------------------------------------------------------

void compare_test::tearDown()
{
}

//------------------------------------------------------------------------------

void compare_test::double_container_test()
{
    // Trivial comparison
    const std::vector<double> a = {1.0, 2.0, 3.0};
    const std::list<double> b   = {a[0], a[1], a[2]};

    CPPUNIT_ASSERT(is_equal(a, b));

    const std::vector<double> c = {a[0], a[1], a[2] + 1.0};

    CPPUNIT_ASSERT(!is_equal(a, c));

    // Almost not but still equal comparison
    const std::vector<double> d = {a[0], a[1], std::nextafter(a[2], std::numeric_limits<double>::max())};
    CPPUNIT_ASSERT(is_equal(a, d));

    // Almost but not equal comparison
    const std::vector<double> e = {
        a[0],
        a[1],
        std::nextafter(std::nextafter(a[2], std::numeric_limits<double>::max()), std::numeric_limits<double>::max())
    };
    CPPUNIT_ASSERT(!is_equal(a, e));

    // Test also < and >
    CPPUNIT_ASSERT(is_less(a[0], a[1]));
    CPPUNIT_ASSERT(is_greater(a[2], a[0]));

    CPPUNIT_ASSERT(is_less(a[2], e[2]));
    CPPUNIT_ASSERT(is_greater(e[2], a[2]));
}

//------------------------------------------------------------------------------

void compare_test::float_container_test()
{
    // Trivial comparison
    const std::vector<float> a = {1.0F, 2.0F, 3.0F};
    const std::list<float> b   = {a[0], a[1], a[2]};

    CPPUNIT_ASSERT(is_equal(a, b));

    const std::vector<float> c = {a[0], a[1], a[2] + 1.0F};

    CPPUNIT_ASSERT(!is_equal(a, c));

    // Almost not but still equal comparison
    const std::vector<float> d = {a[0], a[1], std::nextafter(a[2], std::numeric_limits<float>::max())};
    CPPUNIT_ASSERT(is_equal(a, d));

    // Almost but not equal comparison
    const std::vector<float> e = {
        a[0],
        a[1],
        std::nextafter(std::nextafter(a[2], std::numeric_limits<float>::max()), std::numeric_limits<float>::max())
    };
    CPPUNIT_ASSERT(!is_equal(a, e));

    // Test also < and >
    CPPUNIT_ASSERT(is_less(a[0], a[1]));
    CPPUNIT_ASSERT(is_greater(a[2], a[0]));

    CPPUNIT_ASSERT(is_less(a[2], e[2]));
    CPPUNIT_ASSERT(is_greater(e[2], a[2]));
}

//------------------------------------------------------------------------------

void compare_test::mixed_container_test()
{
    // Trivial comparison
    constexpr std::array<const double, 3> a = {1.0, 2.0, 3.0};
    constexpr std::array<const float, 3> b  = {1.0F, 2.0F, 3.0F};

    CPPUNIT_ASSERT(is_equal(a, b));

    constexpr std::array<const float, 3> c = {b[0], b[1], b[2] + 1.0F};

    CPPUNIT_ASSERT(!is_equal(a, c));

    // Almost not but still equal comparison
    const std::vector<float> d = {b[0], b[1], std::nextafter(b[2], std::numeric_limits<float>::max())};
    CPPUNIT_ASSERT(is_equal(a, d));

    // Almost but not equal comparison
    const std::vector<float> e = {
        b[0],
        b[1],
        std::nextafter(std::nextafter(b[2], std::numeric_limits<float>::max()), std::numeric_limits<float>::max())
    };
    CPPUNIT_ASSERT(!is_equal(a, e));

    // Test also < and >
    CPPUNIT_ASSERT(is_less(a[0], a[1]));
    CPPUNIT_ASSERT(is_greater(a[2], a[0]));

    CPPUNIT_ASSERT(is_less(a[2], e[2]));
    CPPUNIT_ASSERT(is_greater(e[2], a[2]));
}

//------------------------------------------------------------------------------

void compare_test::sandbox_test()
{
    constexpr double a = 1.0;
    double b           = 0.0;

    for(int i = 0 ; i < 10 ; ++i)
    {
        b += 0.1;
    }

    // This demonstrates why is_equal is useful...
    CPPUNIT_ASSERT(a != b);
    CPPUNIT_ASSERT(is_equal(a, b));

    // This is problematic for classical unscaled epsilon based solutions
    constexpr float c = 1.0001F;
    constexpr float d = 0.99998F;

    CPPUNIT_ASSERT(c != d);
    CPPUNIT_ASSERT(!is_equal(c, d));

    // Exactly one ULP away from 'e'
    constexpr float e = 67329.234F;
    constexpr float f = 67329.242F;

    CPPUNIT_ASSERT(e != f);
    // This should be false, but it is true because there is only one ULP away from 'e'
    CPPUNIT_ASSERT(is_equal(e, f));

    // < or > returns true because the difference between e and f is signed
    CPPUNIT_ASSERT(is_less(e, f));
    CPPUNIT_ASSERT(is_greater(f, e));

    // .007812500000000000 Which is not 0 !
    const float g = (f - e);
    CPPUNIT_ASSERT(g != 0.0F);
    CPPUNIT_ASSERT(!is_equal(g, 0.0F));

    // Double precision is a bit better
    constexpr double h = 67329.234;
    constexpr double i = 67329.242;

    CPPUNIT_ASSERT(h != i);
    CPPUNIT_ASSERT(!is_equal(h, i));
}

//------------------------------------------------------------------------------

void compare_test::infinite_test()
{
    constexpr float a = std::numeric_limits<float>::infinity();
    constexpr float b = std::numeric_limits<float>::infinity();
    constexpr float c = -std::numeric_limits<float>::infinity();
    constexpr float d = -std::numeric_limits<float>::infinity();

    CPPUNIT_ASSERT(is_equal(a, b));
    CPPUNIT_ASSERT(is_equal(c, d));
    CPPUNIT_ASSERT(!is_equal(a, c));
}

//------------------------------------------------------------------------------

void compare_test::nan_test()
{
    constexpr float a = std::numeric_limits<float>::quiet_NaN();
    constexpr float b = std::numeric_limits<float>::quiet_NaN();
    constexpr float c = std::numeric_limits<float>::signaling_NaN();
    constexpr float d = std::numeric_limits<float>::signaling_NaN();

    CPPUNIT_ASSERT(is_equal(a, b));
    CPPUNIT_ASSERT(is_equal(c, d));
    CPPUNIT_ASSERT(is_equal(a, c));
}

//------------------------------------------------------------------------------

void compare_test::pointer_test()
{
    {
        // raw pointers
        auto* const a = new int(42);
        auto* const b = new int(42);
        auto* const c = new int(0);
        auto* const d = a;

        CPPUNIT_ASSERT(is_equal(a, b));
        CPPUNIT_ASSERT(!is_equal(a, c));
        CPPUNIT_ASSERT(is_equal(a, d));

        delete a;
        delete b;
        delete c;
    }

    {
        // smart pointers
        const auto a  = std::make_shared<double>(42.0);
        const auto b  = std::make_shared<float>(42.0F);
        const auto c  = std::make_shared<int>(0);
        const auto& d = a;

        CPPUNIT_ASSERT(is_equal(a, b));
        CPPUNIT_ASSERT(!is_equal(a, c));
        CPPUNIT_ASSERT(is_equal(a, d));
    }

    {
        // Smart object pointers
        const auto a = std::make_shared<double>(3.141592653589793238462643383279502884);
        const auto b = std::make_shared<double>(3.141592653589793238462643383279502884);
        const auto c = std::make_shared<double>(0);

        CPPUNIT_ASSERT(is_equal(a, b));
        CPPUNIT_ASSERT(!is_equal(a, c));
    }
}

//------------------------------------------------------------------------------

void compare_test::map_test()
{
    std::map<int, double> a {
        {1, 1.0},
        {2, 2.0},
        {3, 3.0}
    };

    // Identical
    std::map<int, double> b {
        {1, 1.0},
        {2, 2.0},
        {3, 3.0}
    };

    CPPUNIT_ASSERT(is_equal(a, b));

    // Different size
    std::map<int, double> c {
        {1, 1.0},
        {2, 2.0}
    };

    CPPUNIT_ASSERT(!is_equal(a, c));

    // Different values
    std::map<int, double> d {
        {1, 4.0},
        {2, 5.0},
        {3, 6.0}
    };

    CPPUNIT_ASSERT(!is_equal(a, d));

    // Different keys
    std::map<int, double> e {
        {4, 1.0},
        {5, 2.0},
        {6, 3.0}
    };

    CPPUNIT_ASSERT(!is_equal(a, e));

    std::map<int, std::shared_ptr<double> > f {
        {1, std::make_shared<double>(1.0)},
        {2, std::make_shared<double>(2.0)},
        {3, std::make_shared<double>(3.0)}
    };

    std::map<int, std::shared_ptr<double> > g {
        {1, std::make_shared<double>(1.0)},
        {2, std::make_shared<double>(2.0)},
        {3, std::make_shared<double>(3.0)}
    };

    CPPUNIT_ASSERT(is_equal(f, g));

    std::map<int, std::shared_ptr<double> > h {
        {1, f[1]},
        {2, f[2]},
        {3, f[3]}
    };

    CPPUNIT_ASSERT(is_equal(f, h));

    std::map<int, std::shared_ptr<double> > i {
        {1, std::make_shared<double>(4.0)},
        {2, std::make_shared<double>(5.0)},
        {3, std::make_shared<double>(6.0)}
    };

    CPPUNIT_ASSERT(!is_equal(f, i));
}

//------------------------------------------------------------------------------

void compare_test::unordered_set_test()
{
    // Empty set equality test
    CPPUNIT_ASSERT(is_equal(std::unordered_set<int>(), std::unordered_set<int>()));

    // Same size, same values, but different order
    const std::unordered_set<double> a {1.0, 2.0, 3.0};
    const std::unordered_set<double> b {3.0, 1.0, 2.0};

    CPPUNIT_ASSERT(is_equal(a, b));

    // Same size, one different value
    const std::unordered_set<double> c {0.0, 2.0, 3.0};
    CPPUNIT_ASSERT(!is_equal(a, c));
}

//------------------------------------------------------------------------------

void compare_test::pair_test()
{
    const std::pair<int, double> a {1, 1.0};
    const std::pair<int, double> b {1, 1.0};
    const std::pair<int, double> c {1, 2.0};
    const std::pair<int, double> d {2, 1.0};
    const std::pair<int, double> e {2, 2.0};

    CPPUNIT_ASSERT(is_equal(a, b));
    CPPUNIT_ASSERT(!is_equal(a, c));
    CPPUNIT_ASSERT(!is_equal(a, d));
    CPPUNIT_ASSERT(!is_equal(a, e));
}

} // namespace sight::core::tools::ut
