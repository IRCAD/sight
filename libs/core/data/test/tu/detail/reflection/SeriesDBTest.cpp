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

#include "SeriesDBTest.hpp"

#include "DataCampHelper.hpp"

#include <atoms/conversion/convert.hpp>
#include <atoms/Sequence.hpp>

#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/reflection/getObject.hpp>
#include <data/SeriesDB.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::detail::reflection::ut::SeriesDBTest);

namespace sight::data
{

namespace detail
{

namespace reflection
{

namespace ut
{

//------------------------------------------------------------------------------

void SeriesDBTest::setUp()
{
}

//------------------------------------------------------------------------------

void SeriesDBTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesDBTest::propertiesTest()
{
    const ::DataCampHelper::PropertiesNameType dataProperties = {"fields", "values"};

    data::SeriesDB::sptr obj = data::SeriesDB::New();
    data::SeriesDB::ContainerType vectSeries;
    vectSeries.push_back(data::ImageSeries::New());
    vectSeries.push_back(data::ImageSeries::New());
    vectSeries.push_back(data::ModelSeries::New());
    obj->setContainer(vectSeries);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@values.0", vectSeries[0]);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@values.1", vectSeries[1]);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@values.2", vectSeries[2]);

    atoms::Object::sptr metaObject      = atoms::conversion::convert(obj);
    atoms::Object::AttributesType attrs = metaObject->getAttributes();

    CPPUNIT_ASSERT_MESSAGE(
        "Attributes values not found in SeriesDB atom",
        attrs.find("values") != attrs.end()
    );

    atoms::Base::sptr baseAtom = metaObject->getAttribute("values");
    CPPUNIT_ASSERT_MESSAGE("Bad Atom SeriesDB conversion", baseAtom->isSequence());

    atoms::Sequence::sptr seqAtom = atoms::Sequence::dynamicCast(baseAtom);
    CPPUNIT_ASSERT_MESSAGE("Sequence dynamicCast failed", seqAtom);
    CPPUNIT_ASSERT_EQUAL(vectSeries.size(), seqAtom->getValue().size());
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace reflection

} //namespace detail

} //namespace sight::data
