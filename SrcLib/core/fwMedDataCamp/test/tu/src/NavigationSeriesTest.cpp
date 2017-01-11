/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "NavigationSeriesTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/NavigationSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::NavigationSeriesTest );

namespace fwMedDataCamp
{
namespace ut
{

void NavigationSeriesTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwMedDataCamp
    const int version = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

//------------------------------------------------------------------------------

void NavigationSeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void NavigationSeriesTest::propertiesTest()
{
    const std::string instance_uid                            = "123456789";
    const std::string modality                                = "CT";
    const std::string date                                    = "20130214";
    const std::string time                                    = "143328";
    const std::string performing_physicians_name              = "John Doe";
    const std::string description                             = "description";
    const ::DataCampHelper::PropertiesNameType dataProperties = {{"fields"},
                                                                 {"patient"},
                                                                 {"study"},
                                                                 {"equipment"},
                                                                 {"instance_uid"},
                                                                 {"modality"},
                                                                 {"date"},
                                                                 {"time"},
                                                                 {"performing_physicians_name"},
                                                                 {"description"},
                                                                 {"position"},
                                                                 {"move"},
                                                                 {"look_at"}};

    ::fwMedData::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);

    ::fwMedData::NavigationSeries::sptr obj = ::fwMedData::NavigationSeries::New();
    obj->setInstanceUID(instance_uid);
    obj->setModality(modality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setDescription(description);

    ::fwMedData::NavigationSeries::ContainerType positions =
    {
        {0., {-514.1606513019431, -94.68116795798758, 979.3785055174628}}
    };

    ::fwMedData::NavigationSeries::ContainerType move =
    {
        {0., {0.897820633950464, 0.13607544320686907, -1.2769419285130967}},
        {1421.375, {2.322431935080602, 0.33708203932328956, -3.734284956660633}},
        {2114.208, {2.43713135678714, -0.049220088700801926, -3.266794030753196}},
        {3377.9159999999997, {0., 0., 0.}},
        {4027.6536502546687, {-3.964310130780936, 0.08006285575503613, 5.313864037415808}},
        {6237.149579831932, {0., 0., 0.}},
        {6500.074702886248, {-0.8058588959846187, 1.6088833889257592, -2.2406369414514837}},
        {7616.651952461799, {0.15881288125106394, 0.9634983958585909, -3.792280054764281}},
        {9291.517826825127, {15.00876504273313, 1.948698699541007, 9.621351972786318}},
        {10328.339558573854, {8.746762102220163, -1.3252231011236748, 0.40990799919194243}},
        {11344.944, {3.107379091978582, 0.24758149132857085, -1.7704233405817775}},
        {13717.94906621392, {-14.868514648384936, -0.06864726970614742, -1.8512522029189045}},
        {14874.404074702887, {-2.1505985395333633, 0.0025699595738454374, -0.5800507110513734}},
        {15472.570458404074, {0.5463244285267206, -0.009449336194383946, -4.067785389892101}},
        {18862.179966044143, {0., 0., 0.}},
        {20576.923599320886, {-1.4763543428185393, 0.6215147468157091, 4.983670944496179}},
        {23208.855687606112, {0., 0., 0.}}
    };

    ::fwMedData::NavigationSeries::ContainerType lookAt =
    {
        {0., {216.48659596562175, -109.43725495319805, 0.}},
        {4027.6536502546687, {206.6271751068543, -90.3143606602177, -7.6435594203111314}},
        {6500.074702886248, {206.6271751068543, -90.3143606602177, -7.6435594203111314}}
    };

    for(const auto& elt : positions)
    {
        obj->addPosition(elt.first, elt.second);
    }
    for(const auto& elt : move)
    {
        obj->addMove(elt.first, elt.second);
    }
    for(const auto& elt : lookAt)
    {
        obj->addLookAt(elt.first, elt.second);
    }

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@position.0",
                                                 "-514.16065130194306;-94.681167957987583;979.37850551746283;");
    ::DataCampHelper::compareSimplePropertyValue(obj, "@move.0",
                                                 "0.89782063395046396;0.13607544320686907;-1.2769419285130967;");
    ::DataCampHelper::compareSimplePropertyValue(obj, "@look_at.0", "216.48659596562175;-109.43725495319805;0;");
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
