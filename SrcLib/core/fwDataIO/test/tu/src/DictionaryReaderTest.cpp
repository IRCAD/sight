/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <fwTools/System.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/StructureTraitsHelper.hpp>

#include <fwDataIO/reader/DictionaryReader.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

#include "DictionaryReaderTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataIO::ut::DictionaryReaderTest );

namespace fwDataIO
{
namespace ut
{

//------------------------------------------------------------------------------

void DictionaryReaderTest::setUp()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = ::fwTools::System::getTemporaryFolder() / "Dictionary.dic";

    this->generateDictionaryFile(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT(::boost::filesystem::exists(m_tmpDictionaryFilePath));
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::tearDown()
{
    // Clean up after the test run.
    bool suppr = false;
    if( ::boost::filesystem::exists(m_tmpDictionaryFilePath))
    {
        suppr = ::boost::filesystem::remove(m_tmpDictionaryFilePath);
        CPPUNIT_ASSERT(suppr);
    }
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_1()
{
    // Expected data
    ::fwData::StructureTraits::NewSptr expectedSkin;
    expectedSkin->setType("Skin");
    expectedSkin->setClass(::fwData::StructureTraits::ENVIRONMENT);
    expectedSkin->setColor(::fwData::Color::New(255.0f/255.0f, 179.0f/255.0f, 140.0f/255.0f, 100.0/100.0f));
    ::fwData::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = ::fwData::StructureTraits::BODY;
    expectedSkin->setCategories(skinCat);
    expectedSkin->setAnatomicRegion("Entire_Body");
    expectedSkin->setPropertyCategory("Anat_Struct");
    expectedSkin->setPropertyType("Entire_Body");

    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    // get data from file.
    ::fwDataIO::reader::DictionaryReader::NewSptr dictionaryReader;
    dictionaryReader->setObject(structDico);
    dictionaryReader->setFile(m_tmpDictionaryFilePath);
    dictionaryReader->read();

    ::fwData::StructureTraits::sptr struct1 = structDico->getStructure("Skin");
    CPPUNIT_ASSERT(struct1);
    CPPUNIT_ASSERT_EQUAL(struct1->getType(), expectedSkin->getType());
    CPPUNIT_ASSERT_EQUAL(struct1->getClass(), expectedSkin->getClass());

    ::fwData::Color::sptr color1 = struct1->getColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->red(), expectedSkin->getColor()->red(),0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->green(), expectedSkin->getColor()->green(),0.001);
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
    m_tmpDictionaryFilePath = ::fwTools::System::getTemporaryFolder() / "WrongDictionary.dic";
    this->generateDictionaryFileWithMissingSemiColon(m_tmpDictionaryFilePath);

    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    // Get data from file.
    ::fwDataIO::reader::DictionaryReader::NewSptr dictionaryReader;
    dictionaryReader->setObject(structDico);
    dictionaryReader->setFile(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), ::fwCore::Exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_3()
{
    m_tmpDictionaryFilePath = ::fwTools::System::getTemporaryFolder() / "NoDictionary.dic";
    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    // Get data from file.
    ::fwDataIO::reader::DictionaryReader::NewSptr dictionaryReader;
    dictionaryReader->setObject(structDico);
    dictionaryReader->setFile(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), ::fwCore::Exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_4()
{
    // Set up context before running a test.
    m_tmpDictionaryFilePath = ::fwTools::System::getTemporaryFolder() / "WrongDictionary.dic";
    this->generateDictionaryFileWithWrongCategory(m_tmpDictionaryFilePath);

    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    // Get data from file.
    ::fwDataIO::reader::DictionaryReader::NewSptr dictionaryReader;
    dictionaryReader->setObject(structDico);
    dictionaryReader->setFile(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), ::fwCore::Exception);
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::test_5()
{

    // Set up context before running a test.
    m_tmpDictionaryFilePath = ::fwTools::System::getTemporaryFolder() / "WrongDictionary.dic";
    this->generateDictionaryFileWithWrongClass(m_tmpDictionaryFilePath);

    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    // Get data from file.
    ::fwDataIO::reader::DictionaryReader::NewSptr dictionaryReader;
    dictionaryReader->setObject(structDico);
    dictionaryReader->setFile(m_tmpDictionaryFilePath);

    CPPUNIT_ASSERT_THROW(dictionaryReader->read(), ::fwCore::Exception);
}
//------------------------------------------------------------------------------
void DictionaryReaderTest::generateDictionaryFile(::boost::filesystem::path dictionaryFile)
{
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());

    file<<"Skin;(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;

    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithMissingSemiColon(::boost::filesystem::path dictionaryFile)
{
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    // Missing ";" after the type Skin.
    file<<"Skin(255,179,140,100);Body;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithWrongCategory(::boost::filesystem::path dictionaryFile)
{
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    file<<"Skin;(255,179,140,100);Boy;Environment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

void DictionaryReaderTest::generateDictionaryFileWithWrongClass(::boost::filesystem::path dictionaryFile)
{
    std::fstream file;
    file.open(dictionaryFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());
    file<<"Skin;(255,179,140,100);Body;Enironment;;;;Entire_Body;Anat_Struct;Entire_Body" << std::endl;
    file.close();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataIO
