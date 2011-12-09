/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/StructureTraits.hpp>

#include "StructureTraitsDictionaryTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StructureTraitsDictionaryTest );

void StructureTraitsDictionaryTest::setUp()
{
    // Set up context before running a test.

}
void StructureTraitsDictionaryTest::tearDown()
{
    // Clean up after the test run.
}

void StructureTraitsDictionaryTest::testAddingStructure()
{
    ::fwData::StructureTraitsDictionary::NewSptr structDico;
    ::fwData::StructureTraits::NewSptr skin;
    std::string skinType = "Skin";
    skin->setType(skinType);
    skin->setClass(::fwData::StructureTraits::ENVIRONMENT);
    ::fwData::Color::sptr skinColor = ::fwData::Color::New(255.0f/255.0f, 179.0f/255.0f, 140.0f/255.0f, 1.0f);
    skin->setColor(skinColor);
    ::fwData::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = ::fwData::StructureTraits::BODY;
    skin->setCategories(skinCat);
    CPPUNIT_ASSERT_EQUAL(skinType, skin->getType());
    CPPUNIT_ASSERT_EQUAL(::fwData::StructureTraits::ENVIRONMENT, skin->getClass());
    CPPUNIT_ASSERT(skinColor == skin->getColor());
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(skin));

    ::fwData::StructureTraits::NewSptr liver;
    liver->setType("Liver");
    liver->setClass(::fwData::StructureTraits::ORGAN);
    liver->setColor(::fwData::Color::New(204.0f/255.0f, 51.0f/255.0f, 51.0f/255.0f, 1.0));
    ::fwData::StructureTraits::CategoryContainer liverCat(1);
    liverCat[0] = ::fwData::StructureTraits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    CPPUNIT_ASSERT_EQUAL(nativeExp, liver->getNativeExp());
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(liver));

    ::fwData::StructureTraits::NewSptr liverTumor;
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(::fwData::StructureTraits::LESION);
    liverTumor->setColor(::fwData::Color::New(0.0f, 179.0f/255.0f, 0.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = ::fwData::StructureTraits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(liverTumor));

    ::fwData::StructureTraits::NewSptr tumor;
    tumor->setType("Tumor");
    tumor->setClass(::fwData::StructureTraits::LESION);
    tumor->setColor(::fwData::Color::New(0.0f, 0.0f, 1.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer tumorCat(8);
    tumorCat[0] = ::fwData::StructureTraits::BODY;
    tumorCat[1] = ::fwData::StructureTraits::HEAD;
    tumorCat[2] = ::fwData::StructureTraits::NECK;
    tumorCat[3] = ::fwData::StructureTraits::THORAX;
    tumorCat[4] = ::fwData::StructureTraits::ABDOMEN;
    tumorCat[5] = ::fwData::StructureTraits::PELVIS;
    tumorCat[6] = ::fwData::StructureTraits::ARM;
    tumorCat[7] = ::fwData::StructureTraits::LEG;
    tumor->setCategories(tumorCat);
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(tumor));


    CPPUNIT_ASSERT(skin == structDico->getStructure("Skin"));
    CPPUNIT_ASSERT(liver == structDico->getStructure("Liver"));
    CPPUNIT_ASSERT(liverTumor == structDico->getStructure("Liver_Tumor"));

    // check exception is raised if wrong structure
    ::fwData::StructureTraits::NewSptr badClassStructure;
    badClassStructure->setType("my_structure");
    badClassStructure->setClass(::fwData::StructureTraits::ORGAN);
    badClassStructure->setColor(::fwData::Color::New(0.0f, 179.0f/255.0f, 0.0f, 1.0f));
    ::fwData::StructureTraits::CategoryContainer structCat(1);
    structCat[0] = ::fwData::StructureTraits::ABDOMEN;
    badClassStructure->setCategories(structCat);
    badClassStructure->setAttachmentType("Liver");
    CPPUNIT_ASSERT_THROW(structDico->addStructure(badClassStructure), ::fwCore::Exception);

    ::fwData::StructureTraits::NewSptr badAttachmentStructure;
    badAttachmentStructure->setType("my_structure");
    badAttachmentStructure->setClass(::fwData::StructureTraits::LESION);
    badAttachmentStructure->setColor(::fwData::Color::New(0.0f, 179.0f/255.0f, 0.0f, 1.0f));
    badAttachmentStructure->setCategories(structCat);
    badAttachmentStructure->setAttachmentType("Unknown");
    CPPUNIT_ASSERT_THROW(structDico->addStructure(badAttachmentStructure), ::fwCore::Exception);

    // check exception is raised if structure already exist
    ::fwData::StructureTraits::NewSptr liver2;
    liver2->setType("Liver");
    liver2->setClass(::fwData::StructureTraits::ORGAN);
    liver2->setColor(::fwData::Color::New(204.0f/255.0f, 51.0f/255.0f, 51.0f/255.0f, 1.0));
    ::fwData::StructureTraits::CategoryContainer liver2Cat(1);
    liver2Cat[0] = ::fwData::StructureTraits::ABDOMEN;
    liver2->setCategories(liver2Cat);
    CPPUNIT_ASSERT_THROW(structDico->addStructure(liver2), ::fwCore::Exception);

}
