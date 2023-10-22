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
    const unsigned char _nb_img_series,
    const unsigned char _nb_model_series
)
{
    auto series_set = std::make_shared<data::series_set>();

    for(unsigned char nb = 0 ; nb < _nb_img_series ; ++nb)
    {
        data::series::sptr img_series;
        img_series = series_set::createImageSeries();
        series_set->push_back(img_series);
    }

    for(unsigned char nb = 0 ; nb < _nb_model_series ; ++nb)
    {
        data::series::sptr model_series;
        model_series = series_set::createModelSeries(static_cast<unsigned char>(safe_rand() % 5 + 1));
        series_set->push_back(model_series);
    }

    return series_set;
}

//------------------------------------------------------------------------------

void series_set::generateSeriesInformation(data::series::sptr _series)
{
    static unsigned int count = 1;
    std::stringstream str;
    str.width(4);
    str.fill('0');
    str << count++;

    const std::string uid         = "1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str();
    const std::string modality    = "CT";
    const std::string date        = "20130418";
    const std::string time        = "101010.101010";
    const std::string description = "Description";

    data::dicom_values_t performing_physician_name;
    _series->setPerformingPhysicianName(
        "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John"
    );

    _series->setSeriesInstanceUID(uid);
    _series->setModality(modality);
    _series->setSeriesDate(date);
    _series->setSeriesTime(time);
    _series->setSeriesDescription(description);

    const std::string institution = "IRCAD";

    _series->setInstitutionName(institution);

    const std::string patient_name      = "NomSeriesSet1";
    const std::string patient_firstname = "PrenomSeriesSet1";
    const std::string patient_id        = "4564383757";
    const std::string patient_birthdate = "19710418";
    const std::string patient_sex       = "O ";

    _series->setPatientName((patient_name + "^") + patient_firstname);
    _series->setPatientID(patient_id);
    _series->setPatientBirthDate(patient_birthdate);
    _series->setPatientSex(patient_sex);

    // studies informations
    const std::string study_uid         = "1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str();
    const std::string study_date        = "20130418";
    const std::string study_time        = "095948.689872";
    const std::string study_physician   = "Dr^Jekyl";
    const std::string study_description = "Say 33.";
    const std::string patient_age       = "042Y";

    _series->setStudyInstanceUID(study_uid);
    _series->setStudyDate(study_date);
    _series->setStudyTime(study_time);
    _series->setReferringPhysicianName(study_physician);
    _series->setStudyDescription(study_description);
    _series->setPatientAge(patient_age);
}

//------------------------------------------------------------------------------

data::image_series::sptr series_set::createImageSeries()
{
    data::image_series::sptr img_series = std::make_shared<data::image_series>();

    series_set::generateSeriesInformation(img_series);

    image::generateRandomImage(img_series, core::type::INT16);

    return img_series;
}

//------------------------------------------------------------------------------

data::model_series::sptr series_set::createModelSeries(unsigned char _nb_reconstruction)
{
    data::model_series::sptr model_series = std::make_shared<data::model_series>();

    series_set::generateSeriesInformation(model_series);

    data::model_series::reconstruction_vector_t rec_db;
    for(unsigned char nb = 0 ; nb < _nb_reconstruction ; ++nb)
    {
        data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
        // Generates reconstruction with a prefixed index in organName "0_Liver", "1_Liver", ...
        series_set::generateReconstruction(rec, static_cast<int>(nb));

        rec_db.push_back(rec);
    }

    model_series->setReconstructionDB(rec_db);

    return model_series;
}

//------------------------------------------------------------------------------

data::activity::sptr series_set::createActivity()
{
    data::activity::sptr activity = std::make_shared<data::activity>();

    const std::string config_id = "IdOfTheConfig";
    activity->setActivityConfigId(config_id);

    (*activity)["key1"] = std::make_shared<data::string>("ValueOfKey1");

    return activity;
}

//------------------------------------------------------------------------------

void series_set::generateReconstruction(data::reconstruction::sptr _rec, int _index)
{
    _rec->setIsVisible(true);
    const std::string name = "Liver";
    std::string organ_name;
    //If needed, prefix organ name by the number of the reconstruction, to ensure the same reading order for tests.
    if(_index > -1)
    {
        organ_name = std::to_string(_index) + "_" + name;
    }
    else
    {
        organ_name = name;
    }

    _rec->setOrganName(organ_name);
    _rec->set_structure_type("Liver");

    data::image::sptr img = std::make_shared<data::image>();
    image::generateRandomImage(img, core::type::UINT16);
    _rec->setImage(img);

    data::material::sptr material = std::make_shared<data::material>();
    material->ambient()->red()   = 0.75F;
    material->ambient()->green() = 0.10F;
    material->ambient()->blue()  = 0.56F;
    material->ambient()->alpha() = 0.8F;
    material->diffuse()->red()   = 0.85F;
    material->diffuse()->green() = 0.20F;
    material->diffuse()->blue()  = 0.66F;
    material->diffuse()->alpha() = 0.9F;
    _rec->setMaterial(material);

    data::mesh::sptr mesh = std::make_shared<data::mesh>();
    mesh::generateMesh(mesh);

    _rec->setMesh(mesh);
}

} // namespace sight::utest_data::generator
