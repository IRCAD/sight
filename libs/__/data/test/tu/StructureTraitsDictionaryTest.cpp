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

#include "StructureTraitsDictionaryTest.hpp"

#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::StructureTraitsDictionaryTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void StructureTraitsDictionaryTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void StructureTraitsDictionaryTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void StructureTraitsDictionaryTest::testAddingStructure()
{
    data::StructureTraitsDictionary::sptr structDico = std::make_shared<data::StructureTraitsDictionary>();
    data::StructureTraits::sptr skin                 = std::make_shared<data::StructureTraits>();
    std::string skinType                             = "Skin";
    skin->setType(skinType);
    skin->setClass(data::StructureTraits::ENVIRONMENT);
    data::Color::sptr skinColor = std::make_shared<data::Color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->setColor(skinColor);
    data::StructureTraits::CategoryContainer skinCat(1);
    skinCat[0] = data::StructureTraits::BODY;
    skin->setCategories(skinCat);
    CPPUNIT_ASSERT_EQUAL(skinType, skin->getType());
    CPPUNIT_ASSERT_EQUAL(data::StructureTraits::ENVIRONMENT, skin->getClass());
    CPPUNIT_ASSERT(skinColor == skin->getColor());
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(skin));

    data::StructureTraits::sptr liver = std::make_shared<data::StructureTraits>();
    liver->setType("Liver");
    liver->setClass(data::StructureTraits::ORGAN);
    liver->setColor(std::make_shared<data::Color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::StructureTraits::CategoryContainer liverCat(1);
    liverCat[0] = data::StructureTraits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    CPPUNIT_ASSERT_EQUAL(nativeExp, liver->getNativeExp());
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(liver));

    data::StructureTraits::sptr liverTumor = std::make_shared<data::StructureTraits>();
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(data::StructureTraits::LESION);
    liverTumor->setColor(std::make_shared<data::Color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::StructureTraits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = data::StructureTraits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(liverTumor));

    data::StructureTraits::sptr tumor = std::make_shared<data::StructureTraits>();
    tumor->setType("Tumor");
    tumor->setClass(data::StructureTraits::LESION);
    tumor->setColor(std::make_shared<data::Color>(0.0F, 0.0F, 1.0F, 1.0F));
    data::StructureTraits::CategoryContainer tumorCat(8);
    tumorCat[0] = data::StructureTraits::BODY;
    tumorCat[1] = data::StructureTraits::HEAD;
    tumorCat[2] = data::StructureTraits::NECK;
    tumorCat[3] = data::StructureTraits::THORAX;
    tumorCat[4] = data::StructureTraits::ABDOMEN;
    tumorCat[5] = data::StructureTraits::PELVIS;
    tumorCat[6] = data::StructureTraits::ARM;
    tumorCat[7] = data::StructureTraits::LEG;
    tumor->setCategories(tumorCat);
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(tumor));

    CPPUNIT_ASSERT(skin == structDico->getStructure("Skin"));
    CPPUNIT_ASSERT(liver == structDico->getStructure("Liver"));
    CPPUNIT_ASSERT(liverTumor == structDico->getStructure("Liver_Tumor"));

    // check exception is raised if wrong structure
    data::StructureTraits::sptr badClassStructure = std::make_shared<data::StructureTraits>();
    badClassStructure->setType("my_structure");
    badClassStructure->setClass(data::StructureTraits::ORGAN);
    badClassStructure->setColor(std::make_shared<data::Color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::StructureTraits::CategoryContainer structCat(1);
    structCat[0] = data::StructureTraits::ABDOMEN;
    badClassStructure->setCategories(structCat);
    badClassStructure->setAttachmentType("Liver");
    CPPUNIT_ASSERT_THROW(structDico->addStructure(badClassStructure), core::exception);

    data::StructureTraits::sptr badAttachmentStructure = std::make_shared<data::StructureTraits>();
    badAttachmentStructure->setType("my_structure");
    badAttachmentStructure->setClass(data::StructureTraits::LESION);
    badAttachmentStructure->setColor(std::make_shared<data::Color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    badAttachmentStructure->setCategories(structCat);
    badAttachmentStructure->setAttachmentType("Unknown");
    CPPUNIT_ASSERT_THROW(structDico->addStructure(badAttachmentStructure), core::exception);

    // check exception is raised if structure already exist
    data::StructureTraits::sptr liver2 = std::make_shared<data::StructureTraits>();
    liver2->setType("Liver");
    liver2->setClass(data::StructureTraits::ORGAN);
    liver2->setColor(std::make_shared<data::Color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::StructureTraits::CategoryContainer liver2Cat(1);
    liver2Cat[0] = data::StructureTraits::ABDOMEN;
    liver2->setCategories(liver2Cat);
    CPPUNIT_ASSERT_THROW(structDico->addStructure(liver2), core::exception);
}

} // namespace sight::data::ut
