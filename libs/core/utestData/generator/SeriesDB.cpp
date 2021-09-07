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

#include "utestData/generator/SeriesDB.hpp"

#include "utestData/generator/Image.hpp"
#include "utestData/generator/Mesh.hpp"

#include <core/tools/random/Generator.hpp>
#include <core/tools/Type.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>
#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Patient.hpp>
#include <data/Reconstruction.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>
#include <data/Study.hpp>

#include <sstream>

namespace sight::utestData
{

namespace generator
{

using core::tools::random::safeRand;

//------------------------------------------------------------------------------

data::SeriesDB::sptr SeriesDB::createSeriesDB(
    const unsigned char nbImgSeries,
    const unsigned char nbModelSeries,
    const unsigned char nbActivitySeries
)
{
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();
    for(unsigned char nb = 0 ; nb < nbImgSeries ; ++nb)
    {
        data::Series::sptr imgSeries;
        imgSeries = SeriesDB::createImageSeries();
        seriesDB->getContainer().push_back(imgSeries);
    }

    for(unsigned char nb = 0 ; nb < nbModelSeries ; ++nb)
    {
        data::Series::sptr modelSeries;
        modelSeries = SeriesDB::createModelSeries(static_cast<unsigned char>(safeRand() % 5 + 1));
        seriesDB->getContainer().push_back(modelSeries);
    }

    for(unsigned char nb = 0 ; nb < nbActivitySeries ; ++nb)
    {
        data::Series::sptr activitySeries;
        activitySeries = SeriesDB::createActivitySeries();
        seriesDB->getContainer().push_back(activitySeries);
    }

    return seriesDB;
}

//------------------------------------------------------------------------------

data::Patient::sptr SeriesDB::createPatient()
{
    data::Patient::sptr patient         = data::Patient::New();
    const std::string PATIENT_NAME      = "NomSeriesDB1";
    const std::string PATIENT_FIRSTNAME = "PrenomSeriesDB1";
    const std::string PATIENT_ID        = "4564383757";
    const std::string PATIENT_BIRTHDATE = "19710418";
    const std::string PATIENT_SEX       = "O ";

    patient->setName(PATIENT_NAME + "^" + PATIENT_FIRSTNAME);
    patient->setPatientId(PATIENT_ID);
    patient->setBirthdate(PATIENT_BIRTHDATE);
    patient->setSex(PATIENT_SEX);

    return patient;
}

//------------------------------------------------------------------------------

data::Study::sptr SeriesDB::createStudy()
{
    data::Study::sptr study = data::Study::New();

    static unsigned int count = 1;
    std::stringstream str;
    str.width(4);
    str.fill('0');
    str << count++;

    // studies informations
    const std::string UID         = "1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str();
    const std::string DATE        = "20130418";
    const std::string TIME        = "095948.689872 ";
    const std::string PHYSICIAN   = "Dr^Jekyl";
    const std::string CONSULTANT  = "Dr^Doolite";
    const std::string DESCRIPTION = "Say 33. ";
    const std::string PATIENT_AGE = "042Y";

    study->setInstanceUID(UID);
    study->setDate(DATE);
    study->setTime(TIME);
    study->setReferringPhysicianName(PHYSICIAN);
    study->setConsultingPhysicianName(CONSULTANT);
    study->setDescription(DESCRIPTION);
    study->setPatientAge(PATIENT_AGE);

    return study;
}

//------------------------------------------------------------------------------

data::Equipment::sptr SeriesDB::createEquipement()
{
    data::Equipment::sptr equipment = data::Equipment::New();
    const std::string INSTITUTION   = "hospital";
    equipment->setInstitutionName(INSTITUTION);

    return equipment;
}

//------------------------------------------------------------------------------

void SeriesDB::generateSeriesInformation(data::Series::sptr series)
{
    series->setPatient(SeriesDB::createPatient());
    series->setStudy(SeriesDB::createStudy());
    series->setEquipment(SeriesDB::createEquipement());

    static unsigned int count = 1;
    std::stringstream str;
    str.width(4);
    str.fill('0');
    str << count++;

    const std::string UID         = "1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str();
    const std::string MODALITY    = "CT";
    const std::string DATE        = "20130418";
    const std::string TIME        = "101010.101010 ";
    const std::string DESCRIPTION = "Description ";

    data::DicomValuesType performingPhysiciansName;
    performingPhysiciansName.push_back("Dr^Jekyl");
    performingPhysiciansName.push_back("Dr^House");
    performingPhysiciansName.push_back("Dr^Einstein ");
    series->setPerformingPhysiciansName(performingPhysiciansName);

    series->setInstanceUID(UID);
    series->setModality(MODALITY);
    series->setDate(DATE);
    series->setTime(TIME);
    series->setDescription(DESCRIPTION);
}

//------------------------------------------------------------------------------

data::ImageSeries::sptr SeriesDB::createImageSeries()
{
    data::ImageSeries::sptr imgSeries = data::ImageSeries::New();

    SeriesDB::generateSeriesInformation(imgSeries);

    data::Image::sptr image = data::Image::New();
    Image::generateRandomImage(image, core::tools::Type::s_INT16);

    imgSeries->setImage(image);

    return imgSeries;
}

//------------------------------------------------------------------------------

data::ModelSeries::sptr SeriesDB::createModelSeries(unsigned char nbReconstruction)
{
    data::ModelSeries::sptr modelSeries = data::ModelSeries::New();

    SeriesDB::generateSeriesInformation(modelSeries);

    data::ModelSeries::ReconstructionVectorType recDB;
    for(unsigned char nb = 0 ; nb < nbReconstruction ; ++nb)
    {
        data::Reconstruction::sptr rec = data::Reconstruction::New();
        // Generates reconstruction with a prefixed index in organName "0_Liver", "1_Liver", ...
        SeriesDB::generateReconstruction(rec, static_cast<int>(nb));

        recDB.push_back(rec);
    }

    modelSeries->setReconstructionDB(recDB);

    return modelSeries;
}

//------------------------------------------------------------------------------

data::ActivitySeries::sptr SeriesDB::createActivitySeries()
{
    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::New();

    SeriesDB::generateSeriesInformation(activitySeries);

    const std::string CONFIG_ID = "IdOfTheConfig";
    activitySeries->setActivityConfigId(CONFIG_ID);

    data::Composite::sptr data = data::Composite::New();
    data->getContainer()["key1"] = data::String::New("ValueOfKey1");
    activitySeries->setData(data);

    return activitySeries;
}

//------------------------------------------------------------------------------

void SeriesDB::generateReconstruction(data::Reconstruction::sptr rec, int index)
{
    rec->setIsVisible(true);
    const std::string name = "Liver";
    std::string organName;
    //If needed, prefix organ name by the number of the reconstruction, to ensure the same reading order for tests.
    if(index > -1)
    {
        organName = std::to_string(index) + "_" + name;
    }
    else
    {
        organName = name;
    }

    rec->setOrganName(organName);
    rec->setStructureType("Liver");

    data::Image::sptr img = data::Image::New();
    Image::generateRandomImage(img, core::tools::Type::create("uint16"));
    rec->setImage(img);

    data::Material::sptr material = data::Material::New();
    material->ambient()->red()   = 0.75f;
    material->ambient()->green() = 0.10f;
    material->ambient()->blue()  = 0.56f;
    material->ambient()->alpha() = 0.8f;
    material->diffuse()->red()   = 0.85f;
    material->diffuse()->green() = 0.20f;
    material->diffuse()->blue()  = 0.66f;
    material->diffuse()->alpha() = 0.9f;
    rec->setMaterial(material);

    data::Mesh::sptr mesh = data::Mesh::New();
    Mesh::generateMesh(mesh);

    rec->setMesh(mesh);
}

//------------------------------------------------------------------------------

} // namespace generator

} // namespace sight::utestData
