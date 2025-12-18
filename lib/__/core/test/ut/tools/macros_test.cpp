/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

// cspell:ignore NOLINT

#include <core/base.hpp>
#include <core/object.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::tools::macros")
{
    class class_test
    {
    public:

        SIGHT_DECLARE_CLASS(class_test);

        //------------------------------------------------------------------------------

        [[nodiscard]] int get_num() const
        {
            return m_num;
        }

        class_test()
        = default;

        explicit class_test(int _num) :
            m_num(_num)
        {
        }

        virtual ~class_test()
        = default;

    private:

        int m_num {1};
    };

//------------------------------------------------------------------------------

    TEST_CASE("factory_macro")
    {
        const int num = 15;

        class_test::sptr class_test1;
        class_test::sptr class_test2 = std::make_shared<class_test>();
        class_test::sptr class_test3 = std::make_shared<class_test>(num);
        class_test::sptr class_test4 = std::make_shared<class_test>();

        CHECK(class_test1 == nullptr);
        CHECK_EQ(class_test2->get_num(), 1);
        CHECK_EQ(class_test3->get_num(), num);
        CHECK_EQ(class_test4->get_num(), 1);
    }
} // TEST_SUITE
