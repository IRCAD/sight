/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "RecursiveLockTest.hpp"

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/reflection/visitor/RecursiveLock.hpp>

#include <utestData/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::reflection::ut::RecursiveLockTest);

namespace sight::data
{

namespace reflection
{

namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void RecursiveLockTest::setUp()
{
}

//-----------------------------------------------------------------------------

void RecursiveLockTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void RecursiveLockTest::lockTest()
{
    {
        data::Image::sptr image = data::Image::New();
        utestData::generator::Image::generateRandomImage(image, core::tools::Type::create("int16"));
        core::memory::BufferObject::sptr bo = image->getBufferObject();

        {
            data::reflection::visitor::RecursiveLock visitor(image);

            CPPUNIT_ASSERT(!image->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo->getMutex().try_lock());
        }

        CPPUNIT_ASSERT(image->getMutex().try_lock());
        CPPUNIT_ASSERT(bo->getMutex().try_lock());
    }

    {
        data::Image::sptr img1 = data::Image::New();
        utestData::generator::Image::generateRandomImage(img1, core::tools::Type::create("int16"));
        data::Image::sptr img2 = data::Image::New();
        utestData::generator::Image::generateRandomImage(img2, core::tools::Type::create("uint8"));
        data::Composite::sptr composite = data::Composite::New();
        composite->getContainer()["img1"] = img1;
        composite->getContainer()["img2"] = img2;

        core::memory::BufferObject::sptr bo1 = img1->getBufferObject();
        core::memory::BufferObject::sptr bo2 = img2->getBufferObject();

        {
            data::reflection::visitor::RecursiveLock visitor(composite);

            CPPUNIT_ASSERT(!composite->getMutex().try_lock());
            CPPUNIT_ASSERT(!img1->getMutex().try_lock());
            CPPUNIT_ASSERT(!img2->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo1->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo2->getMutex().try_lock());
        }

        CPPUNIT_ASSERT(composite->getMutex().try_lock());
        CPPUNIT_ASSERT(img1->getMutex().try_lock());
        CPPUNIT_ASSERT(img2->getMutex().try_lock());
        CPPUNIT_ASSERT(bo1->getMutex().try_lock());
        CPPUNIT_ASSERT(bo2->getMutex().try_lock());
    }
}

//-----------------------------------------------------------------------------

} // namespace ut

} // namespace reflection

} // namespace sight::data
