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

#include "reflection/CompareObjectsTest.hpp"

#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>

#include <core/Exception.hpp>
#include <core/tools/Type.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/generator/SeriesDB.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( data::reflection::ut::CompareObjectsTest );

namespace sight::data
{
namespace reflection
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void CompareObjectsTest::setUp()
{
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareImageTest()
{
    core::tools::Type type = core::tools::Type::create< float >();
    data::Image::sptr img  = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, type);

    data::Image::sptr imgComp = data::Object::copy(img);

    data::Image::Spacing spacing = imgComp->getSpacing2();
    spacing[0] = 42;
    imgComp->setSpacing2(spacing);

    data::Image::Origin origin = imgComp->getOrigin2();
    origin[2] = 1664;
    imgComp->setOrigin2(origin);

    visitor::CompareObjects visitor;
    visitor.compare(img, imgComp);

    SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    CPPUNIT_ASSERT_EQUAL(size_t(2), props->size());

    CPPUNIT_ASSERT(props->find("spacing.0") != props->end());
    CPPUNIT_ASSERT_EQUAL(std::string("42"), (*props)["spacing.0"]);

    CPPUNIT_ASSERT(props->find("origin.2") != props->end());
    CPPUNIT_ASSERT_EQUAL(std::string("1664"), (*props)["origin.2"]);
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareReconstructionTest()
{
    data::Reconstruction::sptr rec = data::Reconstruction::New();
    ::fwTest::generator::SeriesDB::generateReconstruction(rec);

    data::Reconstruction::sptr recComp = data::Object::copy< data::Reconstruction >(rec);

    recComp->setOrganName("Unknown organ name");
    recComp->setIsVisible(!rec->getIsVisible());

    visitor::CompareObjects visitor;
    visitor.compare(rec, recComp);

    SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    CPPUNIT_ASSERT_EQUAL((size_t)2, props->size());

    CPPUNIT_ASSERT(props->find("organ_name") != props->end());
    CPPUNIT_ASSERT_EQUAL(std::string("Unknown organ name"), (*props)["organ_name"]);

    CPPUNIT_ASSERT(props->find("is_visible") != props->end());
    CPPUNIT_ASSERT_EQUAL(std::string(recComp->getIsVisible() ? "true" : "false"), (*props)["is_visible"]);
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareBufferTest()
{
    data::Image::sptr imgRef  = data::Image::New();
    data::Image::sptr imgComp = data::Image::New();

    core::tools::Type typeRef  = core::tools::Type::create< float >();
    core::tools::Type typeComp = core::tools::Type::create< double >();

    {
        ::fwTest::generator::Image::generateRandomImage(imgRef, typeRef);
        ::fwTest::generator::Image::generateRandomImage(imgComp, typeComp);

        visitor::CompareObjects visitor;
        visitor.compare(imgRef, imgComp);

        SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT(!props->empty());
        CPPUNIT_ASSERT(props->find("array.buffer") != props->end());
    }

    {
        imgComp = data::Object::copy(imgRef);
        visitor::CompareObjects visitor;
        visitor.compare(imgRef, imgComp);

        SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)0);
    }
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareEmpty()
{
    data::Composite::sptr compRef  = data::Composite::New();
    data::Composite::sptr compComp = data::Composite::New();

    (*compRef)["float"] = data::Float::New(0.0f);
    (*compComp)["int"]  = data::Integer::New(0);

    visitor::CompareObjects visitor;
    visitor.compare(compComp, compRef);

    SPTR(visitor::CompareObjects::PropsMapType) diffs = visitor.getDifferences();
    CPPUNIT_ASSERT_EQUAL(diffs->size(), (size_t)2);
    CPPUNIT_ASSERT(diffs->find("values.float.value") != diffs->end());
    CPPUNIT_ASSERT(diffs->find("values.int.value") != diffs->end());
    CPPUNIT_ASSERT_EQUAL((*diffs)["values.float.value"], visitor::CompareObjects::s_MISSING_PROPERTY);
    CPPUNIT_ASSERT_EQUAL((*diffs)["values.int.value"], visitor::CompareObjects::s_MISSING_PROPERTY);
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::exceptionTest()
{
    data::Image::sptr img      = data::Image::New();
    data::Composite::sptr comp = data::Composite::New();

    visitor::CompareObjects visitor;
    CPPUNIT_ASSERT_THROW(visitor.compare(img, comp), core::Exception);

    CPPUNIT_ASSERT_EQUAL(visitor.getReferenceProps().size(), (size_t)0);
    CPPUNIT_ASSERT_EQUAL(visitor.getComparedProps().size(), (size_t)0);
    CPPUNIT_ASSERT_EQUAL(visitor.getDifferences()->size(), (size_t)0);
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace reflection
} // namespace sight::data
