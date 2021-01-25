/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <fwMDSemanticPatch/V10/V09ALA/data/Image.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

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

    atoms::Object::sptr image = atoms::Object::New();
    atoms::Object::sptr patchedObj;

    ::fwAtomsPatch::helper::setClassname(image, "::sight::data::Image");
    ::fwAtomsPatch::helper::setVersion(image, "2");

    ::fwAtomsPatch::helper::Object helperImage(image);

    atoms::Map::sptr fieldMap = atoms::Map::New();

    helperImage.addAttribute("fields", fieldMap);

    // create PointList
    atoms::Object::sptr landmarks = atoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(landmarks, "::sight::data::Landmarks");
    ::fwAtomsPatch::helper::setVersion(landmarks, "1");
    ::fwAtomsPatch::helper::Object helperLandmarks(landmarks);

    atoms::Map::sptr map = atoms::Map::New();
    helperLandmarks.addAttribute("landmarks", map);
    this->addPoint(map, P1, "point_1");
    this->addPoint(map, P2, "point_2");
    this->addPoint(map, P3, "point_3");
    this->addPoint(map, P4, "point_4");
    this->addPoint(map, P5, "point_5");

    fieldMap->insert("m_landmarksId", landmarks);

    patchedObj = atoms::Object::dynamicCast(image->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[image] = patchedObj;

    ::fwMDSemanticPatch::V10::V09ALAdata::Image::sptr patcher =
        ::fwMDSemanticPatch::V10::V09ALAdata::Image::New();
    CPPUNIT_ASSERT_NO_THROW(patcher->apply(image, patchedObj, newVersions));

    CPPUNIT_ASSERT(patchedObj->getAttribute("fields"));
    atoms::Map::sptr newFieldMap = atoms::Map::dynamicCast(patchedObj->getAttribute("fields"));
    CPPUNIT_ASSERT(newFieldMap);

    CPPUNIT_ASSERT(newFieldMap->find("m_imageLandmarksId") != newFieldMap->end());
    atoms::Object::sptr pl = atoms::Object::dynamicCast((*newFieldMap)["m_imageLandmarksId"]);
    CPPUNIT_ASSERT(pl);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::PointList"), ::fwAtomsPatch::helper::getClassname(pl));
    CPPUNIT_ASSERT(pl->getAttribute("points"));
    atoms::Sequence::sptr plSeq = atoms::Sequence::dynamicCast(pl->getAttribute("points"));
    CPPUNIT_ASSERT(plSeq);
    CPPUNIT_ASSERT_EQUAL(size_t(5), plSeq->size());

    size_t count = 0;
    for (const auto& elt: plSeq->getValue())
    {
        atoms::Object::sptr point = atoms::Object::dynamicCast(elt);
        CPPUNIT_ASSERT(point);
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Point"), ::fwAtomsPatch::helper::getClassname(point));
        atoms::Sequence::csptr pointCoords = atoms::Sequence::dynamicCast(point->getAttribute("coord"));
        CPPUNIT_ASSERT_EQUAL(size_t(3), pointCoords->size());
        atoms::Numeric::csptr coordX = atoms::Numeric::dynamicCast(pointCoords->getValue()[0]);
        CPPUNIT_ASSERT(coordX);
        CPPUNIT_ASSERT_EQUAL(pts[count][0], coordX->getValue<double>());
        atoms::Numeric::csptr coordY = atoms::Numeric::dynamicCast(pointCoords->getValue()[1]);
        CPPUNIT_ASSERT(coordY);
        CPPUNIT_ASSERT_EQUAL(pts[count][1], coordY->getValue<double>());
        atoms::Numeric::csptr coordZ = atoms::Numeric::dynamicCast(pointCoords->getValue()[2]);
        CPPUNIT_ASSERT(coordZ);
        CPPUNIT_ASSERT_EQUAL(pts[count][2], coordZ->getValue<double>());
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageV10ToV09ALATest::addPoint(atoms::Map::sptr map, const std::array<double, 3>& pt,
                                    const std::string& label)
{
    atoms::Object::sptr atomGroup = atoms::Object::New();
    atomGroup->setMetaInfo("ID_METAINFO", core::tools::UUID::generateUUID());

    atomGroup->setAttribute("color", atoms::String::New("1;1;1;1"));
    atomGroup->setAttribute("size", atoms::Numeric::New(1));
    atomGroup->setAttribute("shape", atoms::String::New("SPHERE"));
    atomGroup->setAttribute("visibility", atoms::Boolean::New(true));

    atoms::Sequence::sptr seq = atoms::Sequence::New();

    const std::string coords = std::to_string(pt[0]) + ";" + std::to_string(pt[1]) + ";"
                               + std::to_string(pt[2]);
    seq->push_back(atoms::String::New(coords));
    atomGroup->setAttribute("points", seq);
    map->insert(label, atomGroup);
}

//------------------------------------------------------------------------------
} //namespace ut
} //namespace fwMDSemanticPatch
