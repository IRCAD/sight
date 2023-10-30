/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "DictionaryReaderTest.hpp"

#include <core/os/temp_path.hpp>

#include <data/structure_traits.hpp>
#include <data/structure_traits_dictionary.hpp>
#include <data/structure_traits_helper.hpp>

#include <io/__/reader/dictionary_reader.hpp>
#include <io/__/reader/object_reader.hpp>

#include <fstream>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::dictionary_reader_test);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void dictionary_reader_test::setUp()
{
    // Set up context before running a test.
    m_tmp_dictionary_file_path = core::os::temp_dir::shared_directory() / "Dictionary.dic";

    sight::io::ut::dictionary_reader_test::generate_dictionary_file(m_tmp_dictionary_file_path);

    CPPUNIT_ASSERT(std::filesystem::exists(m_tmp_dictionary_file_path));
}

//------------------------------------------------------------------------------

void dictionary_reader_test::tearDown()
{
    // Clean up after the test run.
    if(std::filesystem::exists(m_tmp_dictionary_file_path))
    {
        bool removed = false;
        removed = std::filesystem::remove(m_tmp_dictionary_file_path);
        CPPUNIT_ASSERT(removed);
    }
}

//------------------------------------------------------------------------------

void dictionary_reader_test::test_1()
{
    // Expected data
    data::structure_traits::sptr expected_skin = std::make_shared<data::structure_traits>();
    expected_skin->set_type("Skin");
    expected_skin->set_class(data::structure_traits::environment);
    expected_skin->set_color(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F));
    data::structure_traits::category_container_t skin_cat(1);
    skin_cat[0] = data::structure_traits::body;
    expected_skin->set_categories(skin_cat);
    expected_skin->set_anatomic_region("Entire_Body");
    expected_skin->set_property_category("Anat_Struct");
    expected_skin->set_property_type("Entire_Body");

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmp_dictionary_file_path);
    dictionary_reader->read();

    data::structure_traits::sptr struct1 = struct_dico->get_structure("Skin");
    CPPUNIT_ASSERT(struct1);
    CPPUNIT_ASSERT_EQUAL(struct1->type(), expected_skin->type());
    CPPUNIT_ASSERT_EQUAL(struct1->get_class(), expected_skin->get_class());

    data::color::sptr color1 = struct1->get_color();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->red(), expected_skin->get_color()->red(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->green(), expected_skin->get_color()->green(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->blue(), expected_skin->get_color()->blue(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->alpha(), expected_skin->get_color()->alpha(), 0.001);

    CPPUNIT_ASSERT_EQUAL(struct1->get_categories().size(), expected_skin->get_categories().size());
    CPPUNIT_ASSERT_EQUAL(struct1->get_native_exp(), expected_skin->get_native_exp());
    CPPUNIT_ASSERT_EQUAL(struct1->get_native_geometric_exp(), expected_skin->get_native_geometric_exp());
    CPPUNIT_ASSERT_EQUAL(struct1->get_attachment_type(), expected_skin->get_attachment_type());

    CPPUNIT_ASSERT_EQUAL(struct1->get_anatomic_region(), expected_skin->get_anatomic_region());
    CPPUNIT_ASSERT_EQUAL(struct1->get_property_category(), expected_skin->get_property_category());
    CPPUNIT_ASSERT_EQUAL(struct1->get_property_type(), expected_skin->get_property_type());
}

//------------------------------------------------------------------------------

void dictionary_reader_test::test_2()
{
    // Set up context before running a test.
    m_tmp_dictionary_file_path = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::dictionary_reader_test::generate_dictionary_file_with_missing_semi_colon(m_tmp_dictionary_file_path);

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmp_dictionary_file_path);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------

void dictionary_reader_test::test_3()
{
    m_tmp_dictionary_file_path = core::os::temp_dir::shared_directory() / "NoDictionary.dic";
    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmp_dictionary_file_path);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------

void dictionary_reader_test::test_4()
{
    // Set up context before running a test.
    m_tmp_dictionary_file_path = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::dictionary_reader_test::generate_dictionary_file_with_wrong_category(m_tmp_dictionary_file_path);

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmp_dictionary_file_path);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------

void dictionary_reader_test::test_5()
{
    // Set up context before running a test.
    m_tmp_dictionary_file_path = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::dictionary_reader_test::generate_dictionary_file_with_wrong_class(m_tmp_dictionary_file_path);

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmp_dictionary_file_path);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------
void dictionary_reader_test::generate_dictionary_file(std::filesystem::path _dictionary_file)
{
    std::fstream file;
    file.open(_dictionary_file.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());

    file << "Skin;(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;

    file.close();
}

//------------------------------------------------------------------------------

void dictionary_reader_test::generate_dictionary_file_with_missing_semi_colon(std::filesystem::path _dictionary_file)
{
    std::fstream file;
    file.open(_dictionary_file.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    // Missing ";" after the type Skin.
    file << "Skin(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void dictionary_reader_test::generate_dictionary_file_with_wrong_category(std::filesystem::path _dictionary_file)
{
    std::fstream file;
    file.open(_dictionary_file.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    file << "Skin;(255,179,140,100);Boy;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void dictionary_reader_test::generate_dictionary_file_with_wrong_class(std::filesystem::path _dictionary_file)
{
    //cspell: ignore Enironment Anat
    std::fstream file;
    file.open(_dictionary_file.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    file << "Skin;(255,179,140,100);Body;Enironment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

} // namespace sight::io::ut
