/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "DicomSeriesCampTest.hpp"

#include "DataCampHelper.hpp"

#include <fwData/Array.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwTools/System.hpp>
#include <fwTools/Type.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::DicomSeriesCampTest );

namespace fwMedDataCamp
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomSeriesCampTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomSeriesCampTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomSeriesCampTest::propertiesTest()
{
    const std::string instance_uid               = "123456789";
    const std::string modality                   = "CT";
    const std::string date                       = "20130214";
    const std::string time                       = "143328";
    const std::string performing_physicians_name = "John Doe";
    const std::string description                = "description";
    const std::string filename                   = "dicom.dcm";
    //DicomSeries
    ::DataCampHelper::PropertiesNameType dataProperties = { { "fields" },
                                                            { "number_of_instances" },
                                                            { "dicom_container" },
                                                            { "patient" },
                                                            { "study" },
                                                            { "equipment" },
                                                            { "instance_uid" },
                                                            { "modality" },
                                                            { "date" },
                                                            { "time" },
                                                            { "performing_physicians_name" },
                                                            { "description" },
                                                            { "sop_class_uids" },
                                                            { "computed_tag_values" },
                                                            { "first_instance_number" }};

    ::fwMemory::BufferObject::sptr bufferObj = ::fwMemory::BufferObject::New();

    const std::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "DicomSeriesCampTest";
    std::filesystem::create_directories(path);
    const std::filesystem::path dicomFile = path / filename;
    {
        std::ofstream file;
        file.open(dicomFile.string().c_str(), std::ofstream::out);
        file << "42";
        file.close();
    }

    ::fwMedData::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);

    ::fwMedData::DicomSeries::sptr obj = ::fwMedData::DicomSeries::New();
    obj->setInstanceUID(instance_uid);
    obj->setModality(modality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setDescription(description);
    obj->setNumberOfInstances(100);
    obj->addDicomPath(42, dicomFile);
    obj->addBinary(1664, bufferObj);
    obj->addSOPClassUID("1.2.840.10008.5.1.4.1.1.2");
    obj->addComputedTagValue("(0020,0100)", "1664");
    obj->setFirstInstanceNumber(1);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());

    // Number of instances
    ::std::stringstream ss;
    ss << obj->getNumberOfInstances();
    ::DataCampHelper::compareSimplePropertyValue(obj, "@number_of_instances", ss.str());

    // Dicom binaries
    // TODO: BufferObject is not supported yet, so we cannot make unit tests on this part...
    //::DataCampHelper::compareObjectPropertyValue(obj, "@dicom_container.1664", obj->getDicomContainer().at(1664));

    // SOP class UID
    ::DataCampHelper::compareSimplePropertyValue(obj, "@sop_class_uids.0", "1.2.840.10008.5.1.4.1.1.2");
    // SOP class UID
    ::DataCampHelper::compareSimplePropertyValue(obj, "@sop_class_uids.0", "1.2.840.10008.5.1.4.1.1.2");

    // Computed tag values
    ::DataCampHelper::compareSimplePropertyValue(obj, "@computed_tag_values.(0020,0100)", "1664");

    // First instance number
    ::DataCampHelper::compareSimplePropertyValue(obj, "@first_instance_number", "1");
    // Computed tag values
    ::DataCampHelper::compareSimplePropertyValue(obj, "@computed_tag_values.(0020,0100)", "1664");

    // First instance number
    ::DataCampHelper::compareSimplePropertyValue(obj, "@first_instance_number", "1");
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
