/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/patch/semantic/V1/V2/data/Composite.hpp"

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

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/StructuralCreatorDB.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>

namespace sight::io::patch::semantic
{

namespace V1
{

namespace V2
{

namespace data
{

typedef std::map<sight::atoms::Object::sptr, sight::atoms::Object::sptr> Image2ModelType;

Composite::Composite() :
    io::atoms::patch::ISemanticPatch()
{
    m_originClassname = "sight::data::Composite";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Composite::~Composite()
{
}

// ----------------------------------------------------------------------------

Composite::Composite(const Composite& cpy) :
    io::atoms::patch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void processPlanning(
    const sight::atoms::Map::sptr& oldCompositeMap,
    const sight::atoms::Sequence::sptr& series,
    const Image2ModelType& image2Model,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    sight::atoms::Object::sptr oldPlanningDB = sight::atoms::Object::dynamicCast((*oldCompositeMap)["planningDB"]);
    sight::atoms::Map::sptr oldPlannings     = oldPlanningDB->getAttribute<sight::atoms::Map>("values");

    for(sight::atoms::Map::value_type oldPlanningAtom : oldPlannings->getValue())
    {
        sight::atoms::Map::sptr oldPlanning =
            sight::atoms::Object::dynamicCast(oldPlanningAtom.second)->getAttribute<sight::atoms::Map>("values");

        SIGHT_ASSERT(
            "Didn't find 'acquisition' in planning",
            oldPlanning->getValue().find("acquisition") != oldPlanning->getValue().end()
        );
        sight::atoms::Base::sptr acquisition = oldPlanning->getValue().find("acquisition")->second;
        sight::atoms::Object::sptr acqObj    = sight::atoms::Object::dynamicCast(acquisition);
        SIGHT_ASSERT("Failed to cast acquisition to object", acqObj);

        Image2ModelType::const_iterator it = image2Model.find(newVersions[acqObj]);
        SIGHT_ASSERT("Didn't find image series related to acquisition", it != image2Model.end());
        sight::atoms::Object::sptr imageSeries = it->first;

        sight::atoms::Object::sptr resectionDB =
            sight::atoms::Object::dynamicCast(oldPlanning->getValue().find("resectionDB")->second);

        // Retrieves expert who performed the resection
        sight::atoms::Object::sptr expert =
            sight::atoms::Object::dynamicCast(oldPlanning->getValue().find("expert")->second);

        sight::atoms::Sequence::sptr experts = sight::atoms::Sequence::New();
        experts->push_back(sight::atoms::String::dynamicCast(expert->getAttribute<sight::atoms::String>("value")));

        // Retrieves resection information
        sight::atoms::Object::sptr information =
            sight::atoms::Object::dynamicCast(oldPlanning->getValue().find("information")->second);
        sight::atoms::String::sptr informationStr =
            sight::atoms::String::dynamicCast(information->getAttribute<sight::atoms::String>("value"));

        // Retrieves resection date and time
        sight::atoms::Object::sptr dateTimeObj =
            sight::atoms::Object::dynamicCast(oldPlanning->getValue().find("startDateTime")->second);
        sight::atoms::String::sptr dateTimeStr =
            sight::atoms::String::dynamicCast(dateTimeObj->getAttribute<sight::atoms::String>("value"));

        sight::atoms::String::sptr time = sight::atoms::String::New("");
        sight::atoms::String::sptr date = sight::atoms::String::New("");

        std::string dateTimeStd = dateTimeStr->getString();
        std::vector<std::string> strs;
        ::boost::split(strs, dateTimeStd, ::boost::is_any_of(" "));

        if(strs.size() >= 2)
        {
            date->setValue(strs[0]);
            time->setValue(strs[1]);
        }

        io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();
        sight::atoms::Object::sptr newActivitySeries         = creators->create("sight::data::ActivitySeries", "1");

        sight::atoms::Object::sptr imgComposite = sight::atoms::Object::New();
        io::atoms::patch::helper::setClassname(imgComposite, "sight::data::Composite");
        io::atoms::patch::helper::setVersion(imgComposite, "1");
        io::atoms::patch::helper::generateID(imgComposite);
        io::atoms::patch::helper::cleanFields(imgComposite);

        io::atoms::patch::helper::Object imgCompositeHelper(imgComposite);
        sight::atoms::Map::sptr compositeMap = sight::atoms::Map::New();
        compositeMap->insert("OptionalInputImageKey", imageSeries->getAttribute<sight::atoms::Object>("image"));
        imgCompositeHelper.addAttribute("values", compositeMap);

        sight::atoms::Map::sptr activityDataMap = sight::atoms::Map::New();
        activityDataMap->insert("resectionDB", resectionDB);
        activityDataMap->insert("modelSeries", it->second);
        activityDataMap->insert("imageSeries", imgComposite);

        sight::atoms::Object::sptr mapObj = sight::atoms::Object::New();
        io::atoms::patch::helper::setClassname(mapObj, "sight::data::Composite");
        io::atoms::patch::helper::setVersion(mapObj, "1");
        io::atoms::patch::helper::generateID(mapObj);
        io::atoms::patch::helper::cleanFields(mapObj);

        io::atoms::patch::helper::Object mapObjHelper(mapObj);
        mapObjHelper.addAttribute("values", activityDataMap);

        io::atoms::patch::helper::Object helperActivity(newActivitySeries);
        helperActivity.replaceAttribute("activity_config_id", sight::atoms::String::New("Resection"));
        helperActivity.replaceAttribute("data", mapObj);

        helperActivity.replaceAttribute("modality", sight::atoms::String::New("OT"));
        helperActivity.replaceAttribute("instance_uid", sight::atoms::String::New(core::tools::UUID::generateUUID()));
        helperActivity.replaceAttribute("date", date);
        helperActivity.replaceAttribute("time", time);
        helperActivity.replaceAttribute("performing_physicians_name", experts);
        helperActivity.replaceAttribute("description", informationStr);
        helperActivity.replaceAttribute("patient", imageSeries->getAttribute<sight::atoms::Object>("patient"));
        helperActivity.replaceAttribute("study", imageSeries->getAttribute<sight::atoms::Object>("study"));
        helperActivity.replaceAttribute("equipment", imageSeries->getAttribute<sight::atoms::Object>("equipment"));

        series->push_back(newActivitySeries);
    }
}

// ----------------------------------------------------------------------------

// Convert ProcessingDB Composite into ActivitySeries
void processProcessing(
    const sight::atoms::Map::sptr& oldCompositeMap,
    const sight::atoms::Sequence::sptr& series,
    const Image2ModelType& image2Model,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    sight::atoms::Object::sptr oldProcessingDB =
        sight::atoms::Object::dynamicCast((*oldCompositeMap)["processingDB"]);

    sight::atoms::Map::sptr oldProcessings = oldProcessingDB->getAttribute<sight::atoms::Map>("values");

    for(sight::atoms::Map::value_type oldProcessingAtom : oldProcessings->getValue())
    {
        sight::atoms::Map::sptr oldProcessing =
            sight::atoms::Object::dynamicCast(oldProcessingAtom.second)->getAttribute<sight::atoms::Map>("values");

        // Retrieves expert who performed the resection
        sight::atoms::Object::sptr expert =
            sight::atoms::Object::dynamicCast(oldProcessing->getValue().find("expert")->second);

        sight::atoms::Sequence::sptr experts = sight::atoms::Sequence::New();
        experts->push_back(sight::atoms::String::dynamicCast(expert->getAttribute<sight::atoms::String>("value")));

        // Retrieves resection information
        sight::atoms::Object::sptr information =
            sight::atoms::Object::dynamicCast(oldProcessing->getValue().find("information")->second);
        sight::atoms::String::sptr informationStr =
            sight::atoms::String::dynamicCast(information->getAttribute<sight::atoms::String>("value"));

        // Retrieves resection date and time
        sight::atoms::Object::sptr dateTimeObj =
            sight::atoms::Object::dynamicCast(oldProcessing->getValue().find("startDateTime")->second);
        sight::atoms::String::sptr dateTimeStr =
            sight::atoms::String::dynamicCast(dateTimeObj->getAttribute<sight::atoms::String>("value"));

        sight::atoms::String::sptr time = sight::atoms::String::New("");
        sight::atoms::String::sptr date = sight::atoms::String::New("");

        std::string dateTimeStd = dateTimeStr->getString();
        std::vector<std::string> strs;
        ::boost::split(strs, dateTimeStd, ::boost::is_any_of(" "));

        if(strs.size() >= 2)
        {
            date->setValue(strs[0]);
            time->setValue(strs[1]);
        }

        io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();
        sight::atoms::Object::sptr newActivitySeries         = creators->create("sight::data::ActivitySeries", "1");

        sight::atoms::Map::sptr activityDataMap = sight::atoms::Map::New();
        activityDataMap->insert("processing", oldProcessingAtom.second);

        sight::atoms::Object::sptr mapObj = sight::atoms::Object::New();
        io::atoms::patch::helper::setClassname(mapObj, "sight::data::Composite");
        io::atoms::patch::helper::setVersion(mapObj, "1");
        io::atoms::patch::helper::generateID(mapObj);
        io::atoms::patch::helper::cleanFields(mapObj);

        io::atoms::patch::helper::Object mapObjHelper(mapObj);
        mapObjHelper.addAttribute("values", activityDataMap);

        io::atoms::patch::helper::Object helperActivity(newActivitySeries);
        helperActivity.replaceAttribute("activity_config_id", sight::atoms::String::New("Processing"));
        helperActivity.replaceAttribute("data", mapObj);

        helperActivity.replaceAttribute("modality", sight::atoms::String::New("OT"));
        helperActivity.replaceAttribute("instance_uid", sight::atoms::String::New(core::tools::UUID::generateUUID()));
        helperActivity.replaceAttribute("date", date);
        helperActivity.replaceAttribute("time", time);
        helperActivity.replaceAttribute("performing_physicians_name", experts);
        helperActivity.replaceAttribute("description", informationStr);

        // Check if the processing is associated to an acquisition
        sight::atoms::Base::sptr acquisitionSelection = oldProcessing->getValue().find("acquisitionSelection")->second;
        sight::atoms::Object::sptr acqSelectionObj    = sight::atoms::Object::dynamicCast(acquisitionSelection);
        SIGHT_ASSERT("Failed to cast acquisition composite to object", acqSelectionObj);

        sight::atoms::Map::sptr acqSelectionMap = acqSelectionObj->getAttribute<sight::atoms::Map>("values");

        sight::atoms::Map::const_iterator iter = acqSelectionMap->getValue().find("selectedAcquisition");
        if(iter != acqSelectionMap->getValue().end())
        {
            // get associated image
            sight::atoms::Base::sptr acquisition = iter->second;
            sight::atoms::Object::sptr acqObj    = sight::atoms::Object::dynamicCast(acquisition);
            SIGHT_ASSERT("Failed to cast acquisition to object", acqObj);

            Image2ModelType::const_iterator it = image2Model.find(newVersions[acqObj]);
            if(it != image2Model.end())
            {
                sight::atoms::Object::sptr imageSeries = it->first;
                helperActivity.replaceAttribute(
                    "patient",
                    imageSeries->getAttribute<sight::atoms::Object>("patient")
                );
                helperActivity.replaceAttribute("study", imageSeries->getAttribute<sight::atoms::Object>("study"));
                helperActivity.replaceAttribute(
                    "equipment",
                    imageSeries->getAttribute<sight::atoms::Object>("equipment")
                );
            }
        }

        series->push_back(newActivitySeries);
    }
}

// ----------------------------------------------------------------------------

void Composite::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    Image2ModelType image2Model;

    ISemanticPatch::apply(previous, current, newVersions);

    // Create helper
    io::atoms::patch::helper::Object helper(current);

    // If data::Composite is a MedicalWorkspace
    if(previous->getMetaInfo("compositeType") == "MedicalWorkspace")
    {
        // Update type/version
        io::atoms::patch::helper::setClassname(current, "sight::data::SeriesDB");
        io::atoms::patch::helper::setVersion(current, "1");

        // Update Attributes
        io::atoms::patch::helper::cleanFields(current);
        // Replace sight::atoms::Map by sight::atoms::Sequence ( have the same key )
        sight::atoms::Sequence::sptr series = sight::atoms::Sequence::New();
        helper.replaceAttribute("values", series);

        // Add ImageSeries and create ModelSeries if they are necessary
        sight::atoms::Map::sptr oldCompositeMap = previous->getAttribute<sight::atoms::Map>("values");
        sight::atoms::Object::sptr oldPatientDB =
            sight::atoms::Object::dynamicCast((*oldCompositeMap)["patientDB"]);
        sight::atoms::Sequence::sptr oldPatients = oldPatientDB->getAttribute<sight::atoms::Sequence>("patients");

        io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();

        for(sight::atoms::Base::sptr oldPatientAtom : oldPatients->getValue())
        {
            sight::atoms::Object::sptr oldPatient = sight::atoms::Object::dynamicCast(oldPatientAtom);
            sight::atoms::Object::sptr newPatient = newVersions[oldPatient];

            sight::atoms::Sequence::sptr oldStudies = oldPatient->getAttribute<sight::atoms::Sequence>("studies");

            for(sight::atoms::Base::sptr oldStudyAtom : oldStudies->getValue())
            {
                sight::atoms::Object::sptr oldStudy = sight::atoms::Object::dynamicCast(oldStudyAtom);
                sight::atoms::Object::sptr newStudy = newVersions[oldStudy];

                sight::atoms::Sequence::sptr oldAcquisitions =
                    oldStudy->getAttribute<sight::atoms::Sequence>("acquisitions");

                for(sight::atoms::Base::sptr oldAcqAtom : oldAcquisitions->getValue())
                {
                    sight::atoms::Object::sptr oldAcq = sight::atoms::Object::dynamicCast(oldAcqAtom);

                    // finalize and push newImgSeries
                    sight::atoms::Object::sptr newImgSeries = newVersions[oldAcq];
                    io::atoms::patch::helper::Object imgSeriesHelper(newImgSeries);
                    sight::atoms::Object::sptr clonedPatient = sight::atoms::Object::dynamicCast(newPatient->clone());
                    io::atoms::patch::helper::changeUID(clonedPatient);

                    imgSeriesHelper.replaceAttribute("patient", clonedPatient);
                    sight::atoms::Object::sptr clonedStudy = sight::atoms::Object::dynamicCast(newStudy->clone());
                    io::atoms::patch::helper::changeUID(clonedStudy);

                    imgSeriesHelper.replaceAttribute("study", clonedStudy);
                    imgSeriesHelper.replaceAttribute("modality", oldStudy->getAttribute("modality")->clone());

                    sight::atoms::String::sptr institution;
                    institution =
                        sight::atoms::String::dynamicCast(oldStudy->getAttribute("hospital")->clone());
                    sight::atoms::Object::sptr equipment = newImgSeries->getAttribute<sight::atoms::Object>(
                        "equipment"
                    );
                    io::atoms::patch::helper::Object equipmentHelper(equipment);
                    equipmentHelper.replaceAttribute("institution_name", institution);

                    // Set study date and time if necessary
                    sight::atoms::String::sptr studyDate = newStudy->getAttribute<sight::atoms::String>("date");
                    io::atoms::patch::helper::Object studyHelper(clonedStudy);

                    if(studyDate->getValue().empty())
                    {
                        studyHelper.replaceAttribute("date", newImgSeries->getAttribute("date")->clone());
                    }

                    sight::atoms::String::sptr studyTime = newStudy->getAttribute<sight::atoms::String>("time");
                    if(studyTime->getValue().empty())
                    {
                        studyHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone());
                    }

                    series->push_back(newImgSeries);

                    // finalize and push newModelSeries
                    sight::atoms::Sequence::sptr oldReconstructions =
                        oldAcq->getAttribute<sight::atoms::Sequence>("reconstructions");

                    if(oldReconstructions->size() > 0)
                    {
                        // Create new model series
                        sight::atoms::Object::sptr newModelSeries =
                            creators->create("sight::data::ModelSeries", "1");

                        io::atoms::patch::helper::Object msHelper(newModelSeries);
                        sight::atoms::Object::sptr msPatient =
                            sight::atoms::Object::dynamicCast(clonedPatient->clone());
                        io::atoms::patch::helper::changeUID(msPatient);
                        msHelper.replaceAttribute("patient", msPatient);

                        sight::atoms::Object::sptr msStudy = sight::atoms::Object::dynamicCast(clonedStudy->clone());
                        io::atoms::patch::helper::changeUID(msStudy);
                        msHelper.replaceAttribute("study", msStudy);

                        sight::atoms::Object::sptr msEquipment = sight::atoms::Object::dynamicCast(equipment->clone());
                        io::atoms::patch::helper::changeUID(msEquipment);
                        msHelper.replaceAttribute("equipment", msEquipment);

                        msHelper.replaceAttribute("modality", sight::atoms::String::New("OT"));
                        msHelper.replaceAttribute(
                            "instance_uid",
                            sight::atoms::String::New(core::tools::UUID::generateUUID())
                        );
                        msHelper.replaceAttribute("date", newImgSeries->getAttribute("date")->clone());
                        msHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone());
                        msHelper.replaceAttribute(
                            "performing_physicians_name",
                            newImgSeries->getAttribute("performing_physicians_name")->clone()
                        );
                        msHelper.replaceAttribute("description", newImgSeries->getAttribute("description")->clone());

                        sight::atoms::Sequence::sptr newReconstructions =
                            newModelSeries->getAttribute<sight::atoms::Sequence>("reconstruction_db");
                        for(sight::atoms::Base::sptr oldRecAtom : oldReconstructions->getValue())
                        {
                            sight::atoms::Object::sptr oldRec = sight::atoms::Object::dynamicCast(oldRecAtom);
                            newReconstructions->push_back(newVersions[oldRec]);
                        }

                        image2Model[newImgSeries] = newModelSeries;
                        series->push_back(newModelSeries);
                    }
                }
            }
        }

        processPlanning(oldCompositeMap, series, image2Model, newVersions);
        processProcessing(oldCompositeMap, series, image2Model, newVersions);
    } // End "MedicalWorkspace"
}

} // namespace data

} // namespace V2

} // namespace V1

} // namespace sight::io::patch::semantic
