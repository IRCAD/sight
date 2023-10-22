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

#include "utest_data/generator/object.hpp"

#include "utest_data/generator/image.hpp"
#include "utest_data/generator/mesh.hpp"
#include "utest_data/generator/series_set.hpp"

#include <core/tools/random/generator.hpp>

#include <data/integer.hpp>
#include <data/plane.hpp>
#include <data/resection.hpp>
#include <data/resection_db.hpp>
#include <data/string.hpp>

#include <cmath>
#include <filesystem>

namespace sight::utest_data::generator
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

data::structure_traits_dictionary::sptr object::createStructureTraitsDictionary()
{
    data::structure_traits_dictionary::sptr struct_dico = std::make_shared<data::structure_traits_dictionary>();
    data::structure_traits::sptr skin                   = std::make_shared<data::structure_traits>();
    std::string skin_type                               = "Skin";
    skin->setType(skin_type);
    skin->setClass(data::structure_traits::ENVIRONMENT);
    auto skin_color = std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->setColor(skin_color);
    data::structure_traits::CategoryContainer skin_cat(1);
    skin_cat[0] = data::structure_traits::BODY;
    skin->setCategories(skin_cat);
    struct_dico->addStructure(skin);

    data::structure_traits::sptr liver = std::make_shared<data::structure_traits>();
    liver->setType("Liver");
    liver->setClass(data::structure_traits::ORGAN);
    liver->setColor(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer liver_cat(1);
    liver_cat[0] = data::structure_traits::ABDOMEN;
    liver->setCategories(liver_cat);
    std::string native_exp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(native_exp);
    struct_dico->addStructure(liver);

    data::structure_traits::sptr liver_tumor = std::make_shared<data::structure_traits>();
    liver_tumor->setType("Liver_Tumor");
    liver_tumor->setClass(data::structure_traits::LESION);
    liver_tumor->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::CategoryContainer liver_tumor_cat(1);
    liver_tumor_cat[0] = data::structure_traits::ABDOMEN;
    liver_tumor->setCategories(liver_tumor_cat);
    liver_tumor->set_attachment_type("Liver");
    struct_dico->addStructure(liver_tumor);

    data::structure_traits::sptr tumor = std::make_shared<data::structure_traits>();
    tumor->setType("Tumor");
    tumor->setClass(data::structure_traits::LESION);
    tumor->setColor(std::make_shared<data::color>(0.0F, 0.0F, 1.0F, 1.0F));
    data::structure_traits::CategoryContainer tumor_cat(8);
    tumor_cat[0] = data::structure_traits::BODY;
    tumor_cat[1] = data::structure_traits::HEAD;
    tumor_cat[2] = data::structure_traits::NECK;
    tumor_cat[3] = data::structure_traits::THORAX;
    tumor_cat[4] = data::structure_traits::ABDOMEN;
    tumor_cat[5] = data::structure_traits::PELVIS;
    tumor_cat[6] = data::structure_traits::ARM;
    tumor_cat[7] = data::structure_traits::LEG;
    tumor->setCategories(tumor_cat);

    data::structure_traits::sptr colon = std::make_shared<data::structure_traits>();
    colon->setType("Colon");
    colon->setClass(data::structure_traits::ORGAN);
    colon->setColor(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer colon_cat(1);
    colon_cat[0] = data::structure_traits::ABDOMEN;
    colon->setCategories(colon_cat);
    struct_dico->addStructure(colon);

    data::structure_traits::sptr caecum = std::make_shared<data::structure_traits>();
    caecum->setType("Caecum");
    caecum->setClass(data::structure_traits::FUNCTIONAL);
    caecum->setColor(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer caecum_cat(1);
    caecum_cat[0] = data::structure_traits::ABDOMEN;
    caecum->setCategories(caecum_cat);
    caecum->set_attachment_type("Colon");
    struct_dico->addStructure(caecum);

    return struct_dico;
}

//------------------------------------------------------------------------------

data::structure_traits::sptr object::createStructureTraits()
{
    data::structure_traits::sptr p_structure_traits = std::make_shared<data::structure_traits>();

    const std::string type                                     = "Liver";
    const data::structure_traits::StructureClass struct_class  = data::structure_traits::ORGAN;
    const data::structure_traits::ROIExpression native_exp     = "inter(world(type(Skin)),not(class(Organ)))";
    const data::structure_traits::ROIExpression native_geo_exp = "halfSpace(world(Left_lung),Z+,true)";

    p_structure_traits->setType(type);
    p_structure_traits->setClass(struct_class);
    p_structure_traits->setNativeExp(native_exp);
    p_structure_traits->setNativeGeometricExp(native_geo_exp);

    data::color::sptr color = std::make_shared<data::color>();
    color->setRGBA(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
    );
    p_structure_traits->setColor(color);

    data::structure_traits::CategoryContainer categories(2);
    categories[0] = data::structure_traits::ABDOMEN;
    categories[1] = data::structure_traits::BODY;
    p_structure_traits->setCategories(categories);

    return p_structure_traits;
}

//------------------------------------------------------------------------------

data::color::sptr object::randomizeColor()
{
    data::color::sptr col = std::make_shared<data::color>();
    col->setRGBA(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
    );
    return col;
}

//------------------------------------------------------------------------------

data::transfer_function::sptr object::createTFColor(
    const unsigned char _nb_points,
    const double _window,
    const double _level
)
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->setName("color_t");
    tf->setBackgroundColor(
        data::transfer_function::color_t(
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F
        )
    );

    auto tf_data = std::make_shared<data::transfer_function_piece>();

    auto& pieces = tf->pieces();
    pieces.push_back(tf_data);

    tf_data->setLevel(_level);
    tf_data->setWindow(_window);
    tf_data->setInterpolationMode(data::transfer_function::InterpolationMode::NEAREST);
    tf_data->setClamped(false);

    for(unsigned char nb = 0 ; nb < _nb_points ; ++nb)
    {
        double value = safe_rand() % 100 - _level;
        (*tf_data)[value] = data::transfer_function::color_t(
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F,
            static_cast<float>(safe_rand() % 100) / 100.F
        );
    }

    data::string::sptr my_string = std::make_shared<data::string>("fieldStringValue");
    tf->set_field("fieldStringKey", my_string);

    return tf;
}

//------------------------------------------------------------------------------

data::transfer_function::sptr object::createTFColor()
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->setBackgroundColor(data::transfer_function::color_t(1.0F, 0.3F, 0.6F, 0.1F));
    tf->setName("color_t");

    auto tf_data = std::make_shared<data::transfer_function_piece>();

    auto& pieces = tf->pieces();
    pieces.push_back(tf_data);

    tf_data->setInterpolationMode(data::transfer_function::InterpolationMode::NEAREST);
    tf_data->setClamped(false);
    tf_data->setLevel(900.6);
    tf_data->setWindow(-200.02);

    tf_data->insert({-40.33, data::transfer_function::color_t(0.9F, 0.2F, 0.3F, 0.4F)});
    tf_data->insert({3, data::transfer_function::color_t(0.1F, 0.2F, 0.9F, 0.4F)}); // Invert point 3 <=> -0.2,
    // for tests
    tf_data->insert({-0.2, data::transfer_function::color_t(0.1F, 0.9F, 0.3F, 0.4F)});
    tf_data->insert({150, data::transfer_function::color_t(0.1F, 0.2F, 0.3F, 0.9F)});

    data::string::sptr my_string = std::make_shared<data::string>("fieldStringValue");
    tf->set_field("fieldStringKey", my_string);

    return tf;
}

//------------------------------------------------------------------------------

data::material::sptr object::createMaterial()
{
    // use the default value PHONG,SURFACE, STANDARD

    data::color::sptr ambient = std::make_shared<data::color>();
    ambient->setRGBA(0.5F, 0.5F, 0.5F, 0.5F);

    data::color::sptr diffuse = std::make_shared<data::color>();
    diffuse->setRGBA(0.8F, 0.2F, 0.5F, 0.4F);

    data::material::sptr material = std::make_shared<data::material>();

    material->setAmbient(data::object::copy(ambient));
    material->setDiffuse(data::object::copy(diffuse));

    return material;
}

//------------------------------------------------------------------------------

data::point::sptr object::generatePoint()
{
    std::array<double, 3> coord = {static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300)
    };
    data::point::sptr point = std::make_shared<data::point>();
    point->setCoord(coord);
    return point;
}

