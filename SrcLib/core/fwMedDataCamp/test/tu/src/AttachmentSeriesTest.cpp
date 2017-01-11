/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "AttachmentSeriesTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwMedData/AttachmentSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::AttachmentSeriesTest );

namespace fwMedDataCamp
{
namespace ut
{

void AttachmentSeriesTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwMedDataCamp
    const int version = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

void AttachmentSeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void AttachmentSeriesTest::propertiesTest()
{
    const std::string instance_uid                            = "123456789";
    const std::string modality                                = "CT";
    const std::string date                                    = "20130214";
    const std::string time                                    = "143328";
    const std::string performing_physicians_name              = "John Doe";
    const std::string description                             = "description";
    const std::string mediaType                               = "text";
    const std::string filename                                = "attach.ext";
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
                                                                 {"attachment_availability"},
                                                                 {"attachment_path"},
                                                                 {"media_type"},
                                                                 {"buffer"}};

    ::fwMedData::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);

    ::fwMedData::AttachmentSeries::sptr obj = ::fwMedData::AttachmentSeries::New();
    obj->setInstanceUID(instance_uid);
    obj->setModality(modality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setDescription(description);

    const ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "attachmenttest";
    ::boost::filesystem::create_directories(path);
    const ::boost::filesystem::path attachmentFile = path / filename;

    {
        std::ofstream file;
        file.open(attachmentFile.string().c_str(), std::ofstream::out);
        file << "42";
        file.close();
    }
    obj->setAttachmentPath(attachmentFile);
    obj->setMediaType(mediaType);

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
    //TODO: Enum are not supported yet, so we cannot make unit tests on this part...
    //::DataCampHelper::compareSimplePropertyValue(obj, "@attachment_availability", "LOCAL");
    ::DataCampHelper::compareSimplePropertyValue(obj, "@attachment_path", attachmentFile.string());
    ::DataCampHelper::compareSimplePropertyValue(obj, "@media_type", mediaType);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
