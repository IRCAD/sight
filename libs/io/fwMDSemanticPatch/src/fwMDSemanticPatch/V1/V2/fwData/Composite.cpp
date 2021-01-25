/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwMDSemanticPatch/V1/V2/data/Composite.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/memory/BufferObject.hpp>
#include <core/tools/UUID.hpp>

#include <data/ActivitySeries.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace sight::data
{

typedef std::map< atoms::Object::sptr, atoms::Object::sptr > Image2ModelType;

Composite::Composite() :
    ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::sight::data::Composite";
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
    const atoms::Map::sptr& oldCompositeMap,
    const atoms::Sequence::sptr& series,
    const Image2ModelType& image2Model,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    atoms::Object::sptr oldPlanningDB = atoms::Object::dynamicCast( (*oldCompositeMap)["planningDB"] );
    atoms::Map::sptr oldPlannings     = oldPlanningDB->getAttribute< atoms::Map >("values");

    for( atoms::Map::value_type oldPlanningAtom :  oldPlannings->getValue() )
    {
        atoms::Map::sptr oldPlanning
            = atoms::Object::dynamicCast(oldPlanningAtom.second)->getAttribute< atoms::Map >("values");

        SLM_ASSERT("Didn't find 'acquisition' in planning",
                   oldPlanning->getValue().find("acquisition") != oldPlanning->getValue().end());
        atoms::Base::sptr acquisition = oldPlanning->getValue().find("acquisition")->second;
        atoms::Object::sptr acqObj    = atoms::Object::dynamicCast(acquisition);
        SLM_ASSERT("Failed to cast acquisition to object", acqObj);

        Image2ModelType::const_iterator it = image2Model.find(newVersions[acqObj]);
        SLM_ASSERT("Didn't find image series related to acquisition", it != image2Model.end());
        atoms::Object::sptr imageSeries = it->first;

        atoms::Object::sptr resectionDB
            = atoms::Object::dynamicCast(oldPlanning->getValue().find("resectionDB")->second);

        // Retrieves expert who performed the resection
        atoms::Object::sptr expert
            = atoms::Object::dynamicCast(oldPlanning->getValue().find("expert")->second);

        atoms::Sequence::sptr experts = atoms::Sequence::New();
        experts->push_back( atoms::String::dynamicCast(expert->getAttribute< atoms::String >("value")));

        // Retrieves resection information
        atoms::Object::sptr information
            = atoms::Object::dynamicCast(oldPlanning->getValue().find("information")->second);
        atoms::String::sptr informationStr
            = atoms::String::dynamicCast(information->getAttribute< atoms::String >("value"));

        // Retrieves resection date and time
        atoms::Object::sptr dateTimeObj
            = atoms::Object::dynamicCast(oldPlanning->getValue().find("startDateTime")->second);
        atoms::String::sptr dateTimeStr
            = atoms::String::dynamicCast(dateTimeObj->getAttribute< atoms::String >("value"));

        atoms::String::sptr time = atoms::String::New("");
        atoms::String::sptr date = atoms::String::New("");

        std::string dateTimeStd = dateTimeStr->getString();
        std::vector< std::string > strs;
        ::boost::split(strs, dateTimeStd, ::boost::is_any_of(" "));

        if(strs.size() >= 2)
        {
            date->setValue(strs[0]);
            time->setValue(strs[1]);
        }

        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
        atoms::Object::sptr newActivitySeries = creators->create( "::sight::data::ActivitySeries", "1");

        atoms::Object::sptr imgComposite = atoms::Object::New();
        ::fwAtomsPatch::helper::setClassname(imgComposite, "::sight::data::Composite");
        ::fwAtomsPatch::helper::setVersion(imgComposite, "1");
        ::fwAtomsPatch::helper::generateID(imgComposite);
        ::fwAtomsPatch::helper::cleanFields(imgComposite);

        ::fwAtomsPatch::helper::Object imgCompositeHelper(imgComposite);
        atoms::Map::sptr compositeMap = atoms::Map::New();
        compositeMap->insert("OptionalInputImageKey", imageSeries->getAttribute< atoms::Object >("image"));
        imgCompositeHelper.addAttribute("values", compositeMap);

        atoms::Map::sptr activityDataMap = atoms::Map::New();
        activityDataMap->insert("resectionDB", resectionDB);
        activityDataMap->insert("modelSeries", it->second);
        activityDataMap->insert("imageSeries", imgComposite);

        atoms::Object::sptr mapObj = atoms::Object::New();
        ::fwAtomsPatch::helper::setClassname(mapObj, "::sight::data::Composite");
        ::fwAtomsPatch::helper::setVersion(mapObj, "1");
        ::fwAtomsPatch::helper::generateID(mapObj);
        ::fwAtomsPatch::helper::cleanFields(mapObj);

        ::fwAtomsPatch::helper::Object mapObjHelper(mapObj);
        mapObjHelper.addAttribute("values", activityDataMap);

        ::fwAtomsPatch::helper::Object helperActivity(newActivitySeries);
        helperActivity.replaceAttribute("activity_config_id", atoms::String::New("Resection"));
        helperActivity.replaceAttribute("data", mapObj);

        helperActivity.replaceAttribute("modality", atoms::String::New("OT") );
        helperActivity.replaceAttribute("instance_uid", atoms::String::New(core::tools::UUID::generateUUID()));
        helperActivity.replaceAttribute("date", date);
        helperActivity.replaceAttribute("time", time);
        helperActivity.replaceAttribute("performing_physicians_name", experts);
        helperActivity.replaceAttribute("description", informationStr);
        helperActivity.replaceAttribute("patient", imageSeries->getAttribute< atoms::Object >("patient"));
        helperActivity.replaceAttribute("study", imageSeries->getAttribute< atoms::Object >("study"));
        helperActivity.replaceAttribute("equipment", imageSeries->getAttribute< atoms::Object >("equipment"));

        series->push_back(newActivitySeries);
    }
}

