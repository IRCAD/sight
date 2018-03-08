/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMDSemanticPatch/V1/V2/fwData/Composite.hpp"

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/UUID.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

typedef std::map< ::fwAtoms::Object::sptr, ::fwAtoms::Object::sptr > Image2ModelType;

Composite::Composite() :
    ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Composite";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Composite::~Composite()
{
}

// ----------------------------------------------------------------------------

Composite::Composite( const Composite& cpy ) :
    ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void processPlanning(
    const ::fwAtoms::Map::sptr& oldCompositeMap,
    const ::fwAtoms::Sequence::sptr& series,
    const Image2ModelType& image2Model,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ::fwAtoms::Object::sptr oldPlanningDB = ::fwAtoms::Object::dynamicCast( (*oldCompositeMap)["planningDB"] );
    ::fwAtoms::Map::sptr oldPlannings     = oldPlanningDB->getAttribute< ::fwAtoms::Map >("values");

    for( ::fwAtoms::Map::value_type oldPlanningAtom :  oldPlannings->getValue() )
    {
        ::fwAtoms::Map::sptr oldPlanning
            = ::fwAtoms::Object::dynamicCast(oldPlanningAtom.second)->getAttribute< ::fwAtoms::Map >("values");

        SLM_ASSERT("Didn't find 'acquisition' in planning",
                   oldPlanning->getValue().find("acquisition") != oldPlanning->getValue().end());
        ::fwAtoms::Base::sptr acquisition = oldPlanning->getValue().find("acquisition")->second;
        ::fwAtoms::Object::sptr acqObj    = ::fwAtoms::Object::dynamicCast(acquisition);
        SLM_ASSERT("Failed to cast acquisition to object", acqObj);

        Image2ModelType::const_iterator it = image2Model.find(newVersions[acqObj]);
        SLM_ASSERT("Didn't find image series related to acquisition", it != image2Model.end());
        ::fwAtoms::Object::sptr imageSeries = it->first;

        ::fwAtoms::Object::sptr resectionDB
            = ::fwAtoms::Object::dynamicCast(oldPlanning->getValue().find("resectionDB")->second);

        // Retrieves expert who performed the resection
        ::fwAtoms::Object::sptr expert
            = ::fwAtoms::Object::dynamicCast(oldPlanning->getValue().find("expert")->second);

        ::fwAtoms::Sequence::sptr experts = ::fwAtoms::Sequence::New();
        experts->push_back( ::fwAtoms::String::dynamicCast(expert->getAttribute< ::fwAtoms::String >("value")));

        // Retrieves resection information
        ::fwAtoms::Object::sptr information
            = ::fwAtoms::Object::dynamicCast(oldPlanning->getValue().find("information")->second);
        ::fwAtoms::String::sptr informationStr
            = ::fwAtoms::String::dynamicCast(information->getAttribute< ::fwAtoms::String >("value"));

        // Retrieves resection date and time
        ::fwAtoms::Object::sptr dateTimeObj
            = ::fwAtoms::Object::dynamicCast(oldPlanning->getValue().find("startDateTime")->second);
        ::fwAtoms::String::sptr dateTimeStr
            = ::fwAtoms::String::dynamicCast(dateTimeObj->getAttribute< ::fwAtoms::String >("value"));

        ::fwAtoms::String::sptr time = ::fwAtoms::String::New("");
        ::fwAtoms::String::sptr date = ::fwAtoms::String::New("");

        std::string dateTimeStd = dateTimeStr->getString();
        std::vector< std::string > strs;
        ::boost::split(strs, dateTimeStd, ::boost::is_any_of(" "));

        if(strs.size() >= 2)
        {
            date->setValue(strs[0]);
            time->setValue(strs[1]);
        }

        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
        ::fwAtoms::Object::sptr newActivitySeries          = creators->create( "::fwMedData::ActivitySeries", "1");

        ::fwAtoms::Object::sptr imgComposite = ::fwAtoms::Object::New();
        ::fwAtomsPatch::helper::setClassname(imgComposite, "::fwData::Composite");
        ::fwAtomsPatch::helper::setVersion(imgComposite, "1");
        ::fwAtomsPatch::helper::generateID(imgComposite);
        ::fwAtomsPatch::helper::cleanFields(imgComposite);

        ::fwAtomsPatch::helper::Object imgCompositeHelper(imgComposite);
        ::fwAtoms::Map::sptr compositeMap = ::fwAtoms::Map::New();
        compositeMap->insert("OptionalInputImageKey", imageSeries->getAttribute< ::fwAtoms::Object >("image"));
        imgCompositeHelper.addAttribute("values", compositeMap);

        ::fwAtoms::Map::sptr activityDataMap = ::fwAtoms::Map::New();
        activityDataMap->insert("resectionDB", resectionDB);
        activityDataMap->insert("modelSeries", it->second);
        activityDataMap->insert("imageSeries", imgComposite);

        ::fwAtoms::Object::sptr mapObj = ::fwAtoms::Object::New();
        ::fwAtomsPatch::helper::setClassname(mapObj, "::fwData::Composite");
        ::fwAtomsPatch::helper::setVersion(mapObj, "1");
        ::fwAtomsPatch::helper::generateID(mapObj);
        ::fwAtomsPatch::helper::cleanFields(mapObj);

        ::fwAtomsPatch::helper::Object mapObjHelper(mapObj);
        mapObjHelper.addAttribute("values", activityDataMap);

        ::fwAtomsPatch::helper::Object helperActivity(newActivitySeries);
        helperActivity.replaceAttribute("activity_config_id", ::fwAtoms::String::New("Resection"));
        helperActivity.replaceAttribute("data", mapObj);

        helperActivity.replaceAttribute("modality", ::fwAtoms::String::New("OT") );
        helperActivity.replaceAttribute("instance_uid", ::fwAtoms::String::New(::fwTools::UUID::generateUUID()));
        helperActivity.replaceAttribute("date", date);
        helperActivity.replaceAttribute("time", time);
        helperActivity.replaceAttribute("performing_physicians_name", experts);
        helperActivity.replaceAttribute("description", informationStr);
        helperActivity.replaceAttribute("patient", imageSeries->getAttribute< ::fwAtoms::Object >("patient"));
        helperActivity.replaceAttribute("study", imageSeries->getAttribute< ::fwAtoms::Object >("study"));
        helperActivity.replaceAttribute("equipment", imageSeries->getAttribute< ::fwAtoms::Object >("equipment"));

        series->push_back(newActivitySeries);
    }
}

