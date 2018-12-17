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

#include "ActivitySeriesTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwData/Composite.hpp>

#include <fwDataCamp/Version.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <boost/assign/std/vector.hpp>

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::ActivitySeriesTest );

namespace fwMedDataCamp
{
namespace ut
{

//------------------------------------------------------------------------------

void ActivitySeriesTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    m_fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwDataVersion);

    //Force link with fwMedDataCamp
    m_fwMedDataVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwMedDataVersion);
}

//------------------------------------------------------------------------------

void ActivitySeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ActivitySeriesTest::propertiesTest()
{
    const ::fwMedData::ActivitySeries::ConfigIdType activity_config_id = "Visu2D";

    const ::DataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                    ("patient")
                                                                    ("study")
                                                                    ("equipment")
                                                                    ("instance_uid")
                                                                    ("modality")
                                                                    ("date")
                                                                    ("time")
                                                                    ("performing_physicians_name")
                                                                    ("description")
                                                                    ("activity_config_id")
                                                                    ("data");

    ::fwData::Composite::sptr data = ::fwData::Composite::New();

    ::fwMedData::ActivitySeries::sptr obj = ::fwMedData::ActivitySeries::New();
    obj->setActivityConfigId(activity_config_id);
    obj->setData(data);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@activity_config_id", activity_config_id);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@data", obj->getData());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@data", data);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