// ----------------------------------------------------------------------------

// Convert ProcessingDB Composite into ActivitySeries
void processProcessing(
    const atoms::Map::sptr& oldCompositeMap,
    const atoms::Sequence::sptr& series,
    const Image2ModelType& image2Model,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    atoms::Object::sptr oldProcessingDB = atoms::Object::dynamicCast( (*oldCompositeMap)["processingDB"] );

    atoms::Map::sptr oldProcessings = oldProcessingDB->getAttribute< atoms::Map >("values");

    for( atoms::Map::value_type oldProcessingAtom :  oldProcessings->getValue() )
    {
        atoms::Map::sptr oldProcessing
            = atoms::Object::dynamicCast(oldProcessingAtom.second)->getAttribute< atoms::Map >("values");

        // Retrieves expert who performed the resection
        atoms::Object::sptr expert
            = atoms::Object::dynamicCast(oldProcessing->getValue().find("expert")->second);

        atoms::Sequence::sptr experts = atoms::Sequence::New();
        experts->push_back( atoms::String::dynamicCast(expert->getAttribute< atoms::String >("value")));

        // Retrieves resection information
        atoms::Object::sptr information
            = atoms::Object::dynamicCast(oldProcessing->getValue().find("information")->second);
        atoms::String::sptr informationStr
            = atoms::String::dynamicCast(information->getAttribute< atoms::String >("value"));

        // Retrieves resection date and time
        atoms::Object::sptr dateTimeObj
            = atoms::Object::dynamicCast(oldProcessing->getValue().find("startDateTime")->second);
        atoms::String::sptr dateTimeStr
            = atoms::String::dynamicCast(dateTimeObj->getAttribute< atoms::String >("value"));

        atoms::String::sptr time = atoms::String::New("");
        atoms::String::sptr date = atoms::String::New("");

        std::string dateTimeStd = dateTimeStr->getString();
        std::vector< std::string > strs;
        ::boost::split(strs, dateTimeStd, ::boost::is_any_of(" "));

        if(strs.size() >= 2)
        {
            date->setValue(strs[0]);
            time->setValue(strs[1]);
        }

        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
        atoms::Object::sptr newActivitySeries = creators->create( "::sight::data::ActivitySeries", "1");

        atoms::Map::sptr activityDataMap = atoms::Map::New();
        activityDataMap->insert("processing", oldProcessingAtom.second);

        atoms::Object::sptr mapObj = atoms::Object::New();
        ::fwAtomsPatch::helper::setClassname(mapObj, "::sight::data::Composite");
        ::fwAtomsPatch::helper::setVersion(mapObj, "1");
        ::fwAtomsPatch::helper::generateID(mapObj);
        ::fwAtomsPatch::helper::cleanFields(mapObj);

        ::fwAtomsPatch::helper::Object mapObjHelper(mapObj);
        mapObjHelper.addAttribute("values", activityDataMap);

        ::fwAtomsPatch::helper::Object helperActivity(newActivitySeries);
        helperActivity.replaceAttribute("activity_config_id", atoms::String::New("Processing"));
        helperActivity.replaceAttribute("data", mapObj);

        helperActivity.replaceAttribute("modality", atoms::String::New("OT") );
        helperActivity.replaceAttribute("instance_uid", atoms::String::New(core::tools::UUID::generateUUID()));
        helperActivity.replaceAttribute("date", date);
        helperActivity.replaceAttribute("time", time);
        helperActivity.replaceAttribute("performing_physicians_name", experts);
        helperActivity.replaceAttribute("description", informationStr);

        // Check if the processing is associated to an acquisition
        atoms::Base::sptr acquisitionSelection = oldProcessing->getValue().find("acquisitionSelection")->second;
        atoms::Object::sptr acqSelectionObj    = atoms::Object::dynamicCast(acquisitionSelection);
        SLM_ASSERT("Failed to cast acquisition composite to object", acqSelectionObj);

        atoms::Map::sptr acqSelectionMap = acqSelectionObj->getAttribute< atoms::Map >("values");

        atoms::Map::const_iterator iter = acqSelectionMap->getValue().find("selectedAcquisition");
        if (iter != acqSelectionMap->getValue().end() )
        {
            // get associated image
            atoms::Base::sptr acquisition = iter->second;
            atoms::Object::sptr acqObj    = atoms::Object::dynamicCast(acquisition);
            SLM_ASSERT("Failed to cast acquisition to object", acqObj);

            Image2ModelType::const_iterator it = image2Model.find(newVersions[acqObj]);
            if(it != image2Model.end())
            {
                atoms::Object::sptr imageSeries = it->first;
                helperActivity.replaceAttribute("patient", imageSeries->getAttribute< atoms::Object >("patient"));
                helperActivity.replaceAttribute("study", imageSeries->getAttribute< atoms::Object >("study"));
                helperActivity.replaceAttribute("equipment",
                                                imageSeries->getAttribute< atoms::Object >("equipment"));
            }
        }

        series->push_back(newActivitySeries);
    }

}

