/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include <data/array.hpp>
#include <data/exception.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::array")
{
//-----------------------------------------------------------------------------

    TEST_CASE("allocation")
    {
        sight::data::array::sptr array = std::make_shared<sight::data::array>();
        auto lock                      = array->dump_lock();

        CHECK(array->empty());
        CHECK(array->buffer() == nullptr);
        CHECK(array->size().empty());
        CHECK_EQ(static_cast<std::size_t>(0), array->size_in_bytes());

        sight::data::array::size_t size = {10, 100};

        array->resize(size, sight::core::type::UINT32, true);
        CHECK(array->buffer() != nullptr);
        CHECK(!array->empty());

        CHECK_EQ(size.size(), array->num_dimensions());
        CHECK_EQ(size[0], array->size()[0]);
        CHECK_EQ(size[1], array->size()[1]);
        CHECK_EQ(true, array->get_is_buffer_owner());
        {
            sight::data::array::offset_t stride = {4, 40};
            CHECK(array->get_strides() == stride);
        }
        CHECK_EQ(sight::core::type::UINT32, array->type());
        CHECK_EQ(static_cast<size_t>(4), array->type().size());

        array->clear();
        CHECK_EQ(static_cast<std::size_t>(0), array->size_in_bytes());
        CHECK(array->empty());
        CHECK(array->buffer() == nullptr);
        CHECK_EQ(sight::core::type::NONE, array->type());

        auto* buffer = new std::uint16_t[1000];

        for(std::uint16_t i = 0 ; i < 1000 ; i++)
        {
            buffer[i] = i;
        }

        array->set_buffer(
            buffer,
            false,
            size,
            sight::core::type::UINT16,
            std::make_shared<sight::core::memory::buffer_new_policy>()
        );

        CHECK_EQ(static_cast<std::size_t>(2), array->element_size_in_bytes());
        CHECK_EQ(static_cast<std::size_t>(2 * 10 * 100), array->size_in_bytes());
        {
            sight::data::array::offset_t stride = {2, 20};
            CHECK(array->get_strides() == stride);
        }
        CHECK_EQ(buffer[0], array->at<std::uint16_t>({0, 0}));
        CHECK_EQ(buffer[10], array->at<std::uint16_t>({0, 1}));
        CHECK_EQ(buffer[999], array->at<std::uint16_t>({9, 99}));
        CHECK_EQ(buffer[326], array->at<std::uint16_t>({6, 32}));
        CHECK_EQ(buffer[947], array->at<std::uint16_t>({7, 94}));
        CHECK_EQ(buffer[238], array->at<std::uint16_t>({8, 23}));
        CHECK_EQ(false, array->get_is_buffer_owner());

        array->clear();

        delete[] buffer;
    }

//-----------------------------------------------------------------------------

    TEST_CASE("resize")
    {
        sight::data::array::sptr array = std::make_shared<sight::data::array>();

        sight::data::array::size_t size {10, 100};

        array->resize(size, sight::core::type::UINT32, true);
        auto lock = array->dump_lock();

        CHECK(array->buffer() != nullptr);

        std::uint32_t count = 0;

        auto iter = array->begin<std::uint32_t>();

        for( ; iter != array->end<std::uint32_t>() ; ++iter)
        {
            *iter = count++;
        }

        CHECK_EQ(static_cast<std::size_t>(4), array->element_size_in_bytes());
        CHECK_EQ(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
        {
            sight::data::array::offset_t stride = {4, 40};
            CHECK(array->get_strides() == stride);
        }
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

        sight::data::array::size_t new_size = {100, 10};

        array->resize(new_size);
        CHECK(new_size == array->size());
        CHECK_EQ(static_cast<std::size_t>(4), array->element_size_in_bytes());
        CHECK_EQ(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
        {
            sight::data::array::offset_t stride = {4, 400};
            CHECK(array->get_strides() == stride);
        }
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({10, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({99, 9}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({26, 3}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({47, 9}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({38, 2}));

        new_size.clear();
        new_size = {25, 40};

        array->resize(new_size);
        CHECK(new_size == array->size());
        CHECK_EQ(static_cast<std::size_t>(4), array->element_size_in_bytes());
        {
            sight::data::array::offset_t stride = {4, 100};
            CHECK(array->get_strides() == stride);
        }
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({10, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({24, 39}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({1, 13}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({22, 37}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({13, 9}));

        new_size.clear();

        new_size = {2, 10, 100};

        array->resize(new_size, sight::core::type::UINT16, false);
        CHECK(new_size == array->size());
        CHECK_EQ(static_cast<std::size_t>(2), array->element_size_in_bytes());
        CHECK_EQ(static_cast<std::size_t>(2 * 100 * 10 * 2), array->size_in_bytes());
        {
            sight::data::array::offset_t stride = {2, 4, 40};
            CHECK(array->get_strides() == stride);
        }
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 0, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({0, 9, 99}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({0, 6, 32}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({0, 7, 94}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({0, 8, 23}));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("reallocate")
    {
        sight::data::array::sptr array = std::make_shared<sight::data::array>();

        sight::data::array::size_t size = {10, 100};

        array->resize(size, sight::core::type::UINT32, true);
        auto lock = array->dump_lock();

        std::uint32_t count = 0;
        auto iter           = array->begin<std::uint32_t>();
        auto iter_end       = array->end<std::uint32_t>();
        for( ; iter != iter_end ; ++iter)
        {
            *iter = count++;
        }

        CHECK_EQ(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

        sight::data::array::size_t new_size = {100, 100};

        array->resize(new_size, true);
        CHECK(new_size == array->size());
        CHECK_EQ(static_cast<std::size_t>(4 * 100 * 100), array->size_in_bytes());
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({10, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({99, 9}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({26, 3}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({47, 9}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({38, 2}));

        std::uint32_t value = 1859;
        array->at<std::uint32_t>({50, 90}) = value;
        CHECK_EQ(value, array->at<std::uint32_t>({50, 90}));

        std::uint32_t value2 = 25464;
        array->at<std::uint32_t>({99, 99}) = value2;
        CHECK_EQ(value2, array->at<std::uint32_t>({99, 99}));

        new_size = {2, 100, 100};
        array->resize(new_size, sight::core::type::UINT32, true);
        CHECK(new_size == array->size());
        CHECK_EQ(static_cast<std::size_t>(4 * 2 * 100 * 100), array->size_in_bytes());
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 5, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({1, 99, 4}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({0, 63, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({1, 73, 4}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({0, 19, 1}));

        CHECK_EQ(value, array->at<std::uint32_t>({0, 25, 45}));

        CHECK_EQ(value2, array->at<std::uint32_t>({1, 99, 49}));

        std::uint32_t value3 = 2156;
        array->at<std::uint32_t>({0, 35, 48}) = value3;
        CHECK_EQ(value3, array->at<std::uint32_t>({0, 35, 48}));

        std::uint32_t value4 = 105;
        array->at<std::uint32_t>({1, 99, 99}) = value4;
        CHECK_EQ(value4, array->at<std::uint32_t>({1, 99, 99}));

        new_size.clear();

        new_size = {10, 100};

        array->resize(new_size, true);
        CHECK(new_size == array->size());
        CHECK_EQ(static_cast<std::size_t>(4), array->element_size_in_bytes());
        CHECK_EQ(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

        new_size = {2, 10, 100};
        array->resize(new_size, sight::core::type::UINT16, true);
        CHECK_EQ(static_cast<std::size_t>(2), array->element_size_in_bytes());
        CHECK_EQ(static_cast<std::size_t>(2 * 10 * 100 * 2), array->size_in_bytes());
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 0, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({0, 9, 99}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({0, 6, 32}));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({0, 7, 94}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({0, 8, 23}));

        new_size = {10, 100};
        array->resize(new_size, true);
        CHECK(new_size == array->size());
        CHECK_EQ(static_cast<std::size_t>(2), array->element_size_in_bytes());
        CHECK_EQ(static_cast<std::size_t>(2 * 10 * 100), array->size_in_bytes());
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 2}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({2, 65}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({6, 47}));

        array->clear();
        CHECK_EQ(static_cast<std::size_t>(0), array->size_in_bytes());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        sight::data::array::sptr array = std::make_shared<sight::data::array>();

        sight::data::array::size_t size = {10, 100};

        array->resize(size, sight::core::type::UINT32, true);
        auto array_lock = array->dump_lock();
        CHECK(array->buffer() != nullptr);

        std::uint32_t count = 0;
        auto iter           = array->begin<std::uint32_t>();
        auto iter_end       = array->end<std::uint32_t>();

        for( ; iter != iter_end ; ++iter)
        {
            *iter = count++;
        }

        sight::data::array::sptr deep_copy_array = sight::data::object::copy(array);
        auto deep_copy_array_lock                = deep_copy_array->dump_lock();

        // check deep_copy
        CHECK_EQ(array->element_size_in_bytes(), deep_copy_array->element_size_in_bytes());
        CHECK_EQ(array->size_in_bytes(), deep_copy_array->size_in_bytes());
        CHECK(array->get_strides() == deep_copy_array->get_strides());
        CHECK(array->size() == deep_copy_array->size());
        CHECK_EQ(
            array->at<std::uint32_t>({0, 0}),
            deep_copy_array->at<std::uint32_t>({0, 0})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({0, 1}),
            deep_copy_array->at<std::uint32_t>({0, 1})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({9, 99}),
            deep_copy_array->at<std::uint32_t>({9, 99})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({6, 32}),
            deep_copy_array->at<std::uint32_t>({6, 32})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({7, 94}),
            deep_copy_array->at<std::uint32_t>({7, 94})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({8, 23}),
            deep_copy_array->at<std::uint32_t>({8, 23})
        );
        CHECK_EQ(true, deep_copy_array->get_is_buffer_owner());

        sight::data::array::csptr deep_copy_array2 = sight::data::object::copy(array);
        auto deep_copy_array_lock2                 = deep_copy_array->dump_lock();

        CHECK_EQ(array->element_size_in_bytes(), deep_copy_array->element_size_in_bytes());
        CHECK_EQ(array->size_in_bytes(), deep_copy_array->size_in_bytes());
        CHECK(array->get_strides() == deep_copy_array->get_strides());
        CHECK(array->size() == deep_copy_array->size());
        CHECK_EQ(
            array->at<std::uint32_t>({0, 0}),
            deep_copy_array->at<std::uint32_t>({0, 0})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({0, 1}),
            deep_copy_array->at<std::uint32_t>({0, 1})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({9, 99}),
            deep_copy_array->at<std::uint32_t>({9, 99})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({6, 32}),
            deep_copy_array->at<std::uint32_t>({6, 32})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({7, 94}),
            deep_copy_array->at<std::uint32_t>({7, 94})
        );
        CHECK_EQ(
            array->at<std::uint32_t>({8, 23}),
            deep_copy_array->at<std::uint32_t>({8, 23})
        );
        CHECK_EQ(true, deep_copy_array->get_is_buffer_owner());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("dump_lock")
    {
        sight::data::array::size_t size = {10, 100};
        sight::data::array::sptr array  = std::make_shared<sight::data::array>();
        array->resize({12, 15}, sight::core::type::INT16, true);

        CHECK_THROWS_AS(array->buffer(), sight::data::exception);

        auto lock = array->dump_lock();
        CHECK_NOTHROW(array->buffer());
    }

//-----------------------------------------------------------------------------

    TEST_CASE("buffer_access")
    {
        // test get_buffer
        sight::data::array::sptr array = std::make_shared<sight::data::array>();

        sight::data::array::size_t size = {10, 100};

        array->resize(size, sight::core::type::UINT32, true);

        {
            CHECK_THROWS_AS(array->buffer(), sight::core::exception);
            CHECK_THROWS_AS(array->begin(), sight::core::exception);
            CHECK_THROWS_AS(array->end(), sight::core::exception);
        }

        const auto lock = array->dump_lock();
        {
            // Check that the iterator properly lock the buffer
            CHECK_NOTHROW(array->begin());
            CHECK_NOTHROW(array->buffer());
        }

        std::uint32_t count = 0;
        for(auto& iter : array->range<std::uint32_t>())
        {
            iter = count++;
        }

        CHECK_EQ(static_cast<std::uint32_t>(1000), count);
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(99), array->at<std::uint32_t>({9, 9}));

        CHECK_THROWS_AS(array->at<std::uint32_t>({10, 0}), sight::data::exception);
        CHECK_THROWS_AS(array->at<std::uint32_t>({0, 100}), sight::data::exception);

        {
            auto itr = array->begin<std::uint32_t>();
            CHECK_EQ(static_cast<std::uint32_t>(0), *itr);
            itr++;
            CHECK_EQ(static_cast<std::uint32_t>(1), *itr);
            ++itr;
            CHECK_EQ(static_cast<std::uint32_t>(2), *itr);
            itr += 5;
            CHECK_EQ(static_cast<std::uint32_t>(7), *itr);
            --itr;
            CHECK_EQ(static_cast<std::uint32_t>(6), *itr);
            itr -= 2;
            CHECK_EQ(static_cast<std::uint32_t>(4), *itr);

            std::uint32_t val  = 4;
            const auto end_itr = array->end<std::uint32_t>();
            while(itr != end_itr)
            {
                CHECK_EQ(val, *itr);
                CHECK_NOTHROW(++itr);
                ++val;
            }

            CHECK_EQ(static_cast<std::uint32_t>(1000), val);
        }
        {
            auto itr = array->cbegin<std::uint32_t>();
            CHECK_EQ(static_cast<std::uint32_t>(0), *itr);
            itr++;
            CHECK_EQ(static_cast<std::uint32_t>(1), *itr);
            ++itr;
            CHECK_EQ(static_cast<std::uint32_t>(2), *itr);
            itr += 5;
            CHECK_EQ(static_cast<std::uint32_t>(7), *itr);
            --itr;
            CHECK_EQ(static_cast<std::uint32_t>(6), *itr);
            itr -= 2;
            CHECK_EQ(static_cast<std::uint32_t>(4), *itr);

            std::uint32_t val  = 4;
            const auto end_itr = array->end<std::uint32_t>();
            while(itr != end_itr)
            {
                CHECK_EQ(val, *itr);
                CHECK_NOTHROW(++itr);
                ++val;
            }
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("const_array")
    {
        sight::data::array::sptr array = std::make_shared<sight::data::array>();

        sight::data::array::size_t size = {10, 100};

        array->resize(size, sight::core::type::UINT32, true);
        auto lock = array->dump_lock();

        std::uint32_t count = 0;
        auto iter           = array->begin<std::uint32_t>();
        for( ; iter != array->end<std::uint32_t>() ; ++iter)
        {
            *iter = count++;
        }

        sight::data::array::csptr array2 = sight::data::object::copy(array);
        const auto lock2                 = array2->dump_lock();

        CHECK_EQ(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
        CHECK_EQ(static_cast<std::uint32_t>(0), array->at<std::uint32_t>(0));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
        CHECK_EQ(static_cast<std::uint32_t>(10), array->at<std::uint32_t>(10));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
        CHECK_EQ(static_cast<std::uint32_t>(999), array->at<std::uint32_t>(999));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
        CHECK_EQ(static_cast<std::uint32_t>(326), array->at<std::uint32_t>(326));
        CHECK_EQ(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
        CHECK_EQ(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

        auto it1        = array->begin<std::uint32_t>();
        auto it2        = array2->begin<std::uint32_t>();
        const auto end1 = array->end<std::uint32_t>();
        const auto end2 = array2->end<std::uint32_t>();

        while(it1 != end1 && it2 != end2)
        {
            CHECK_EQ(*it1, *it2);
            CHECK_NOTHROW(++it1);
            CHECK_NOTHROW(++it2);
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("empty_iterator")
    {
        sight::data::array::sptr array = std::make_shared<sight::data::array>();
        array->resize({10, 100}, sight::core::type::UINT32);
        auto lock = array->dump_lock();

        std::uint32_t count   = 0;
        auto iter_for_filling = array->begin<std::uint32_t>();
        const auto end        = array->end<std::uint32_t>();

        for( ; iter_for_filling != end ; ++iter_for_filling)
        {
            *iter_for_filling = count++;
        }

        auto iter = array->begin<std::uint32_t>();

        sight::data::array::iterator<std::uint32_t> max_iter;

        std::uint32_t max_value = *iter;
        for( ; iter != end ; ++iter)
        {
            if(*iter > max_value)
            {
                max_iter  = iter;
                max_value = *iter;
            }
        }

        CHECK_EQ(*max_iter, count - 1);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto array1 = std::make_shared<sight::data::array>();
        auto array2 = std::make_shared<sight::data::array>();

        CHECK(*array1 == *array2);

        // Fill array1
        const sight::data::array::size_t size {10, 10};

        array1->resize(size, sight::core::type::UINT32, true);
        auto lock = array1->dump_lock();

        std::uint32_t count = 0;
        // NOLINTNEXTLINE(modernize-loop-convert)
        for(auto it = array1->begin<std::uint32_t>(), end = array1->end<std::uint32_t>() ; it != end ; ++it)
        {
            *it = count++;
        }

        CHECK(*array1 != *array2);

        // Fill array2
        array2->resize(size, sight::core::type::UINT32, true);
        auto lock2 = array2->dump_lock();

        count = 666;
        // NOLINTNEXTLINE(modernize-loop-convert)
        for(auto it = array2->begin<std::uint32_t>(), end = array2->end<std::uint32_t>() ; it != end ; ++it)
        {
            *it = count++;
        }

        CHECK(*array1 != *array2);

        count = 0;
        // NOLINTNEXTLINE(modernize-loop-convert)
        for(auto it = array2->begin<std::uint32_t>(), end = array2->end<std::uint32_t>() ; it != end ; ++it)
        {
            *it = count++;
        }

        CHECK(*array1 == *array2);

        // Test also deepcopy, just for fun
        auto array3 = std::make_shared<sight::data::array>();
        array3->deep_copy(array1);
        CHECK(*array1 == *array3);
    }

//------------------------------------------------------------------------------

    TEST_CASE("swap")
    {
        auto array1 = std::make_shared<sight::data::array>();
        array1->resize({3}, sight::core::type::UINT32, true);
        auto lock1         = array1->dump_lock();
        std::uint8_t count = 1;
        // NOLINTNEXTLINE(modernize-loop-convert)
        for(auto it = array1->begin<std::uint32_t>(), end = array1->end<std::uint32_t>() ; it != end ; ++it)
        {
            *it = count++;
        }

        auto array2 = std::make_shared<sight::data::array>();
        array2->resize({6}, sight::core::type::INT16, true);
        auto lock2 = array2->dump_lock();
        // NOLINTNEXTLINE(modernize-loop-convert)
        for(auto it = array2->begin<std::int16_t>(), end = array2->end<std::int16_t>() ; it != end ; ++it)
        {
            *it = count++;
        }

        count = 1;
        std::size_t i = 0;
        for(auto it = array1->begin<std::uint32_t>(), end = array1->end<std::uint32_t>() ; it != end ; ++it, ++i)
        {
            CHECK(*it == count++);
        }

        i = 0;
        for(auto it = array2->begin<std::int16_t>(), end = array2->end<std::int16_t>() ; it != end ; ++it, ++i)
        {
            CHECK(*it == count++);
        }

        array1->swap(array2);

        count = 1;
        i     = 0;
        for(auto it = array2->begin<std::uint32_t>(), end = array2->end<std::uint32_t>() ; it != end ; ++it, ++i)
        {
            CHECK_MESSAGE(static_cast<std::uint32_t>(count++) == *it, "i=", std::to_string(i));
        }

        i = 0;
        for(auto it = array1->begin<std::int16_t>(), end = array1->end<std::int16_t>() ; it != end ; ++it, ++i)
        {
            CHECK_MESSAGE(static_cast<std::int16_t>(count++) == *it, "i=", std::to_string(i));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("resize_non_owner")
    {
        auto array   = std::make_shared<sight::data::array>();
        auto* buffer = new std::uint32_t[3];
        array->set_buffer(buffer, false, {3}, sight::core::type::UINT32);
        CHECK_THROWS_AS(array->resize({6}, sight::core::type::UINT32, true), sight::data::exception);
        delete[] buffer;
    }

//------------------------------------------------------------------------------

    TEST_CASE("set_buffer_object_null_then_resize")
    {
        auto array = std::make_shared<sight::data::array>();
        /* TODO: fix crash
           array->setBufferObject(nullptr);
           array->resize({2}, sight::core::type::UINT8);
         */
    }

//------------------------------------------------------------------------------

    TEST_CASE("at")
    {
        auto array = std::make_shared<sight::data::array>();
        array->resize({3}, sight::core::type::UINT32, true);

        const auto lock = array->dump_lock();

        for(std::uint32_t i = 0 ; i < 3 ; i++)
        {
            array->at<std::uint32_t>(i) = i + 1;
        }

        for(std::uint32_t i = 0 ; i < 3 ; i++)
        {
            CHECK_EQ(i + 1, array->at<std::uint32_t>(i));
        }
    }

//-----------------------------------------------------------------------------
} // TEST_SUITE("sight::data::array")
