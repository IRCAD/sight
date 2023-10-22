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

void structure_traits_dictionary_test::testAddingStructure()
{
    data::structure_traits_dictionary::sptr struct_dico = std::make_shared<data::structure_traits_dictionary>();
    data::structure_traits::sptr skin                   = std::make_shared<data::structure_traits>();
    std::string skin_type                               = "Skin";
    skin->setType(skin_type);
    skin->setClass(data::structure_traits::ENVIRONMENT);
    data::color::sptr skin_color = std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->setColor(skin_color);
    data::structure_traits::CategoryContainer skin_cat(1);
    skin_cat[0] = data::structure_traits::BODY;
    skin->setCategories(skin_cat);
    CPPUNIT_ASSERT_EQUAL(skin_type, skin->getType());
    CPPUNIT_ASSERT_EQUAL(data::structure_traits::ENVIRONMENT, skin->getClass());
    CPPUNIT_ASSERT(skin_color == skin->getColor());
    CPPUNIT_ASSERT_NO_THROW(struct_dico->addStructure(skin));

    data::structure_traits::sptr liver = std::make_shared<data::structure_traits>();
    liver->setType("Liver");
    liver->setClass(data::structure_traits::ORGAN);
    liver->setColor(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer liver_cat(1);
    liver_cat[0] = data::structure_traits::ABDOMEN;
    liver->setCategories(liver_cat);
    std::string native_exp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(native_exp);
    CPPUNIT_ASSERT_EQUAL(native_exp, liver->getNativeExp());
    CPPUNIT_ASSERT_NO_THROW(struct_dico->addStructure(liver));

    data::structure_traits::sptr liver_tumor = std::make_shared<data::structure_traits>();
    liver_tumor->setType("Liver_Tumor");
    liver_tumor->setClass(data::structure_traits::LESION);
    liver_tumor->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::CategoryContainer liver_tumor_cat(1);
    liver_tumor_cat[0] = data::structure_traits::ABDOMEN;
    liver_tumor->setCategories(liver_tumor_cat);
    liver_tumor->set_attachment_type("Liver");
    CPPUNIT_ASSERT_NO_THROW(struct_dico->addStructure(liver_tumor));

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
    CPPUNIT_ASSERT_NO_THROW(struct_dico->addStructure(tumor));

    CPPUNIT_ASSERT(skin == struct_dico->getStructure("Skin"));
    CPPUNIT_ASSERT(liver == struct_dico->getStructure("Liver"));
    CPPUNIT_ASSERT(liver_tumor == struct_dico->getStructure("Liver_Tumor"));

    // check exception is raised if wrong structure
    data::structure_traits::sptr bad_class_structure = std::make_shared<data::structure_traits>();
    bad_class_structure->setType("my_structure");
    bad_class_structure->setClass(data::structure_traits::ORGAN);
    bad_class_structure->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::CategoryContainer struct_cat(1);
    struct_cat[0] = data::structure_traits::ABDOMEN;
    bad_class_structure->setCategories(struct_cat);
    bad_class_structure->set_attachment_type("Liver");
    CPPUNIT_ASSERT_THROW(struct_dico->addStructure(bad_class_structure), core::exception);

    data::structure_traits::sptr bad_attachment_structure = std::make_shared<data::structure_traits>();
    bad_attachment_structure->setType("my_structure");
    bad_attachment_structure->setClass(data::structure_traits::LESION);
    bad_attachment_structure->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    bad_attachment_structure->setCategories(struct_cat);
    bad_attachment_structure->set_attachment_type("Unknown");
    CPPUNIT_ASSERT_THROW(struct_dico->addStructure(bad_attachment_structure), core::exception);

    // check exception is raised if structure already exist
    data::structure_traits::sptr liver2 = std::make_shared<data::structure_traits>();
    liver2->setType("Liver");
    liver2->setClass(data::structure_traits::ORGAN);
    liver2->setColor(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer liver2_cat(1);
    liver2_cat[0] = data::structure_traits::ABDOMEN;
    liver2->setCategories(liver2_cat);
    CPPUNIT_ASSERT_THROW(struct_dico->addStructure(liver2), core::exception);
}

} // namespace sight::data::ut
