/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

data::series_set::sptr series_set::create_series_set(
    const unsigned char _nb_img_series,
    const unsigned char _nb_model_series
)
{
    auto series_set = std::make_shared<data::series_set>();

    for(unsigned char nb = 0 ; nb < _nb_img_series ; ++nb)
    {
        data::series::sptr img_series;
        img_series = series_set::create_image_series();
        series_set->push_back(img_series);
    }

    for(unsigned char nb = 0 ; nb < _nb_model_series ; ++nb)
    {
        data::series::sptr model_series;
        model_series = series_set::create_model_series(static_cast<unsigned char>(safe_rand() % 5 + 1));
        series_set->push_back(model_series);
    }

    return series_set;
}

//------------------------------------------------------------------------------

void series_set::generate_series_information(data::series::sptr _series)
{
    static unsigned int count = 1;
    std::stringstream str;
    str.width(4);
    str.fill('0');
    str << count++;

    const std::string uid         = "1.2.826.0.1.3680043.2.1125.102906542887009256605006409108689" + str.str();
    const auto modality           = data::dicom::modality_t::ct;
    const std::string date        = "20130418";
    const std::string time        = "101010.101010";
    const std::string description = "Description";

    data::dicom_values_t performing_physician_name;
    _series->set_performing_physician_name(
        "Adams^John Robert Quincy^^Rev.^B.A. M.Div.\\Morrison-Jones^Susan^^^Ph.D., Chief Executive Officer\\Doe^John"
    );

    _series->set_series_instance_uid(uid);
    _series->set_modality(modality);
    _series->set_series_date(date);
    _series->set_series_time(time);
    _series->set_series_description(description);

    const std::string institution = "IRCAD";

    _series->set_institution_name(institution);

    const std::string patient_name      = "NomSeriesSet1";
    const std::string patient_firstname = "PrenomSeriesSet1";
    const std::string patient_id        = "4564383757";
    const std::string patient_birthdate = "19710418";
    const std::string patient_sex       = "O ";

    _series->set_patient_name((patient_name + "^") + patient_firstname);
    _series->set_patient_id(patient_id);
    _series->set_patient_birth_date(patient_birthdate);
    _series->set_patient_sex(patient_sex);

    // studies informations
    const std::string study_uid         = "1.2.826.0.1.3680043.2.1125.44278200849347599055201494082232" + str.str();
    const std::string study_date        = "20130418";
    const std::string study_time        = "095948.689872";
    const std::string study_physician   = "Dr^Jekyl";
    const std::string study_description = "Say 33.";
    const std::string patient_age       = "042Y";

    _series->set_study_instance_uid(study_uid);
    _series->set_study_date(study_date);
    _series->set_study_time(study_time);
    _series->set_referring_physician_name(study_physician);
    _series->set_study_description(study_description);
    _series->set_patient_age(patient_age);
}

//------------------------------------------------------------------------------

data::image_series::sptr series_set::create_image_series()
{
    data::image_series::sptr img_series = std::make_shared<data::image_series>();

    series_set::generate_series_information(img_series);

    image::generate_random_image(img_series, core::type::INT16);

    return img_series;
}

//------------------------------------------------------------------------------

data::model_series::sptr series_set::create_model_series(unsigned char _nb_reconstruction)
{
    data::model_series::sptr model_series = std::make_shared<data::model_series>();

    series_set::generate_series_information(model_series);

    data::model_series::reconstruction_vector_t rec_db;
    for(unsigned char nb = 0 ; nb < _nb_reconstruction ; ++nb)
    {
        data::reconstruction::sptr rec = std::make_shared<data::reconstruction>();
        // Generates reconstruction with a prefixed index in organName "0_Liver", "1_Liver", ...
        series_set::generate_reconstruction(rec, static_cast<int>(nb));

        rec_db.push_back(rec);
    }

    model_series->set_reconstruction_db(rec_db);

    return model_series;
}

//------------------------------------------------------------------------------

data::activity::sptr series_set::create_activity()
{
    data::activity::sptr activity = std::make_shared<data::activity>();

    const std::string config_id = "IdOfTheConfig";
    activity->set_activity_config_id(config_id);

    (*activity)["key1"] = std::make_shared<data::string>("ValueOfKey1");

    return activity;
}

//------------------------------------------------------------------------------

void series_set::generate_reconstruction(data::reconstruction::sptr _rec, int _index)
{
    _rec->set_is_visible(true);
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

    _rec->set_organ_name(organ_name);
    _rec->set_structure_type("Liver");

    data::image::sptr img = std::make_shared<data::image>();
    image::generate_random_image(img, core::type::UINT16);
    _rec->set_image(img);

    data::material::sptr material = std::make_shared<data::material>();
    material->ambient()->red()   = 0.75F;
    material->ambient()->green() = 0.10F;
    material->ambient()->blue()  = 0.56F;
    material->ambient()->alpha() = 0.8F;
    material->diffuse()->red()   = 0.85F;
    material->diffuse()->green() = 0.20F;
    material->diffuse()->blue()  = 0.66F;
    material->diffuse()->alpha() = 0.9F;
    _rec->set_material(material);

    data::mesh::sptr mesh = std::make_shared<data::mesh>();
    mesh::generate_mesh(mesh);

    _rec->set_mesh(mesh);
}

} // namespace sight::utest_data::generator
