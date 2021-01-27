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

#include "GetObjectTest.hpp"

#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/exception/NullPointer.hpp>
#include <data/reflection/exception/ObjectNotFound.hpp>
#include <data/reflection/getObject.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <utestData/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( sight::data::reflection::ut::GetObjectTest );

namespace sight::data
{
namespace reflection
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void GetObjectTest::setUp()
{
}

//-----------------------------------------------------------------------------

void GetObjectTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void GetObjectTest::getTest()
{
    // Visit 1
    data::Image::sptr img1 = data::Image::New();
    utestData::generator::Image::generateRandomImage(img1, core::tools::Type::create("int16"));
    data::Image::sptr img2 = data::Image::New();
    utestData::generator::Image::generateRandomImage(img2, core::tools::Type::create("uint8"));
    data::Composite::sptr composite = data::Composite::New();
    composite->getContainer()["img1"] = img1;
    composite->getContainer()["img2"] = img2;
    data::Object::sptr subObj1 = data::reflection::getObject( composite, "@values.img2" );
    CPPUNIT_ASSERT_MESSAGE("Image must be equal", subObj1 == img2);

    // Visit 2
    data::Float::sptr zspacing = data::reflection::getObject< data::Float >( composite,
                                                                             "@values.img2.spacing.2" );
    CPPUNIT_ASSERT_MESSAGE("spacing must be equal",
                           img2->getSpacing2()[2] - 0.001 < zspacing->value() &&
                           zspacing->value() < img2->getSpacing2()[2] + 0.001 );

    // Visit 3
    composite->setField("myImage1", img1);
    img1->setField("myImage2", img2);
    data::Object::sptr subObj2 = data::reflection::getObject( composite, "@fields.myImage1.fields.myImage2" );
    CPPUNIT_ASSERT_MESSAGE("Image must be equal", subObj2 == img2 );
}

//-----------------------------------------------------------------------------

void GetObjectTest::invalidPathTest()
{
    data::Composite::sptr composite = data::Composite::New();
    data::String::sptr text         = data::String::New("Text");
    (*composite)["string"] = text;
    data::Integer::sptr intValue = data::Integer::New(321);
    data::Float::sptr floatValue = data::Float::New(1.234f);
    data::Vector::sptr vector    = data::Vector::New();
    data::Image::sptr img        = data::Image::New();
    utestData::generator::Image::generateRandomImage(img, core::tools::Type::create("int16"));

    auto& internalVector = vector->getContainer();
    internalVector.push_back(intValue);
    internalVector.push_back(floatValue);
    internalVector.push_back(img);
    (*composite)["vector"] = vector;

    // no exception version
    data::Object::sptr obj = data::reflection::getObject( composite, "@values.string" );
    CPPUNIT_ASSERT_MESSAGE("::sight::data::String must be equal", obj == text );

    // with exception version
    obj = data::reflection::getObject( composite, "@values.string", true );
    CPPUNIT_ASSERT_MESSAGE("::sight::data::String must be equal", obj == text );

    // no exception version
    data::Object::sptr invalidObj = data::reflection::getObject( composite, "@values.invalidPath", false );
    CPPUNIT_ASSERT_MESSAGE("Object must not exist", !invalidObj );

    // exception version : path is invalid
    CPPUNIT_ASSERT_THROW(
        data::reflection::getObject( composite, "@values.invalidPath", true ),
        data::reflection::exception::ObjectNotFound
        );
    CPPUNIT_ASSERT_EQUAL(size_t(2), composite->size() );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Vector tests

    // no exception version
    obj = data::reflection::getObject( composite, "@values.vector.values.0" );
    CPPUNIT_ASSERT_MESSAGE("::sight::data::Integer must be equal", obj == intValue );

    // with exception version
    obj = data::reflection::getObject( composite, "@values.vector.values.0", true );
    CPPUNIT_ASSERT_MESSAGE("::sight::data::Integer must be equal", obj == intValue );

    /// This is important to test vectors subobjects properties to ensure the visitor path is correct

    // no exception version
    obj = data::reflection::getObject( composite, "@values.vector.values.2.spacing.2" );
    data::Float::sptr zspacing = ::std::dynamic_pointer_cast< data::Float >(obj);
    CPPUNIT_ASSERT_MESSAGE("spacing must be equal",
                           img->getSpacing2()[2] - 0.001 < zspacing->value() &&
                           zspacing->value() < img->getSpacing2()[2] + 0.001 );

    // with exception version
    obj      = data::reflection::getObject( composite, "@values.vector.values.2.spacing.2", true );
    zspacing = ::std::dynamic_pointer_cast< data::Float >(obj);
    CPPUNIT_ASSERT_MESSAGE("spacing must be equal",
                           img->getSpacing2()[2] - 0.001 < zspacing->value() &&
                           zspacing->value() < img->getSpacing2()[2] + 0.001 );

    // out of bounds, no exception version
    invalidObj = data::reflection::getObject( composite, "@values.vector.values.2.spacing.15", false );
    CPPUNIT_ASSERT_MESSAGE("Object must not exist", !invalidObj );

    // out of bounds, with exception version
    CPPUNIT_ASSERT_THROW( data::reflection::getObject( composite, "@values.vector.values.2.spacing.15", true ),
                          data::reflection::exception::NullPointer);

}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace reflection
} // namespace sight::data