// ----------------------------------------------------------------------------

// Convert ProcessingDB Composite into ActivitySeries
void processProcessing(
    const ::fwAtoms::Map::sptr& oldCompositeMap,
    const ::fwAtoms::Sequence::sptr& series,
    const Image2ModelType& image2Model,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ::fwAtoms::Object::sptr oldProcessingDB = ::fwAtoms::Object::dynamicCast( (*oldCompositeMap)["processingDB"] );

    ::fwAtoms::Map::sptr oldProcessings = oldProcessingDB->getAttribute< ::fwAtoms::Map >("values");

    for( ::fwAtoms::Map::value_type oldProcessingAtom :  oldProcessings->getValue() )
    {
        ::fwAtoms::Map::sptr oldProcessing
            = ::fwAtoms::Object::dynamicCast(oldProcessingAtom.second)->getAttribute< ::fwAtoms::Map >("values");

        // Retrieves expert who performed the resection
        ::fwAtoms::Object::sptr expert
            = ::fwAtoms::Object::dynamicCast(oldProcessing->getValue().find("expert")->second);

        ::fwAtoms::Sequence::sptr experts = ::fwAtoms::Sequence::New();
        experts->push_back( ::fwAtoms::String::dynamicCast(expert->getAttribute< ::fwAtoms::String >("value")));

        // Retrieves resection information
        ::fwAtoms::Object::sptr information
            = ::fwAtoms::Object::dynamicCast(oldProcessing->getValue().find("information")->second);
        ::fwAtoms::String::sptr informationStr
            = ::fwAtoms::String::dynamicCast(information->getAttribute< ::fwAtoms::String >("value"));

        // Retrieves resection date and time
        ::fwAtoms::Object::sptr dateTimeObj
            = ::fwAtoms::Object::dynamicCast(oldProcessing->getValue().find("startDateTime")->second);
        ::fwAtoms::String::sptr dateTimeStr
            = ::fwAtoms::String::dynamicCast(dateTimeObj->getAttribute< ::fwAtoms::String >("value"));

        ::fwAtoms::String::sptr time = ::fwAtoms::String::New("");
        ::fwAtoms::String::sptr date = ::fwAtoms::String::New("");

        std::string dateTimeStd = dateTimeStr->getString();
        std::vector< std::string > strs;
        ::boost::split(strs, dateTimeStd, ::boost::is_any_of(" "));

        if(strs.size() >= 2)
        {
            date->setValue(strs[0]);
            time->setValue(strs[1]);
        }

        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
        ::fwAtoms::Object::sptr newActivitySeries          = creators->create( "::fwMedData::ActivitySeries", "1");

        ::fwAtoms::Map::sptr activityDataMap = ::fwAtoms::Map::New();
        activityDataMap->insert("processing", oldProcessingAtom.second);

        ::fwAtoms::Object::sptr mapObj = ::fwAtoms::Object::New();
        ::fwAtomsPatch::helper::setClassname(mapObj, "::fwData::Composite");
        ::fwAtomsPatch::helper::setVersion(mapObj, "1");
        ::fwAtomsPatch::helper::generateID(mapObj);
        ::fwAtomsPatch::helper::cleanFields(mapObj);

        ::fwAtomsPatch::helper::Object mapObjHelper(mapObj);
        mapObjHelper.addAttribute("values", activityDataMap);

        ::fwAtomsPatch::helper::Object helperActivity(newActivitySeries);
        helperActivity.replaceAttribute("activity_config_id", ::fwAtoms::String::New("Processing"));
        helperActivity.replaceAttribute("data", mapObj);

        helperActivity.replaceAttribute("modality", ::fwAtoms::String::New("OT") );
        helperActivity.replaceAttribute("instance_uid", ::fwAtoms::String::New(::fwTools::UUID::generateUUID()));
        helperActivity.replaceAttribute("date", date);
        helperActivity.replaceAttribute("time", time);
        helperActivity.replaceAttribute("performing_physicians_name", experts);
        helperActivity.replaceAttribute("description", informationStr);

        // Check if the processing is associated to an acquisition
        ::fwAtoms::Base::sptr acquisitionSelection = oldProcessing->getValue().find("acquisitionSelection")->second;
        ::fwAtoms::Object::sptr acqSelectionObj    = ::fwAtoms::Object::dynamicCast(acquisitionSelection);
        SLM_ASSERT("Failed to cast acquisition composite to object", acqSelectionObj);

        ::fwAtoms::Map::sptr acqSelectionMap = acqSelectionObj->getAttribute< ::fwAtoms::Map >("values");

        ::fwAtoms::Map::const_iterator iter = acqSelectionMap->getValue().find("selectedAcquisition");
        if (iter != acqSelectionMap->getValue().end() )
        {
            // get associated image
            ::fwAtoms::Base::sptr acquisition = iter->second;
            ::fwAtoms::Object::sptr acqObj    = ::fwAtoms::Object::dynamicCast(acquisition);
            SLM_ASSERT("Failed to cast acquisition to object", acqObj);

            Image2ModelType::const_iterator it = image2Model.find(newVersions[acqObj]);
            if(it != image2Model.end())
            {
                ::fwAtoms::Object::sptr imageSeries = it->first;
                helperActivity.replaceAttribute("patient", imageSeries->getAttribute< ::fwAtoms::Object >("patient"));
                helperActivity.replaceAttribute("study", imageSeries->getAttribute< ::fwAtoms::Object >("study"));
                helperActivity.replaceAttribute("equipment",
                                                imageSeries->getAttribute< ::fwAtoms::Object >("equipment"));
            }
        }

        series->push_back(newActivitySeries);
    }

}

