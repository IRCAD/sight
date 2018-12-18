/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "CompareObjectsTest.hpp"

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwCore/Exception.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Float.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/generator/SeriesDB.hpp>

#include <fwTools/Type.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataCamp::ut::CompareObjectsTest );

namespace fwDataCamp
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
    ::fwTools::Type type      = ::fwTools::Type::create< float >();
    ::fwData::Image::sptr img = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, type);

    ::fwData::Image::sptr imgComp = ::fwData::Object::copy(img);

    ::fwData::Image::SpacingType spacing = imgComp->getSpacing();
    spacing[0]                           = 42;
    imgComp->setSpacing(spacing);

    ::fwData::Image::OriginType origin = imgComp->getOrigin();
    origin[2]                          = 1664;
    imgComp->setOrigin(origin);

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
    ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::New();
    ::fwTest::generator::SeriesDB::generateReconstruction(rec);

    ::fwData::Reconstruction::sptr recComp = ::fwData::Object::copy< ::fwData::Reconstruction >(rec);

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
    ::fwData::Image::sptr imgRef  = ::fwData::Image::New();
    ::fwData::Image::sptr imgComp = ::fwData::Image::New();

    ::fwTools::Type typeRef  = ::fwTools::Type::create< float >();
    ::fwTools::Type typeComp = ::fwTools::Type::create< double >();

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
        imgComp = ::fwData::Object::copy(imgRef);
        visitor::CompareObjects visitor;
        visitor.compare(imgRef, imgComp);

        SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)0);
    }
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareEmpty()
{
    ::fwData::Composite::sptr compRef  = ::fwData::Composite::New();
    ::fwData::Composite::sptr compComp = ::fwData::Composite::New();

    (*compRef)["float"] = ::fwData::Float::New(0.0f);
    (*compComp)["int"]  = ::fwData::Integer::New(0);

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
    ::fwData::Image::sptr img      = ::fwData::Image::New();
    ::fwData::Composite::sptr comp = ::fwData::Composite::New();

    visitor::CompareObjects visitor;
    CPPUNIT_ASSERT_THROW(visitor.compare(img, comp), ::fwCore::Exception);

    CPPUNIT_ASSERT_EQUAL(visitor.getReferenceProps().size(), (size_t)0);
    CPPUNIT_ASSERT_EQUAL(visitor.getComparedProps().size(), (size_t)0);
    CPPUNIT_ASSERT_EQUAL(visitor.getDifferences()->size(), (size_t)0);
}

//-----------------------------------------------------------------------------

}  // namespace ut
}  // namespace fwDataCamp
