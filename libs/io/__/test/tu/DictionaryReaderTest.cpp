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

#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/StructureTraitsHelper.hpp>

#include <io/__/reader/DictionaryReader.hpp>
#include <io/__/reader/IObjectReader.hpp>

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
    data::StructureTraits::sptr expectedSkin = std::make_shared<data::StructureTraits>();
    expectedSkin->setType("Skin");
    expectedSkin->setClass(data::StructureTraits::ENVIRONMENT);
    expectedSkin->setColor(std::make_shared<data::Color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F));
    data::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = data::StructureTraits::BODY;
    expectedSkin->setCategories(skinCat);
    expectedSkin->setAnatomicRegion("Entire_Body");
    expectedSkin->setPropertyCategory("Anat_Struct");
    expectedSkin->setPropertyType("Entire_Body");

    auto structDico = std::make_shared<data::StructureTraitsDictionary>();
    // get data from file.
    auto dictionaryReader = std::make_shared<io::reader::DictionaryReader>();
    dictionaryReader->setObject(structDico);
    dictionaryReader->set_file(m_tmpDictionaryFilePath);
    dictionaryReader->read();

    data::StructureTraits::sptr struct1 = structDico->getStructure("Skin");
    CPPUNIT_ASSERT(struct1);
    CPPUNIT_ASSERT_EQUAL(struct1->getType(), expectedSkin->getType());
    CPPUNIT_ASSERT_EQUAL(struct1->getClass(), expectedSkin->getClass());

    data::Color::sptr color1 = struct1->getColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->red(), expectedSkin->getColor()->red(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->green(), expectedSkin->getColor()->green(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->blue(), expectedSkin->getColor()->blue(), 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->alpha(), expectedSkin->getColor()->alpha(), 0.001);

    CPPUNIT_ASSERT_EQUAL(struct1->getCategories().size(), expectedSkin->getCategories().size());
    CPPUNIT_ASSERT_EQUAL(struct1->getNativeExp(), expectedSkin->getNativeExp());
    CPPUNIT_ASSERT_EQUAL(struct1->getNativeGeometricExp(), expectedSkin->getNativeGeometricExp());
    CPPUNIT_ASSERT_EQUAL(struct1->getAttachmentType(), expectedSkin->getAttachmentType());

    CPPUNIT_ASSERT_EQUAL(struct1->getAnatomicRegion(), expectedSkin->getAnatomicRegion());
    CPPUNIT_ASSERT_EQUAL(struct1->getPropertyCategory(), expectedSkin->getPropertyCategory());
    CPPUNIT_ASSERT_EQUAL(struct1->getPropertyType(), expectedSkin->getPropertyType());
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_2()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::DictionaryReaderTest::generateDictionaryFileWithMissingSemiColon(m_tmpDictionaryFilePath);

    auto structDico = std::make_shared<data::StructureTraitsDictionary>();
    // Get data from file.
    auto dictionaryReader = std::make_shared<io::reader::DictionaryReader>();
    dictionaryReader->setObject(structDico);
    dictionaryReader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), core::exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_3()
{
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "NoDictionary.dic";
    auto structDico = std::make_shared<data::StructureTraitsDictionary>();
    // Get data from file.
    auto dictionaryReader = std::make_shared<io::reader::DictionaryReader>();
    dictionaryReader->setObject(structDico);
    dictionaryReader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), core::exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_4()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::DictionaryReaderTest::generateDictionaryFileWithWrongCategory(m_tmpDictionaryFilePath);

    auto structDico = std::make_shared<data::StructureTraitsDictionary>();
    // Get data from file.
    auto dictionaryReader = std::make_shared<io::reader::DictionaryReader>();
    dictionaryReader->setObject(structDico);
    dictionaryReader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), core::exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_5()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = core::os::temp_dir::shared_directory() / "WrongDictionary.dic";
    sight::io::ut::DictionaryReaderTest::generateDictionaryFileWithWrongClass(m_tmpDictionaryFilePath);

    auto structDico = std::make_shared<data::StructureTraitsDictionary>();
    // Get data from file.
    auto dictionaryReader = std::make_shared<io::reader::DictionaryReader>();
    dictionaryReader->setObject(structDico);
    dictionaryReader->set_file(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), core::exception);
}

//------------------------------------------------------------------------------
void DictionaryReaderTest::generateDictionaryFile(std::filesystem::path dictionaryFile)
{
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());

    file << "Skin;(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;

    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithMissingSemiColon(std::filesystem::path dictionaryFile)
{
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    // Missing ";" after the type Skin.
    file << "Skin(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithWrongCategory(std::filesystem::path dictionaryFile)
{
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    file << "Skin;(255,179,140,100);Boy;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithWrongClass(std::filesystem::path dictionaryFile)
{
    //cspell: ignore Enironment Anat
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    file << "Skin;(255,179,140,100);Body;Enironment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

} // namespace sight::io::ut