// ----------------------------------------------------------------------------

void Composite::apply(
    const atoms::Object::sptr& previous,
    const atoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    Image2ModelType image2Model;

    ISemanticPatch::apply(previous, current, newVersions);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    // If data::Composite is a MedicalWorkspace
    if ( previous->getMetaInfo("compositeType") == "MedicalWorkspace" )
    {
        // Update type/version
        ::fwAtomsPatch::helper::setClassname( current, "::sight::data::SeriesDB" );
        ::fwAtomsPatch::helper::setVersion( current, "1" );

        // Update Attributes
        ::fwAtomsPatch::helper::cleanFields( current );
        // Replace atoms::Map by atoms::Sequence ( have the same key )
        atoms::Sequence::sptr series = atoms::Sequence::New();
        helper.replaceAttribute( "values", series );

        // Add ImageSeries and create ModelSeries if they are necessary
        atoms::Map::sptr oldCompositeMap = previous->getAttribute< atoms::Map >("values");
        atoms::Object::sptr oldPatientDB =
            atoms::Object::dynamicCast( (*oldCompositeMap)["patientDB"] );
        atoms::Sequence::sptr oldPatients = oldPatientDB->getAttribute< atoms::Sequence >("patients");

        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();

        for( atoms::Base::sptr oldPatientAtom :  oldPatients->getValue() )
        {
            atoms::Object::sptr oldPatient = atoms::Object::dynamicCast( oldPatientAtom );
            atoms::Object::sptr newPatient = newVersions[oldPatient];

            atoms::Sequence::sptr oldStudies = oldPatient->getAttribute< atoms::Sequence >("studies");

            for( atoms::Base::sptr oldStudyAtom :  oldStudies->getValue() )
            {
                atoms::Object::sptr oldStudy = atoms::Object::dynamicCast( oldStudyAtom );
                atoms::Object::sptr newStudy = newVersions[oldStudy];

                atoms::Sequence::sptr oldAcquisitions =
                    oldStudy->getAttribute< atoms::Sequence >("acquisitions");

                for( atoms::Base::sptr oldAcqAtom :  oldAcquisitions->getValue() )
                {
                    atoms::Object::sptr oldAcq = atoms::Object::dynamicCast( oldAcqAtom );

                    // finalize and push newImgSeries
                    atoms::Object::sptr newImgSeries = newVersions[oldAcq];
                    ::fwAtomsPatch::helper::Object imgSeriesHelper(newImgSeries);
                    atoms::Object::sptr clonedPatient = atoms::Object::dynamicCast(newPatient->clone());
                    ::fwAtomsPatch::helper::changeUID(clonedPatient);

                    imgSeriesHelper.replaceAttribute("patient", clonedPatient );
                    atoms::Object::sptr clonedStudy = atoms::Object::dynamicCast(newStudy->clone());
                    ::fwAtomsPatch::helper::changeUID(clonedStudy);

                    imgSeriesHelper.replaceAttribute("study", clonedStudy );
                    imgSeriesHelper.replaceAttribute("modality", oldStudy->getAttribute("modality")->clone() );

                    atoms::String::sptr institution;
                    institution =
                        atoms::String::dynamicCast( oldStudy->getAttribute("hospital")->clone() );
                    atoms::Object::sptr equipment = newImgSeries->getAttribute< atoms::Object >("equipment");
                    ::fwAtomsPatch::helper::Object equipmentHelper(equipment);
                    equipmentHelper.replaceAttribute("institution_name", institution);

                    // Set study date and time if necessary
                    atoms::String::sptr studyDate = newStudy->getAttribute< atoms::String >("date");
                    ::fwAtomsPatch::helper::Object studyHelper(clonedStudy);

                    if ( studyDate->getValue().empty() )
                    {
                        studyHelper.replaceAttribute("date", newImgSeries->getAttribute("date")->clone());
                    }

                    atoms::String::sptr studyTime = newStudy->getAttribute< atoms::String >("time");
                    if ( studyTime->getValue().empty() )
                    {
                        studyHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone());
                    }

                    series->push_back( newImgSeries );

                    // finalize and push newModelSeries
                    atoms::Sequence::sptr oldReconstructions =
                        oldAcq->getAttribute< atoms::Sequence >("reconstructions");

                    if ( oldReconstructions->size() > 0 )
                    {
                        // Create new model series
                        atoms::Object::sptr newModelSeries =
                            creators->create( "::sight::data::ModelSeries", "1");

                        ::fwAtomsPatch::helper::Object msHelper(newModelSeries);
                        atoms::Object::sptr msPatient = atoms::Object::dynamicCast(clonedPatient->clone());
                        ::fwAtomsPatch::helper::changeUID(msPatient);
                        msHelper.replaceAttribute("patient", msPatient );

                        atoms::Object::sptr msStudy = atoms::Object::dynamicCast(clonedStudy->clone());
                        ::fwAtomsPatch::helper::changeUID(msStudy);
                        msHelper.replaceAttribute("study", msStudy );

                        atoms::Object::sptr msEquipment = atoms::Object::dynamicCast(equipment->clone());
                        ::fwAtomsPatch::helper::changeUID(msEquipment);
                        msHelper.replaceAttribute("equipment", msEquipment );

                        msHelper.replaceAttribute("modality", atoms::String::New("OT") );
                        msHelper.replaceAttribute("instance_uid",
                                                  atoms::String::New(core::tools::UUID::generateUUID()) );
                        msHelper.replaceAttribute("date", newImgSeries->getAttribute("date")->clone() );
                        msHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone() );
                        msHelper.replaceAttribute("performing_physicians_name",
                                                  newImgSeries->getAttribute("performing_physicians_name")->clone() );
                        msHelper.replaceAttribute("description", newImgSeries->getAttribute("description")->clone() );

                        atoms::Sequence::sptr newReconstructions =
                            newModelSeries->getAttribute< atoms::Sequence >("reconstruction_db");
                        for( atoms::Base::sptr oldRecAtom :  oldReconstructions->getValue() )
                        {
                            atoms::Object::sptr oldRec = atoms::Object::dynamicCast( oldRecAtom );
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

} // namespace sight::data
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch
