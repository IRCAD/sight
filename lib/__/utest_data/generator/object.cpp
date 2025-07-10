/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

data::structure_traits_dictionary::sptr object::create_structure_traits_dictionary()
{
    data::structure_traits_dictionary::sptr struct_dico = std::make_shared<data::structure_traits_dictionary>();
    data::structure_traits::sptr skin                   = std::make_shared<data::structure_traits>();
    std::string skin_type                               = "Skin";
    skin->set_type(skin_type);
    skin->set_class(data::structure_traits::environment);
    auto skin_color = std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->set_color(skin_color);
    data::structure_traits::category_container_t skin_cat(1);
    skin_cat[0] = data::structure_traits::body;
    skin->set_categories(skin_cat);
    struct_dico->add_structure(skin);

    data::structure_traits::sptr liver = std::make_shared<data::structure_traits>();
    liver->set_type("Liver");
    liver->set_class(data::structure_traits::organ);
    liver->set_color(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::category_container_t liver_cat(1);
    liver_cat[0] = data::structure_traits::abdomen;
    liver->set_categories(liver_cat);
    std::string native_exp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->set_native_exp(native_exp);
    struct_dico->add_structure(liver);

    data::structure_traits::sptr liver_tumor = std::make_shared<data::structure_traits>();
    liver_tumor->set_type("Liver_Tumor");
    liver_tumor->set_class(data::structure_traits::lesion);
    liver_tumor->set_color(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::category_container_t liver_tumor_cat(1);
    liver_tumor_cat[0] = data::structure_traits::abdomen;
    liver_tumor->set_categories(liver_tumor_cat);
    liver_tumor->set_attachment_type("Liver");
    struct_dico->add_structure(liver_tumor);

    data::structure_traits::sptr tumor = std::make_shared<data::structure_traits>();
    tumor->set_type("Tumor");
    tumor->set_class(data::structure_traits::lesion);
    tumor->set_color(std::make_shared<data::color>(0.0F, 0.0F, 1.0F, 1.0F));
    data::structure_traits::category_container_t tumor_cat(8);
    tumor_cat[0] = data::structure_traits::body;
    tumor_cat[1] = data::structure_traits::head;
    tumor_cat[2] = data::structure_traits::neck;
    tumor_cat[3] = data::structure_traits::thorax;
    tumor_cat[4] = data::structure_traits::abdomen;
    tumor_cat[5] = data::structure_traits::pelvis;
    tumor_cat[6] = data::structure_traits::arm;
    tumor_cat[7] = data::structure_traits::leg;
    tumor->set_categories(tumor_cat);

    data::structure_traits::sptr colon = std::make_shared<data::structure_traits>();
    colon->set_type("Colon");
    colon->set_class(data::structure_traits::organ);
    colon->set_color(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::structure_traits::category_container_t colon_cat(1);
    colon_cat[0] = data::structure_traits::abdomen;
    colon->set_categories(colon_cat);
    struct_dico->add_structure(colon);

    data::structure_traits::sptr caecum = std::make_shared<data::structure_traits>();
    caecum->set_type("Caecum");
    caecum->set_class(data::structure_traits::functional);
    caecum->set_color(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 128.0F / 255.0F, 1.0F));
    data::structure_traits::category_container_t caecum_cat(1);
    caecum_cat[0] = data::structure_traits::abdomen;
    caecum->set_categories(caecum_cat);
    caecum->set_attachment_type("Colon");
    struct_dico->add_structure(caecum);

    return struct_dico;
}

//------------------------------------------------------------------------------

data::structure_traits::sptr object::create_structure_traits()
{
    data::structure_traits::sptr p_structure_traits = std::make_shared<data::structure_traits>();

    const std::string type                                        = "Liver";
    const data::structure_traits::structure_class struct_class    = data::structure_traits::organ;
    const data::structure_traits::roi_expression_t native_exp     = "inter(world(type(Skin)),not(class(Organ)))";
    const data::structure_traits::roi_expression_t native_geo_exp = "halfSpace(world(Left_lung),Z+,true)";

    p_structure_traits->set_type(type);
    p_structure_traits->set_class(struct_class);
    p_structure_traits->set_native_exp(native_exp);
    p_structure_traits->set_native_geometric_exp(native_geo_exp);

    data::color::sptr color = std::make_shared<data::color>();
    color->set_rgba(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
    );
    p_structure_traits->set_color(color);

    data::structure_traits::category_container_t categories(2);
    categories[0] = data::structure_traits::abdomen;
    categories[1] = data::structure_traits::body;
    p_structure_traits->set_categories(categories);

    return p_structure_traits;
}

//------------------------------------------------------------------------------

data::color::sptr object::randomize_color()
{
    data::color::sptr col = std::make_shared<data::color>();
    col->set_rgba(
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F,
        static_cast<float>(safe_rand() % 100) / 100.F
    );
    return col;
}

//------------------------------------------------------------------------------

data::transfer_function::sptr object::create_tf_color(
    const unsigned char _nb_points,
    const double _window,
    const double _level
)
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->set_name("color_t");
    tf->set_background_color(
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

    tf_data->set_level(_level);
    tf_data->set_window(_window);
    tf_data->set_interpolation_mode(data::transfer_function::interpolation_mode::nearest);
    tf_data->set_clamped(false);

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

data::transfer_function::sptr object::create_tf_color()
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->set_background_color(data::transfer_function::color_t(1.0F, 0.3F, 0.6F, 0.1F));
    tf->set_name("color_t");

    auto tf_data = std::make_shared<data::transfer_function_piece>();

    auto& pieces = tf->pieces();
    pieces.push_back(tf_data);

    tf_data->set_interpolation_mode(data::transfer_function::interpolation_mode::nearest);
    tf_data->set_clamped(false);
    tf_data->set_level(900.6);
    tf_data->set_window(-200.02);

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

data::material::sptr object::create_material()
{
    // use the default value PHONG,SURFACE, STANDARD

    data::color::sptr ambient = std::make_shared<data::color>();
    ambient->set_rgba(0.5F, 0.5F, 0.5F, 0.5F);

    data::color::sptr diffuse = std::make_shared<data::color>();
    diffuse->set_rgba(0.8F, 0.2F, 0.5F, 0.4F);

    data::material::sptr material = std::make_shared<data::material>();

    material->set_ambient(data::object::copy(ambient));
    material->set_diffuse(data::object::copy(diffuse));

    return material;
}

//------------------------------------------------------------------------------

data::point::sptr object::generate_point()
{
    std::array<double, 3> coord = {static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300),
                                   static_cast<double>(safe_rand() % 300)
    };
    data::point::sptr point = std::make_shared<data::point>();
    *point = coord;
    return point;
}

//------------------------------------------------------------------------------

data::plane::sptr object::generate_plane()
{
    data::plane::sptr plane = std::make_shared<data::plane>();
    plane->set_value(generate_point(), generate_point(), generate_point());
    plane->set_is_intersection(((safe_rand() % 2) != 0));
    return plane;
}

//------------------------------------------------------------------------------

data::resection::sptr object::generate_resection()
{
    data::resection::sptr resection = std::make_shared<data::resection>();

    resection->set_name("Resection1");
    resection->set_is_safe_part(((safe_rand() % 2) != 0));
    resection->set_is_valid(((safe_rand() % 2) != 0));
    resection->set_is_visible(((safe_rand() % 2) != 0));
    data::reconstruction::sptr rec_input = std::make_shared<data::reconstruction>();

    utest_data::generator::series_set::generate_reconstruction(rec_input);
    data::resection::resection_inputs inputs;
    inputs.push_back(rec_input);
    resection->set_inputs(inputs);

    data::reconstruction::sptr rec_output = std::make_shared<data::reconstruction>();
    utest_data::generator::series_set::generate_reconstruction(rec_output);
    data::resection::resection_outputs outputs;
    outputs.push_back(rec_output);
    resection->set_outputs(outputs);

    data::plane_list::plane_list_t planes;
    planes.push_back(generate_plane());
    planes.push_back(generate_plane());
    data::plane_list::sptr plane_list = std::make_shared<data::plane_list>();
    plane_list->set_planes(planes);

    return resection;
}

//------------------------------------------------------------------------------

data::resection_db::sptr object::generate_resection_db()
{
    data::resection_db::sptr resection_db = std::make_shared<data::resection_db>();
    resection_db->add_resection(generate_resection());
    return resection_db;
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::generator
