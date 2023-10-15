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
    data::structure_traits_dictionary::sptr structDico = std::make_shared<data::structure_traits_dictionary>();
    data::structure_traits::sptr skin                  = std::make_shared<data::structure_traits>();
    std::string skinType                               = "Skin";
    skin->setType(skinType);
    skin->setClass(data::structure_traits::ENVIRONMENT);
    data::color::sptr skinColor = std::make_shared<data::color>(1.0F, 179.0F / 255.0F, 140.0F / 255.0F, 1.0F);
    skin->setColor(skinColor);
    data::structure_traits::CategoryContainer skinCat(1);
    skinCat[0] = data::structure_traits::BODY;
    skin->setCategories(skinCat);
    CPPUNIT_ASSERT_EQUAL(skinType, skin->getType());
    CPPUNIT_ASSERT_EQUAL(data::structure_traits::ENVIRONMENT, skin->getClass());
    CPPUNIT_ASSERT(skinColor == skin->getColor());
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(skin));

    data::structure_traits::sptr liver = std::make_shared<data::structure_traits>();
    liver->setType("Liver");
    liver->setClass(data::structure_traits::ORGAN);
    liver->setColor(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer liverCat(1);
    liverCat[0] = data::structure_traits::ABDOMEN;
    liver->setCategories(liverCat);
    std::string nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    liver->setNativeExp(nativeExp);
    CPPUNIT_ASSERT_EQUAL(nativeExp, liver->getNativeExp());
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(liver));

    data::structure_traits::sptr liverTumor = std::make_shared<data::structure_traits>();
    liverTumor->setType("Liver_Tumor");
    liverTumor->setClass(data::structure_traits::LESION);
    liverTumor->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::CategoryContainer liverTumorCat(1);
    liverTumorCat[0] = data::structure_traits::ABDOMEN;
    liverTumor->setCategories(liverTumorCat);
    liverTumor->setAttachmentType("Liver");
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(liverTumor));

    data::structure_traits::sptr tumor = std::make_shared<data::structure_traits>();
    tumor->setType("Tumor");
    tumor->setClass(data::structure_traits::LESION);
    tumor->setColor(std::make_shared<data::color>(0.0F, 0.0F, 1.0F, 1.0F));
    data::structure_traits::CategoryContainer tumorCat(8);
    tumorCat[0] = data::structure_traits::BODY;
    tumorCat[1] = data::structure_traits::HEAD;
    tumorCat[2] = data::structure_traits::NECK;
    tumorCat[3] = data::structure_traits::THORAX;
    tumorCat[4] = data::structure_traits::ABDOMEN;
    tumorCat[5] = data::structure_traits::PELVIS;
    tumorCat[6] = data::structure_traits::ARM;
    tumorCat[7] = data::structure_traits::LEG;
    tumor->setCategories(tumorCat);
    CPPUNIT_ASSERT_NO_THROW(structDico->addStructure(tumor));

    CPPUNIT_ASSERT(skin == structDico->getStructure("Skin"));
    CPPUNIT_ASSERT(liver == structDico->getStructure("Liver"));
    CPPUNIT_ASSERT(liverTumor == structDico->getStructure("Liver_Tumor"));

    // check exception is raised if wrong structure
    data::structure_traits::sptr badClassStructure = std::make_shared<data::structure_traits>();
    badClassStructure->setType("my_structure");
    badClassStructure->setClass(data::structure_traits::ORGAN);
    badClassStructure->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    data::structure_traits::CategoryContainer structCat(1);
    structCat[0] = data::structure_traits::ABDOMEN;
    badClassStructure->setCategories(structCat);
    badClassStructure->setAttachmentType("Liver");
    CPPUNIT_ASSERT_THROW(structDico->addStructure(badClassStructure), core::exception);

    data::structure_traits::sptr badAttachmentStructure = std::make_shared<data::structure_traits>();
    badAttachmentStructure->setType("my_structure");
    badAttachmentStructure->setClass(data::structure_traits::LESION);
    badAttachmentStructure->setColor(std::make_shared<data::color>(0.0F, 179.0F / 255.0F, 0.0F, 1.0F));
    badAttachmentStructure->setCategories(structCat);
    badAttachmentStructure->setAttachmentType("Unknown");
    CPPUNIT_ASSERT_THROW(structDico->addStructure(badAttachmentStructure), core::exception);

    // check exception is raised if structure already exist
    data::structure_traits::sptr liver2 = std::make_shared<data::structure_traits>();
    liver2->setType("Liver");
    liver2->setClass(data::structure_traits::ORGAN);
    liver2->setColor(std::make_shared<data::color>(204.0F / 255.0F, 51.0F / 255.0F, 51.0F / 255.0F, 1.0F));
    data::structure_traits::CategoryContainer liver2Cat(1);
    liver2Cat[0] = data::structure_traits::ABDOMEN;
    liver2->setCategories(liver2Cat);
    CPPUNIT_ASSERT_THROW(structDico->addStructure(liver2), core::exception);
}

} // namespace sight::data::ut
