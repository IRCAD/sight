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

#include "detail/reflection/CompareObjectsTest.hpp"

#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <core/Exception.hpp>
#include <core/tools/Type.hpp>

#include <data/reflection/visitor/CompareObjects.hpp>

#include <fwTest/generator/SeriesDB.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::detail::reflection::ut::CompareObjectsTest );

namespace fwMedData
{
namespace detail
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

void CompareObjectsTest::compareSeriesDBTest()
{
    ::fwMedData::SeriesDB::sptr seriesDBRef  = ::fwTest::generator::SeriesDB::createSeriesDB(1, 1, 1);
    ::fwMedData::SeriesDB::sptr seriesDBComp = data::Object::copy< ::fwMedData::SeriesDB >(seriesDBRef);

    {
        data::reflection::visitor::CompareObjects visitor;
        visitor.compare(seriesDBRef, seriesDBComp);

        SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT_EQUAL(size_t(0), props->size());
    }

    {
        CPPUNIT_ASSERT_EQUAL(size_t(3), seriesDBComp->getContainer().size());
        ::fwMedData::Series::sptr series = seriesDBComp->getContainer()[0];
        CPPUNIT_ASSERT(series);

        ::fwMedData::Patient::sptr patient = series->getPatient();
        const std::string name = patient->getName() + "X";

        patient->setName(name);
        patient->setSex("M");
        patient->setPatientId("42");

        data::reflection::visitor::CompareObjects visitor;
        visitor.compare(seriesDBRef, seriesDBComp);

        SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)3);

        CPPUNIT_ASSERT(props->find("values.0.patient.name") != props->end());
        CPPUNIT_ASSERT_EQUAL(name, (*props)["values.0.patient.name"]);

        CPPUNIT_ASSERT(props->find("values.0.patient.patient_id") != props->end());
        CPPUNIT_ASSERT_EQUAL(std::string("42"), (*props)["values.0.patient.patient_id"]);

        CPPUNIT_ASSERT(props->find("values.0.patient.sex") != props->end());
        CPPUNIT_ASSERT_EQUAL(std::string("M"), (*props)["values.0.patient.sex"]);
    }
}

//-----------------------------------------------------------------------------

} // namespace ut
} //namespace reflection
} //namespace detail
} // namespace fwMedData