//------------------------------------------------------------------------------

data::plane::sptr object::generatePlane()
{
    data::plane::sptr plane = std::make_shared<data::plane>();
    plane->setValue(generatePoint(), generatePoint(), generatePoint());
    plane->setIsIntersection(((safe_rand() % 2) != 0));
    return plane;
}

//------------------------------------------------------------------------------

data::resection::sptr object::generateResection()
{
    data::resection::sptr resection = std::make_shared<data::resection>();

    resection->setName("Resection1");
    resection->setIsSafePart(((safe_rand() % 2) != 0));
    resection->setIsValid(((safe_rand() % 2) != 0));
    resection->setIsVisible(((safe_rand() % 2) != 0));
    data::reconstruction::sptr rec_input = std::make_shared<data::reconstruction>();

    utest_data::generator::series_set::generateReconstruction(rec_input);
    data::resection::ResectionInputs inputs;
    inputs.push_back(rec_input);
    resection->SetInputs(inputs);

    data::reconstruction::sptr rec_output = std::make_shared<data::reconstruction>();
    utest_data::generator::series_set::generateReconstruction(rec_output);
    data::resection::ResectionOutputs outputs;
    outputs.push_back(rec_output);
    resection->setOutputs(outputs);

    data::plane_list::PlaneListContainer planes;
    planes.push_back(generatePlane());
    planes.push_back(generatePlane());
    data::plane_list::sptr plane_list = std::make_shared<data::plane_list>();
    plane_list->setPlanes(planes);

    return resection;
}

//------------------------------------------------------------------------------

data::resection_db::sptr object::generateResectionDB()
{
    data::resection_db::sptr resection_db = std::make_shared<data::resection_db>();
    resection_db->addResection(generateResection());
    return resection_db;
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::generator
