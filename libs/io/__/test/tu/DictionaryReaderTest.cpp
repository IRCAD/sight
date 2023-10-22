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
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::DictionaryReaderTest);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void DictionaryReaderTest::setUp()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "Dictionary.dic";

    sight::io::ut::DictionaryReaderTest::generateDictionaryFile(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT(std::filesystem::exists(m_tmpDictionaryFilePath));
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::tearDown()
{
    // Clean up after the test run.
    if(std::filesystem::exists(m_tmpDictionaryFilePath))
    {
        bool removed = false;
        removed = std::filesystem::remove(m_tmpDictionaryFilePath);
        CPPUNIT_ASSERT(removed);
    }
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_1()
{
    // Expected data
    data::structure_traits::sptr expected_skin = std::make_shared<data::structure_traits>();
    expected_skin->setType("Skin");
    expected_skin->setClass(data::structure_traits::ENVIRONMENT);
    expected_skin->setColor(std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer skin_cat(1);
    skin_cat[0] = data::structure_traits::BODY;
    expected_skin->setCategories(skin_cat);
    expected_skin->setAnatomicRegion("Entire_Body");
    expected_skin->setPropertyCategory("Anat_Struct");
    expected_skin->set_property_type("Entire_Body");

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmpDictionaryFilePath);
    dictionary_reader->read();

    data::structure_traits::sptr struct1 = struct_dico->getStructure("Skin");
    CPPUNIT_ASSERT(struct1);
    CPPUNIT_ASSERT_EQUAL(struct1->getType(), expected_skin->getType());
    CPPUNIT_ASSERT_EQUAL(struct1->getClass(), expected_skin->getClass());

    data::color::sptr color1 = struct1->getColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->red(), expected_skin->getColor()->red(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->green(), expected_skin->getColor()->green(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->blue(), expected_skin->getColor()->blue(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->alpha(), expected_skin->getColor()->alpha(), 0.001);

    CPPUNIT_ASSERT_EQUAL(struct1->getCategories().size(), expected_skin->getCategories().size());
    CPPUNIT_ASSERT_EQUAL(struct1->getNativeExp(), expected_skin->getNativeExp());
    CPPUNIT_ASSERT_EQUAL(struct1->getNativeGeometricExp(), expected_skin->getNativeGeometricExp());
    CPPUNIT_ASSERT_EQUAL(struct1->get_attachment_type(), expected_skin->get_attachment_type());

    CPPUNIT_ASSERT_EQUAL(struct1->getAnatomicRegion(), expected_skin->getAnatomicRegion());
    CPPUNIT_ASSERT_EQUAL(struct1->getPropertyCategory(), expected_skin->getPropertyCategory());
    CPPUNIT_ASSERT_EQUAL(struct1->get_property_type(), expected_skin->get_property_type());
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_2()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::DictionaryReaderTest::generateDictionaryFileWithMissingSemiColon(m_tmpDictionaryFilePath);

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_3()
{
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "NoDictionary.dic";
    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_4()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::DictionaryReaderTest::generateDictionaryFileWithWrongCategory(m_tmpDictionaryFilePath);

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_5()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::DictionaryReaderTest::generateDictionaryFileWithWrongClass(m_tmpDictionaryFilePath);

    auto struct_dico = std::make_shared<data::structure_traits_dictionary>();
    // Get data from file.
    auto dictionary_reader = std::make_shared<io::reader::dictionary_reader>();
    dictionary_reader->set_object(struct_dico);
    dictionary_reader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionary_reader->read(), core::exception);
}

//------------------------------------------------------------------------------
void DictionaryReaderTest::generateDictionaryFile(std::filesystem::path _dictionary_file)
{
    std::fstream file;
    file.open(_dictionary_file.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());

    file << "Skin;(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;

    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithMissingSemiColon(std::filesystem::path _dictionary_file)
{
    std::fstream file;
    file.open(_dictionary_file.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    // Missing ";" after the type Skin.
    file << "Skin(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithWrongCategory(std::filesystem::path _dictionary_file)
{
    std::fstream file;
    file.open(_dictionary_file.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    file << "Skin;(255,179,140,100);Boy;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithWrongClass(std::filesystem::path _dictionary_file)
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
