/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "ImageV10ToV09ALATest.hpp"

#include <fwMDSemanticPatch/V10/V09ALA/fwData/Image.hpp>

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>

#include <fwTools/UUID.hpp>

#include <boost/algorithm/string.hpp>

#include <array>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMDSemanticPatch::ut::ImageV10ToV09ALATest );

namespace fwMDSemanticPatch
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageV10ToV09ALATest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImageV10ToV09ALATest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageV10ToV09ALATest::applyPatchTest()
{
    const std::array<double, 3> P1 = {{33.5, 25.54, 45.45}};
    const std::array<double, 3> P2 = {{45.45, 54.4, 42.5}};
    const std::array<double, 3> P3 = {{5.4, 5.5, 4.5}};
    const std::array<double, 3> P4 = {{465.5, 5.4, 89.5}};
    const std::array<double, 3> P5 = {{3.5, 8.3, 0.}};

    std::vector<std::array<double, 3> > pts;
    pts.push_back(P1);
    pts.push_back(P2);
    pts.push_back(P3);
    pts.push_back(P4);
    pts.push_back(P5);

    ::fwAtoms::Object::sptr image = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr patchedObj;

    ::fwAtomsPatch::helper::setClassname(image, "::fwData::Image");
    ::fwAtomsPatch::helper::setVersion(image, "2");

    ::fwAtomsPatch::helper::Object helperImage(image);

    ::fwAtoms::Map::sptr fieldMap = ::fwAtoms::Map::New();

    helperImage.addAttribute("fields", fieldMap);

    // create PointList
    ::fwAtoms::Object::sptr landmarks = ::fwAtoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(landmarks, "::fwData::Landmarks");
    ::fwAtomsPatch::helper::setVersion(landmarks, "1");
    ::fwAtomsPatch::helper::Object helperLandmarks(landmarks);

    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::New();
    helperLandmarks.addAttribute("landmarks", map);
    this->addPoint(map, P1, "point_1");
    this->addPoint(map, P2, "point_2");
    this->addPoint(map, P3, "point_3");
    this->addPoint(map, P4, "point_4");
    this->addPoint(map, P5, "point_5");

    fieldMap->insert("m_landmarksId", landmarks);

    patchedObj = ::fwAtoms::Object::dynamicCast(image->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[image] = patchedObj;

    ::fwMDSemanticPatch::V10::V09ALA::fwData::Image::sptr patcher =
        ::fwMDSemanticPatch::V10::V09ALA::fwData::Image::New();
    CPPUNIT_ASSERT_NO_THROW(patcher->apply(image, patchedObj, newVersions));

    CPPUNIT_ASSERT(patchedObj->getAttribute("fields"));
    ::fwAtoms::Map::sptr newFieldMap = ::fwAtoms::Map::dynamicCast(patchedObj->getAttribute("fields"));
    CPPUNIT_ASSERT(newFieldMap);

    CPPUNIT_ASSERT(newFieldMap->find("m_imageLandmarksId") != newFieldMap->end());
    ::fwAtoms::Object::sptr pl = ::fwAtoms::Object::dynamicCast((*newFieldMap)["m_imageLandmarksId"]);
    CPPUNIT_ASSERT(pl);
    CPPUNIT_ASSERT_EQUAL(std::string("::fwData::PointList"), ::fwAtomsPatch::helper::getClassname(pl));
    CPPUNIT_ASSERT(pl->getAttribute("points"));
    ::fwAtoms::Sequence::sptr plSeq = ::fwAtoms::Sequence::dynamicCast(pl->getAttribute("points"));
    CPPUNIT_ASSERT(plSeq);
    CPPUNIT_ASSERT_EQUAL(size_t(5), plSeq->size());

    size_t count = 0;
    for (const auto& elt: plSeq->getValue())
    {
        ::fwAtoms::Object::sptr point = ::fwAtoms::Object::dynamicCast(elt);
        CPPUNIT_ASSERT(point);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Point"), ::fwAtomsPatch::helper::getClassname(point));
        ::fwAtoms::Sequence::csptr pointCoords = ::fwAtoms::Sequence::dynamicCast(point->getAttribute("coord"));
        CPPUNIT_ASSERT_EQUAL(size_t(3), pointCoords->size());
        ::fwAtoms::Numeric::csptr coordX = ::fwAtoms::Numeric::dynamicCast(pointCoords->getValue()[0]);
        CPPUNIT_ASSERT(coordX);
        CPPUNIT_ASSERT_EQUAL(pts[count][0], coordX->getValue<double>());
        ::fwAtoms::Numeric::csptr coordY = ::fwAtoms::Numeric::dynamicCast(pointCoords->getValue()[1]);
        CPPUNIT_ASSERT(coordY);
        CPPUNIT_ASSERT_EQUAL(pts[count][1], coordY->getValue<double>());
        ::fwAtoms::Numeric::csptr coordZ = ::fwAtoms::Numeric::dynamicCast(pointCoords->getValue()[2]);
        CPPUNIT_ASSERT(coordZ);
        CPPUNIT_ASSERT_EQUAL(pts[count][2], coordZ->getValue<double>());
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageV10ToV09ALATest::addPoint(::fwAtoms::Map::sptr map, const std::array<double, 3>& pt,
                                    const std::string& label)
{
    ::fwAtoms::Object::sptr atomGroup = ::fwAtoms::Object::New();
    atomGroup->setMetaInfo("ID_METAINFO", ::fwTools::UUID::generateUUID());

    atomGroup->setAttribute("color", ::fwAtoms::String::New("1;1;1;1"));
    atomGroup->setAttribute("size", ::fwAtoms::Numeric::New(1));
    atomGroup->setAttribute("shape", ::fwAtoms::String::New("SPHERE"));
    atomGroup->setAttribute("visibility", ::fwAtoms::Boolean::New(true));

    ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();

    const std::string coords = std::to_string(pt[0]) + ";" + std::to_string(pt[1]) + ";"
                               + std::to_string(pt[2]);
    seq->push_back(::fwAtoms::String::New(coords));
    atomGroup->setAttribute("points", seq);
    map->insert(label, atomGroup);
}

//------------------------------------------------------------------------------
} //namespace ut
} //namespace fwMDSemanticPatch
