/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "array_test.hpp"

#include <data/array.hpp>
#include <data/exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::array_test);

namespace sight::data::ut
{

//-----------------------------------------------------------------------------

void array_test::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void array_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void array_test::allocation()
{
    data::array::sptr array = std::make_shared<data::array>();
    auto lock               = array->dump_lock();

    CPPUNIT_ASSERT(array->empty());
    CPPUNIT_ASSERT(array->buffer() == nullptr);
    CPPUNIT_ASSERT(array->size().empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), array->size_in_bytes());

    data::array::size_t size = {10, 100};

    array->resize(size, core::type::UINT32, true);
    CPPUNIT_ASSERT(array->buffer() != nullptr);
    CPPUNIT_ASSERT(!array->empty());

    CPPUNIT_ASSERT_EQUAL(size.size(), array->num_dimensions());
    CPPUNIT_ASSERT_EQUAL(size[0], array->size()[0]);
    CPPUNIT_ASSERT_EQUAL(size[1], array->size()[1]);
    CPPUNIT_ASSERT_EQUAL(true, array->get_is_buffer_owner());
    {
        data::array::offset_t stride = {4, 40};
        CPPUNIT_ASSERT(array->get_strides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(core::type::UINT32, array->type());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), array->type().size());

    array->clear();
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), array->size_in_bytes());
    CPPUNIT_ASSERT(array->empty());
    CPPUNIT_ASSERT(array->buffer() == nullptr);
    CPPUNIT_ASSERT_EQUAL(core::type::NONE, array->type());

    auto* buffer = new std::uint16_t[1000];

    for(std::uint16_t i = 0 ; i < 1000 ; i++)
    {
        buffer[i] = i;
    }

    array->set_buffer(buffer, false, size, core::type::UINT16, std::make_shared<core::memory::buffer_new_policy>());

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2 * 10 * 100), array->size_in_bytes());
    {
        data::array::offset_t stride = {2, 20};
        CPPUNIT_ASSERT(array->get_strides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(buffer[0], array->at<std::uint16_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(buffer[10], array->at<std::uint16_t>({0, 1}));
    CPPUNIT_ASSERT_EQUAL(buffer[999], array->at<std::uint16_t>({9, 99}));
    CPPUNIT_ASSERT_EQUAL(buffer[326], array->at<std::uint16_t>({6, 32}));
    CPPUNIT_ASSERT_EQUAL(buffer[947], array->at<std::uint16_t>({7, 94}));
    CPPUNIT_ASSERT_EQUAL(buffer[238], array->at<std::uint16_t>({8, 23}));
    CPPUNIT_ASSERT_EQUAL(false, array->get_is_buffer_owner());

    array->clear();

    delete[] buffer;
}

//-----------------------------------------------------------------------------

void array_test::resize()
{
    data::array::sptr array = std::make_shared<data::array>();

    data::array::size_t size {10, 100};

    array->resize(size, core::type::UINT32, true);
    auto lock = array->dump_lock();

    CPPUNIT_ASSERT(array->buffer() != nullptr);

    std::uint32_t count = 0;

    auto iter = array->begin<std::uint32_t>();

    for( ; iter != array->end<std::uint32_t>() ; ++iter)
    {
        *iter = count++;
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
    {
        data::array::offset_t stride = {4, 40};
        CPPUNIT_ASSERT(array->get_strides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

    data::array::size_t new_size = {100, 10};

    array->resize(new_size);
    CPPUNIT_ASSERT(new_size == array->size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
    {
        data::array::offset_t stride = {4, 400};
        CPPUNIT_ASSERT(array->get_strides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({10, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({99, 9}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({26, 3}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({47, 9}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({38, 2}));

    new_size.clear();
    new_size = {25, 40};

    array->resize(new_size);
    CPPUNIT_ASSERT(new_size == array->size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), array->element_size_in_bytes());
    {
        data::array::offset_t stride = {4, 100};
        CPPUNIT_ASSERT(array->get_strides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({10, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({24, 39}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({1, 13}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({22, 37}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({13, 9}));

    new_size.clear();

    new_size = {2, 10, 100};

    array->resize(new_size, core::type::UINT16, false);
    CPPUNIT_ASSERT(new_size == array->size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2 * 100 * 10 * 2), array->size_in_bytes());
    {
        data::array::offset_t stride = {2, 4, 40};
        CPPUNIT_ASSERT(array->get_strides() == stride);
    }
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({0, 9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({0, 6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({0, 7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({0, 8, 23}));
}

//-----------------------------------------------------------------------------

void array_test::reallocate()
{
    data::array::sptr array = std::make_shared<data::array>();

    data::array::size_t size = {10, 100};

    array->resize(size, core::type::UINT32, true);
    auto lock = array->dump_lock();

    std::uint32_t count = 0;
    auto iter           = array->begin<std::uint32_t>();
    auto iter_end       = array->end<std::uint32_t>();
    for( ; iter != iter_end ; ++iter)
    {
        *iter = count++;
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

    data::array::size_t new_size = {100, 100};

    array->resize(new_size, true);
    CPPUNIT_ASSERT(new_size == array->size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4 * 100 * 100), array->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({10, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({99, 9}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({26, 3}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({47, 9}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({38, 2}));

    std::uint32_t value = 1859;
    array->at<std::uint32_t>({50, 90}) = value;
    CPPUNIT_ASSERT_EQUAL(value, array->at<std::uint32_t>({50, 90}));

    std::uint32_t value2 = 25464;
    array->at<std::uint32_t>({99, 99}) = value2;
    CPPUNIT_ASSERT_EQUAL(value2, array->at<std::uint32_t>({99, 99}));

    new_size = {2, 100, 100};
    array->resize(new_size, core::type::UINT32, true);
    CPPUNIT_ASSERT(new_size == array->size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4 * 2 * 100 * 100), array->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 5, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({1, 99, 4}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({0, 63, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({1, 73, 4}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({0, 19, 1}));

    CPPUNIT_ASSERT_EQUAL(value, array->at<std::uint32_t>({0, 25, 45}));

    CPPUNIT_ASSERT_EQUAL(value2, array->at<std::uint32_t>({1, 99, 49}));

    std::uint32_t value3 = 2156;
    array->at<std::uint32_t>({0, 35, 48}) = value3;
    CPPUNIT_ASSERT_EQUAL(value3, array->at<std::uint32_t>({0, 35, 48}));

    std::uint32_t value4 = 105;
    array->at<std::uint32_t>({1, 99, 99}) = value4;
    CPPUNIT_ASSERT_EQUAL(value4, array->at<std::uint32_t>({1, 99, 99}));

    new_size.clear();

    new_size = {10, 100};

    array->resize(new_size, true);
    CPPUNIT_ASSERT(new_size == array->size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

    new_size = {2, 10, 100};
    array->resize(new_size, core::type::UINT16, true);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2 * 10 * 100 * 2), array->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({0, 9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({0, 6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({0, 7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({0, 8, 23}));

    new_size = {10, 100};
    array->resize(new_size, true);
    CPPUNIT_ASSERT(new_size == array->size());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2 * 10 * 100), array->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 2}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({2, 65}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({6, 47}));

    array->clear();
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), array->size_in_bytes());
}

//-----------------------------------------------------------------------------

void array_test::copy()
{
    data::array::sptr array = std::make_shared<data::array>();

    data::array::size_t size = {10, 100};

    array->resize(size, core::type::UINT32, true);
    auto array_lock = array->dump_lock();
    CPPUNIT_ASSERT(array->buffer() != nullptr);

    std::uint32_t count = 0;
    auto iter           = array->begin<std::uint32_t>();
    auto iter_end       = array->end<std::uint32_t>();

    for( ; iter != iter_end ; ++iter)
    {
        *iter = count++;
    }

    data::array::sptr deep_copy_array = data::object::copy(array);
    auto deep_copy_array_lock         = deep_copy_array->dump_lock();

    // check deep_copy
    CPPUNIT_ASSERT_EQUAL(array->element_size_in_bytes(), deep_copy_array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(array->size_in_bytes(), deep_copy_array->size_in_bytes());
    CPPUNIT_ASSERT(array->get_strides() == deep_copy_array->get_strides());
    CPPUNIT_ASSERT(array->size() == deep_copy_array->size());
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({0, 0}),
        deep_copy_array->at<std::uint32_t>({0, 0})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({0, 1}),
        deep_copy_array->at<std::uint32_t>({0, 1})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({9, 99}),
        deep_copy_array->at<std::uint32_t>({9, 99})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({6, 32}),
        deep_copy_array->at<std::uint32_t>({6, 32})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({7, 94}),
        deep_copy_array->at<std::uint32_t>({7, 94})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({8, 23}),
        deep_copy_array->at<std::uint32_t>({8, 23})
    );
    CPPUNIT_ASSERT_EQUAL(true, deep_copy_array->get_is_buffer_owner());

    data::array::csptr deep_copy_array2 = data::object::copy(array);
    auto deep_copy_array_lock2          = deep_copy_array->dump_lock();

    CPPUNIT_ASSERT_EQUAL(array->element_size_in_bytes(), deep_copy_array->element_size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(array->size_in_bytes(), deep_copy_array->size_in_bytes());
    CPPUNIT_ASSERT(array->get_strides() == deep_copy_array->get_strides());
    CPPUNIT_ASSERT(array->size() == deep_copy_array->size());
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({0, 0}),
        deep_copy_array->at<std::uint32_t>({0, 0})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({0, 1}),
        deep_copy_array->at<std::uint32_t>({0, 1})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({9, 99}),
        deep_copy_array->at<std::uint32_t>({9, 99})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({6, 32}),
        deep_copy_array->at<std::uint32_t>({6, 32})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({7, 94}),
        deep_copy_array->at<std::uint32_t>({7, 94})
    );
    CPPUNIT_ASSERT_EQUAL(
        array->at<std::uint32_t>({8, 23}),
        deep_copy_array->at<std::uint32_t>({8, 23})
    );
    CPPUNIT_ASSERT_EQUAL(true, deep_copy_array->get_is_buffer_owner());
}

//-----------------------------------------------------------------------------

void array_test::dump_lock_test()
{
    data::array::size_t size = {10, 100};
    data::array::sptr array  = std::make_shared<data::array>();
    array->resize({12, 15}, core::type::INT16, true);

    CPPUNIT_ASSERT_THROW(array->buffer(), data::exception);

    auto lock = array->dump_lock();
    CPPUNIT_ASSERT_NO_THROW(array->buffer());
}

//-----------------------------------------------------------------------------

void array_test::buffer_access_test()
{
    // test get_buffer
    data::array::sptr array = std::make_shared<data::array>();

    data::array::size_t size = {10, 100};

    array->resize(size, core::type::UINT32, true);

    {
        CPPUNIT_ASSERT_THROW(array->buffer(), core::exception);
        CPPUNIT_ASSERT_THROW(array->begin(), core::exception);
        CPPUNIT_ASSERT_THROW(array->end(), core::exception);
    }

    const auto lock = array->dump_lock();
    {
        // Check that the iterator properly lock the buffer
        CPPUNIT_ASSERT_NO_THROW(array->begin());
        CPPUNIT_ASSERT_NO_THROW(array->buffer());
    }

    std::uint32_t count = 0;
    for(auto& iter : array->range<std::uint32_t>())
    {
        iter = count++;
    }

    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(1000), count);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(99), array->at<std::uint32_t>({9, 9}));

    CPPUNIT_ASSERT_THROW(array->at<std::uint32_t>({10, 0}), data::exception);
    CPPUNIT_ASSERT_THROW(array->at<std::uint32_t>({0, 100}), data::exception);

    {
        auto itr = array->begin<std::uint32_t>();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), *itr);
        itr++;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(1), *itr);
        ++itr;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(2), *itr);
        itr += 5;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(7), *itr);
        --itr;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(6), *itr);
        itr -= 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(4), *itr);

        std::uint32_t val  = 4;
        const auto end_itr = array->end<std::uint32_t>();
        while(itr != end_itr)
        {
            CPPUNIT_ASSERT_EQUAL(val, *itr);
            CPPUNIT_ASSERT_NO_THROW(++itr);
            ++val;
        }

        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(1000), val);
    }
    {
        auto itr = array->cbegin<std::uint32_t>();
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), *itr);
        itr++;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(1), *itr);
        ++itr;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(2), *itr);
        itr += 5;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(7), *itr);
        --itr;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(6), *itr);
        itr -= 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(4), *itr);

        std::uint32_t val  = 4;
        const auto end_itr = array->end<std::uint32_t>();
        while(itr != end_itr)
        {
            CPPUNIT_ASSERT_EQUAL(val, *itr);
            CPPUNIT_ASSERT_NO_THROW(++itr);
            ++val;
        }
    }
}

//-----------------------------------------------------------------------------

void array_test::const_array_test()
{
    data::array::sptr array = std::make_shared<data::array>();

    data::array::size_t size = {10, 100};

    array->resize(size, core::type::UINT32, true);
    auto lock = array->dump_lock();

    std::uint32_t count = 0;
    auto iter           = array->begin<std::uint32_t>();
    for( ; iter != array->end<std::uint32_t>() ; ++iter)
    {
        *iter = count++;
    }

    data::array::csptr array2 = data::object::copy(array);
    const auto lock2          = array2->dump_lock();

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4 * 10 * 100), array->size_in_bytes());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>({0, 0}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(0), array->at<std::uint32_t>(0));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>({0, 1}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(10), array->at<std::uint32_t>(10));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>({9, 99}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(999), array->at<std::uint32_t>(999));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>({6, 32}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(326), array->at<std::uint32_t>(326));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(947), array->at<std::uint32_t>({7, 94}));
    CPPUNIT_ASSERT_EQUAL(static_cast<std::uint32_t>(238), array->at<std::uint32_t>({8, 23}));

    auto it1        = array->begin<std::uint32_t>();
    auto it2        = array2->begin<std::uint32_t>();
    const auto end1 = array->end<std::uint32_t>();
    const auto end2 = array2->end<std::uint32_t>();

    while(it1 != end1 && it2 != end2)
    {
        CPPUNIT_ASSERT_EQUAL(*it1, *it2);
        CPPUNIT_ASSERT_NO_THROW(++it1);
        CPPUNIT_ASSERT_NO_THROW(++it2);
    }
}

//-----------------------------------------------------------------------------

void array_test::empty_iterator_test()
{
    data::array::sptr array = std::make_shared<data::array>();
    array->resize({10, 100}, core::type::UINT32);
    auto lock = array->dump_lock();

    std::uint32_t count   = 0;
    auto iter_for_filling = array->begin<std::uint32_t>();
    const auto end        = array->end<std::uint32_t>();

    for( ; iter_for_filling != end ; ++iter_for_filling)
    {
        *iter_for_filling = count++;
    }

    auto iter = array->begin<std::uint32_t>();

    data::array::iterator<std::uint32_t> max_iter;

    std::uint32_t max_value = *iter;
    for( ; iter != end ; ++iter)
    {
        if(*iter > max_value)
        {
            max_iter  = iter;
            max_value = *iter;
        }
    }

    CPPUNIT_ASSERT_EQUAL(*max_iter, count - 1);
}

//------------------------------------------------------------------------------

void array_test::equality_test()
{
    auto array1 = std::make_shared<data::array>();
    auto array2 = std::make_shared<data::array>();

    CPPUNIT_ASSERT(*array1 == *array2);

    // Fill array1
    const data::array::size_t size {10, 10};

    array1->resize(size, core::type::UINT32, true);
    auto lock = array1->dump_lock();

    std::uint32_t count = 0;
    // NOLINTNEXTLINE(modernize-loop-convert)
    for(auto it = array1->begin<std::uint32_t>(), end = array1->end<std::uint32_t>() ; it != end ; ++it)
    {
        *it = count++;
    }

    CPPUNIT_ASSERT(*array1 != *array2);

    // Fill array2
    array2->resize(size, core::type::UINT32, true);
    auto lock2 = array2->dump_lock();

    count = 666;
    // NOLINTNEXTLINE(modernize-loop-convert)
    for(auto it = array2->begin<std::uint32_t>(), end = array2->end<std::uint32_t>() ; it != end ; ++it)
    {
        *it = count++;
    }

    CPPUNIT_ASSERT(*array1 != *array2);

    count = 0;
    // NOLINTNEXTLINE(modernize-loop-convert)
    for(auto it = array2->begin<std::uint32_t>(), end = array2->end<std::uint32_t>() ; it != end ; ++it)
    {
        *it = count++;
    }

    CPPUNIT_ASSERT(*array1 == *array2);

    // Test also deepcopy, just for fun
    auto array3 = std::make_shared<data::array>();
    array3->deep_copy(array1);
    CPPUNIT_ASSERT(*array1 == *array3);
}

//------------------------------------------------------------------------------

void array_test::swap_test()
{
    auto array1 = std::make_shared<data::array>();
    array1->resize({3}, core::type::UINT32, true);
    auto lock1         = array1->dump_lock();
    std::uint8_t count = 1;
    // NOLINTNEXTLINE(modernize-loop-convert)
    for(auto it = array1->begin<std::uint32_t>(), end = array1->end<std::uint32_t>() ; it != end ; ++it)
    {
        *it = count++;
    }

    auto array2 = std::make_shared<data::array>();
    array2->resize({6}, core::type::INT16, true);
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
        CPPUNIT_ASSERT_MESSAGE("i=" + std::to_string(i), *it == count++);
    }

    i = 0;
    for(auto it = array2->begin<std::int16_t>(), end = array2->end<std::int16_t>() ; it != end ; ++it, ++i)
    {
        CPPUNIT_ASSERT_MESSAGE("i=" + std::to_string(i), *it == count++);
    }

    array1->swap(array2);

    count = 1;
    i     = 0;
    for(auto it = array2->begin<std::uint32_t>(), end = array2->end<std::uint32_t>() ; it != end ; ++it, ++i)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("i=" + std::to_string(i), static_cast<std::uint32_t>(count++), *it);
    }

    i = 0;
    for(auto it = array1->begin<std::int16_t>(), end = array1->end<std::int16_t>() ; it != end ; ++it, ++i)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("i=" + std::to_string(i), static_cast<std::int16_t>(count++), *it);
    }
}

//------------------------------------------------------------------------------

void array_test::resize_non_owner_test()
{
    auto array   = std::make_shared<data::array>();
    auto* buffer = new std::uint32_t[3];
    array->set_buffer(buffer, false, {3}, core::type::UINT32);
    CPPUNIT_ASSERT_THROW(array->resize({6}, core::type::UINT32, true), data::exception);
    delete[] buffer;
}

//------------------------------------------------------------------------------

void array_test::set_buffer_object_null_then_resize_test()
{
    auto array = std::make_shared<data::array>();
    /* TODO: fix crash
       array->setBufferObject(nullptr);
       array->resize({2}, core::type::UINT8);
     */
}

//------------------------------------------------------------------------------

void array_test::at_test()
{
    auto array = std::make_shared<data::array>();
    array->resize({3}, core::type::UINT32, true);

    for(std::uint32_t i = 0 ; i < 3 ; i++)
    {
        array->at<std::uint32_t>(i) = i + 1;
    }

    for(std::uint32_t i = 0 ; i < 3 ; i++)
    {
        CPPUNIT_ASSERT_EQUAL(i + 1, array->at<std::uint32_t>(i));
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::data::ut
