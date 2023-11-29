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

#include "structure_traits_dictionary_test.hpp"

#include <data/structure_traits.hpp>
#include <data/structure_traits_dictionary.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::structure_traits_dictionary_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void structure_traits_dictionary_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void structure_traits_dictionary_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void structure_traits_dictionary_test::test_adding_structure()
{
    data::structure_traits_dictionary::sptr struct_dico = std::make_shared<data::structure_traits_dictionary>();
    data::structure_traits::sptr skin                   = std::make_shared<data::structure_traits>();
    std::string skin_type                               = "Skin";
    skin->set_type(skin_type);
    skin->set_class(data::structure_traits::environment);
    data::color::sptr skin_color = std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->set_color(skin_color);
    data::structure_traits::category_container_t skin_cat(1);
    skin_cat[0] = data::structure_traits::body;
    skin->set_categories(skin_cat);
    CPPUNIT_ASSERT_EQUAL(skin_type, skin->type());
    CPPUNIT_ASSERT_EQUAL(data::structure_traits::environment, skin->get_class());
    CPPUNIT_ASSERT(skin_color == skin->get_color());
    CPPUNIT_ASSERT_NO_THROW(struct_dico->add_structure(skin));

    data::structure_traits::sptr liver = std::make_shared<data::structure_traits>();
    liver->set_type("Liver");
    liver->set_class(data::structure_traits::organ);
    liver->set_color(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::category_container_t liver_cat(1);
    liver_cat[0] = data::structure_traits::abdomen;
    liver->set_categories(liver_cat);
    std::string native_exp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->set_native_exp(native_exp);
    CPPUNIT_ASSERT_EQUAL(native_exp, liver->get_native_exp());
    CPPUNIT_ASSERT_NO_THROW(struct_dico->add_structure(liver));

    data::structure_traits::sptr liver_tumor = std::make_shared<data::structure_traits>();
    liver_tumor->set_type("Liver_Tumor");
    liver_tumor->set_class(data::structure_traits::lesion);
    liver_tumor->set_color(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::category_container_t liver_tumor_cat(1);
    liver_tumor_cat[0] = data::structure_traits::abdomen;
    liver_tumor->set_categories(liver_tumor_cat);
    liver_tumor->set_attachment_type("Liver");
    CPPUNIT_ASSERT_NO_THROW(struct_dico->add_structure(liver_tumor));

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
    CPPUNIT_ASSERT_NO_THROW(struct_dico->add_structure(tumor));

    CPPUNIT_ASSERT(skin == struct_dico->get_structure("Skin"));
    CPPUNIT_ASSERT(liver == struct_dico->get_structure("Liver"));
    CPPUNIT_ASSERT(liver_tumor == struct_dico->get_structure("Liver_Tumor"));

    // check exception is raised if wrong structure
    data::structure_traits::sptr bad_class_structure = std::make_shared<data::structure_traits>();
    bad_class_structure->set_type("my_structure");
    bad_class_structure->set_class(data::structure_traits::organ);
    bad_class_structure->set_color(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::category_container_t struct_cat(1);
    struct_cat[0] = data::structure_traits::abdomen;
    bad_class_structure->set_categories(struct_cat);
    bad_class_structure->set_attachment_type("Liver");
    CPPUNIT_ASSERT_THROW(struct_dico->add_structure(bad_class_structure), core::exception);

    data::structure_traits::sptr bad_attachment_structure = std::make_shared<data::structure_traits>();
    bad_attachment_structure->set_type("my_structure");
    bad_attachment_structure->set_class(data::structure_traits::lesion);
    bad_attachment_structure->set_color(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    bad_attachment_structure->set_categories(struct_cat);
    bad_attachment_structure->set_attachment_type("Unknown");
    CPPUNIT_ASSERT_THROW(struct_dico->add_structure(bad_attachment_structure), core::exception);

    // check exception is raised if structure already exist
    data::structure_traits::sptr liver2 = std::make_shared<data::structure_traits>();
    liver2->set_type("Liver");
    liver2->set_class(data::structure_traits::organ);
    liver2->set_color(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::category_container_t liver2_cat(1);
    liver2_cat[0] = data::structure_traits::abdomen;
    liver2->set_categories(liver2_cat);
    CPPUNIT_ASSERT_THROW(struct_dico->add_structure(liver2), core::exception);
}

} // namespace sight::data::ut
