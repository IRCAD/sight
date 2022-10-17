/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "utestData/generator/SeriesSet.hpp"

#include "utestData/generator/Image.hpp"
#include "utestData/generator/Mesh.hpp"

#include <core/tools/random/Generator.hpp>
#include <core/Type.hpp>

#include <data/Activity.hpp>
#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>
#include <data/String.hpp>

#include <sstream>

namespace sight::utestData::generator
{

using core::tools::random::safeRand;

//------------------------------------------------------------------------------

data::SeriesSet::sptr SeriesSet::createSeriesSet(
    const unsigned char nbImgSeries,
    const unsigned char nbModelSeries
)
{
    auto series_set = data::SeriesSet::New();

    for(unsigned char nb = 0 ; nb < nbImgSeries ; ++nb)
    {
        data::Series::sptr imgSeries;
        imgSeries = SeriesSet::createImageSeries();
        series_set->push_back(imgSeries);
    }

    for(unsigned char nb = 0 ; nb < nbModelSeries ; ++nb)
    {
        data::Series::sptr modelSeries;
        modelSeries = SeriesSet::createModelSeries(static_cast<unsigned char>(safeRand() % 5 + 1));
        series_set->push_back(modelSeries);
    }

    return series_set;
}

//------------------------------------------------------------------------------

void SeriesSet::generateSeriesInformation(data::Series::sptr series)
{
    static unsigned int count = 1;
    std::stringstream str;
    str.width(4);
    str.fill('0');
    str << count++;

    const std::string UID         = "1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str();
    const std::string MODALITY    = "CT";
    const std::string DATE        = "20130418";
    const std::string TIME        = "101010.101010";
    const std::string DESCRIPTION = "Description";

    data::DicomValuesType performingPhysicianName;
    series->setPerformingPhysicianName(
        "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John"
    );

    series->setSeriesInstanceUID(UID);
    series->setModality(MODALITY);
    series->setSeriesDate(DATE);
    series->setSeriesTime(TIME);
    series->setSeriesDescription(DESCRIPTION);

    const std::string INSTITUTION = "IRCAD";

    series->setInstitutionName(INSTITUTION);

    const std::string PATIENT_NAME      = "NomSeriesSet1";
    const std::string PATIENT_FIRSTNAME = "PrenomSeriesSet1";
    const std::string PATIENT_ID        = "4564383757";
    const std::string PATIENT_BIRTHDATE = "19710418";
    const std::string PATIENT_SEX       = "O ";

    series->setPatientName((PATIENT_NAME + "^") + PATIENT_FIRSTNAME);
    series->setPatientID(PATIENT_ID);
    series->setPatientBirthDate(PATIENT_BIRTHDATE);
    series->setPatientSex(PATIENT_SEX);

    // studies informations
    const std::string STUDY_UID         = "1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str();
    const std::string STUDY_DATE        = "20130418";
    const std::string STUDY_TIME        = "095948.689872";
    const std::string STUDY_PHYSICIAN   = "Dr^Jekyl";
    const std::string STUDY_DESCRIPTION = "Say 33.";
    const std::string PATIENT_AGE       = "042Y";

    series->setStudyInstanceUID(STUDY_UID);
    series->setStudyDate(STUDY_DATE);
    series->setStudyTime(STUDY_TIME);
    series->setReferringPhysicianName(STUDY_PHYSICIAN);
    series->setStudyDescription(STUDY_DESCRIPTION);
    series->setPatientAge(PATIENT_AGE);
}

//------------------------------------------------------------------------------

data::ImageSeries::sptr SeriesSet::createImageSeries()
{
    data::ImageSeries::sptr imgSeries = data::ImageSeries::New();

    SeriesSet::generateSeriesInformation(imgSeries);

    Image::generateRandomImage(imgSeries, core::Type::INT16);

    return imgSeries;
}

//------------------------------------------------------------------------------

data::ModelSeries::sptr SeriesSet::createModelSeries(unsigned char nbReconstruction)
{
    data::ModelSeries::sptr modelSeries = data::ModelSeries::New();

    SeriesSet::generateSeriesInformation(modelSeries);

    data::ModelSeries::ReconstructionVectorType recDB;
    for(unsigned char nb = 0 ; nb < nbReconstruction ; ++nb)
    {
        data::Reconstruction::sptr rec = data::Reconstruction::New();
        // Generates reconstruction with a prefixed index in organName "0_Liver", "1_Liver", ...
        SeriesSet::generateReconstruction(rec, static_cast<int>(nb));

        recDB.push_back(rec);
    }

    modelSeries->setReconstructionDB(recDB);

    return modelSeries;
}

//------------------------------------------------------------------------------

data::Activity::sptr SeriesSet::createActivity()
{
    data::Activity::sptr activity = data::Activity::New();

    const std::string CONFIG_ID = "IdOfTheConfig";
    activity->setActivityConfigId(CONFIG_ID);

    auto data = data::Composite::New();
    (*data)["key1"] = data::String::New("ValueOfKey1");
    activity->setData(data);

    return activity;
}

//------------------------------------------------------------------------------

void SeriesSet::generateReconstruction(data::Reconstruction::sptr rec, int index)
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
    Image::generateRandomImage(img, core::Type::UINT16);
    rec->setImage(img);

    data::Material::sptr material = data::Material::New();
    material->ambient()->red()   = 0.75F;
    material->ambient()->green() = 0.10F;
    material->ambient()->blue()  = 0.56F;
    material->ambient()->alpha() = 0.8F;
    material->diffuse()->red()   = 0.85F;
    material->diffuse()->green() = 0.20F;
    material->diffuse()->blue()  = 0.66F;
    material->diffuse()->alpha() = 0.9F;
    rec->setMaterial(material);

    data::Mesh::sptr mesh = data::Mesh::New();
    Mesh::generateMesh(mesh);

    rec->setMesh(mesh);
}

} // namespace sight::utestData::generator