// ----------------------------------------------------------------------------

void Composite::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    Image2ModelType image2Model;

    ISemanticPatch::apply(previous, current, newVersions);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    // If fwData::Composite is a MedicalWorkspace
    if ( previous->getMetaInfo("compositeType") == "MedicalWorkspace" )
    {
        // Update type/version
        ::fwAtomsPatch::helper::setClassname( current, "::fwMedData::SeriesDB" );
        ::fwAtomsPatch::helper::setVersion( current, "1" );

        // Update Attributes
        ::fwAtomsPatch::helper::cleanFields( current );
        // Replace fwAtoms::Map by fwAtoms::Sequence ( have the same key )
        ::fwAtoms::Sequence::sptr series = ::fwAtoms::Sequence::New();
        helper.replaceAttribute( "values", series );

        // Add ImageSeries and create ModelSeries if they are necessary
        ::fwAtoms::Map::sptr oldCompositeMap = previous->getAttribute< ::fwAtoms::Map >("values");
        ::fwAtoms::Object::sptr oldPatientDB =
            ::fwAtoms::Object::dynamicCast( (*oldCompositeMap)["patientDB"] );
        ::fwAtoms::Sequence::sptr oldPatients = oldPatientDB->getAttribute< ::fwAtoms::Sequence >("patients");

        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();

        for( ::fwAtoms::Base::sptr oldPatientAtom :  oldPatients->getValue() )
        {
            ::fwAtoms::Object::sptr oldPatient = ::fwAtoms::Object::dynamicCast( oldPatientAtom );
            ::fwAtoms::Object::sptr newPatient = newVersions[oldPatient];

            ::fwAtoms::Sequence::sptr oldStudies = oldPatient->getAttribute< ::fwAtoms::Sequence >("studies");

            for( ::fwAtoms::Base::sptr oldStudyAtom :  oldStudies->getValue() )
            {
                ::fwAtoms::Object::sptr oldStudy = ::fwAtoms::Object::dynamicCast( oldStudyAtom );
                ::fwAtoms::Object::sptr newStudy = newVersions[oldStudy];

                ::fwAtoms::Sequence::sptr oldAcquisitions =
                    oldStudy->getAttribute< ::fwAtoms::Sequence >("acquisitions");

                for( ::fwAtoms::Base::sptr oldAcqAtom :  oldAcquisitions->getValue() )
                {
                    ::fwAtoms::Object::sptr oldAcq = ::fwAtoms::Object::dynamicCast( oldAcqAtom );

                    // finalize and push newImgSeries
                    ::fwAtoms::Object::sptr newImgSeries = newVersions[oldAcq];
                    ::fwAtomsPatch::helper::Object imgSeriesHelper(newImgSeries);
                    ::fwAtoms::Object::sptr clonedPatient = ::fwAtoms::Object::dynamicCast(newPatient->clone());
                    ::fwAtomsPatch::helper::changeUID(clonedPatient);

                    imgSeriesHelper.replaceAttribute("patient", clonedPatient );
                    ::fwAtoms::Object::sptr clonedStudy = ::fwAtoms::Object::dynamicCast(newStudy->clone());
                    ::fwAtomsPatch::helper::changeUID(clonedStudy);

                    imgSeriesHelper.replaceAttribute("study", clonedStudy );
                    imgSeriesHelper.replaceAttribute("modality", oldStudy->getAttribute("modality")->clone() );

                    ::fwAtoms::String::sptr institution;
                    institution =
                        ::fwAtoms::String::dynamicCast( oldStudy->getAttribute("hospital")->clone() );
                    ::fwAtoms::Object::sptr equipment = newImgSeries->getAttribute< ::fwAtoms::Object >("equipment");
                    ::fwAtomsPatch::helper::Object equipmentHelper(equipment);
                    equipmentHelper.replaceAttribute("institution_name", institution);

                    // Set study date and time if necessary
                    ::fwAtoms::String::sptr studyDate = newStudy->getAttribute< ::fwAtoms::String >("date");
                    ::fwAtomsPatch::helper::Object studyHelper(clonedStudy);

                    if ( studyDate->getValue().empty() )
                    {
                        studyHelper.replaceAttribute("date", newImgSeries->getAttribute("date")->clone());
                    }

                    ::fwAtoms::String::sptr studyTime = newStudy->getAttribute< ::fwAtoms::String >("time");
                    if ( studyTime->getValue().empty() )
                    {
                        studyHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone());
                    }

                    series->push_back( newImgSeries );

                    // finalize and push newModelSeries
                    ::fwAtoms::Sequence::sptr oldReconstructions =
                        oldAcq->getAttribute< ::fwAtoms::Sequence >("reconstructions");

                    if ( oldReconstructions->size() > 0 )
                    {
                        // Create new model series
                        ::fwAtoms::Object::sptr newModelSeries =
                            creators->create( "::fwMedData::ModelSeries", "1");

                        ::fwAtomsPatch::helper::Object msHelper(newModelSeries);
                        ::fwAtoms::Object::sptr msPatient = ::fwAtoms::Object::dynamicCast(clonedPatient->clone());
                        ::fwAtomsPatch::helper::changeUID(msPatient);
                        msHelper.replaceAttribute("patient", msPatient );

                        ::fwAtoms::Object::sptr msStudy = ::fwAtoms::Object::dynamicCast(clonedStudy->clone());
                        ::fwAtomsPatch::helper::changeUID(msStudy);
                        msHelper.replaceAttribute("study", msStudy );

                        ::fwAtoms::Object::sptr msEquipment = ::fwAtoms::Object::dynamicCast(equipment->clone());
                        ::fwAtomsPatch::helper::changeUID(msEquipment);
                        msHelper.replaceAttribute("equipment", msEquipment );

                        msHelper.replaceAttribute("modality", ::fwAtoms::String::New("OT") );
                        msHelper.replaceAttribute("instance_uid",
                                                  ::fwAtoms::String::New(::fwTools::UUID::generateUUID()) );
                        msHelper.replaceAttribute("date", newImgSeries->getAttribute("date")->clone() );
                        msHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone() );
                        msHelper.replaceAttribute("performing_physicians_name",
                                                  newImgSeries->getAttribute("performing_physicians_name")->clone() );
                        msHelper.replaceAttribute("description", newImgSeries->getAttribute("description")->clone() );

                        ::fwAtoms::Sequence::sptr newReconstructions =
                            newModelSeries->getAttribute< ::fwAtoms::Sequence >("reconstruction_db");
                        for( ::fwAtoms::Base::sptr oldRecAtom :  oldReconstructions->getValue() )
                        {
                            ::fwAtoms::Object::sptr oldRec = ::fwAtoms::Object::dynamicCast( oldRecAtom );
                            newReconstructions->push_back(newVersions[oldRec]);
                        }

                        image2Model[newImgSeries] = newModelSeries;
                        series->push_back( newModelSeries );
                    }
                }
            }
        }

        processPlanning(oldCompositeMap, series, image2Model, newVersions);
        processProcessing(oldCompositeMap, series, image2Model, newVersions);

    } // End "MedicalWorkspace"
}

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch
