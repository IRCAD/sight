/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "utest_data/generator/series_set.hpp"

#include "utest_data/generator/image.hpp"
#include "utest_data/generator/mesh.hpp"

#include <core/tools/random/generator.hpp>
#include <core/type.hpp>

#include <data/activity.hpp>
#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>

#include <sstream>

namespace sight::utest_data::generator
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

data::series_set::sptr series_set::createSeriesSet(
    const unsigned char nbImgSeries,
    const unsigned char nbModelSeries
)
{
    auto series_set = std::make_shared<data::series_set>();

    for(unsigned char nb = 0 ; nb < nbImgSeries ; ++nb)
    {
        data::series::sptr imgSeries;
        imgSeries = series_set::createImageSeries();
        series_set->push_back(imgSeries);
    }

    for(unsigned char nb = 0 ; nb < nbModelSeries ; ++nb)
    {
        data::series::sptr modelSeries;
        modelSeries = series_set::createModelSeries(static_cast<unsigned char>(safe_rand() % 5 + 1));
        series_set->push_back(modelSeries);
    }

    return series_set;
}

//------------------------------------------------------------------------------

void series_set::generateSeriesInformation(data::series::sptr series)
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

data::image_series::sptr series_set::createImageSeries()
{
    data::image_series::sptr imgSeries = std::make_shared<data::image_series>();

    series_set::generateSeriesInformation(imgSeries);

    image::generateRandomImage(imgSeries, core::type::INT16);

    return imgSeries;
}

//------------------------------------------------------------------------------

data::model_series::sptr series_set::createModelSeries(unsigned char nbReconstruction)
{
    data::model_series::sptr modelSeries = std::make_shared<data::model_series>();

    series_set::generateSeriesInformation(modelSeries);

    data::model_series::ReconstructionVectorType recDB;
    for(unsigned char nb = 0 ; nb < nbReconstruction ; ++nb)
    {
        data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
        // Generates reconstruction with a prefixed index in organName "0_Liver", "1_Liver", ...
        series_set::generateReconstruction(rec, static_cast<int>(nb));

        recDB.push_back(rec);
    }

    modelSeries->setReconstructionDB(recDB);

    return modelSeries;
}

//------------------------------------------------------------------------------

data::activity::sptr series_set::createActivity()
{
    data::activity::sptr activity = std::make_shared<data::activity>();

    const std::string CONFIG_ID = "IdOfTheConfig";
    activity->setActivityConfigId(CONFIG_ID);

    (*activity)["key1"] = std::make_shared<data::string>("ValueOfKey1");

    return activity;
}

//------------------------------------------------------------------------------

void series_set::generateReconstruction(data::reconstruction::sptr rec, int index)
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

    data::image::sptr img = std::make_shared<data::image>();
    image::generateRandomImage(img, core::type::UINT16);
    rec->setImage(img);

    data::material::sptr material = std::make_shared<data::material>();
    material->ambient()->red()   = 0.75F;
    material->ambient()->green() = 0.10F;
    material->ambient()->blue()  = 0.56F;
    material->ambient()->alpha() = 0.8F;
    material->diffuse()->red()   = 0.85F;
    material->diffuse()->green() = 0.20F;
    material->diffuse()->blue()  = 0.66F;
    material->diffuse()->alpha() = 0.9F;
    rec->setMaterial(material);

    data::mesh::sptr mesh = std::make_shared<data::mesh>();
    mesh::generateMesh(mesh);

    rec->setMesh(mesh);
}

} // namespace sight::utest_data::generator
