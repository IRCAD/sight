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

#include "ImageV09ALAToV10Test.hpp"

#include <fwMDSemanticPatch/V09ALA/V10/data/Image.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwStructuralPatch/PatchLoader.hpp>

#include <utest/Exception.hpp>

#include <boost/algorithm/string.hpp>

#include <array>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMDSemanticPatch::ut::ImageV09ALAToV10Test );

namespace fwMDSemanticPatch
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageV09ALAToV10Test::setUp()
{
    // Set up context before running a test.
    ::fwStructuralPatch::PatchLoader::loadPatches();
}

//------------------------------------------------------------------------------

void ImageV09ALAToV10Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageV09ALAToV10Test::applyPatchTest()
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
    atoms::Object::sptr pl = atoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(pl, "::sight::data::PointList");
    ::fwAtomsPatch::helper::setVersion(pl, "1");
    ::fwAtomsPatch::helper::Object helperPL(pl);

    atoms::Sequence::sptr seq = atoms::Sequence::New();
    helperPL.addAttribute("points", seq);
    this->addPoint(seq, P1, "point_1");
    this->addPoint(seq, P2, "point_2");
    this->addPoint(seq, P3, "point_3");
    this->addPoint(seq, P4, "point_4");
    this->addPoint(seq, P5, "point_5");

    fieldMap->insert("m_imageLandmarksId", pl);

    patchedObj = atoms::Object::dynamicCast(image->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[image] = patchedObj;

    ::fwMDSemanticPatch::V09ALA::V10data::Image::sptr patcher =
        ::fwMDSemanticPatch::V09ALA::V10data::Image::New();
    CPPUNIT_ASSERT_NO_THROW(patcher->apply(image, patchedObj, newVersions));

    CPPUNIT_ASSERT(patchedObj->getAttribute("fields"));
    atoms::Map::sptr newFieldMap = atoms::Map::dynamicCast(patchedObj->getAttribute("fields"));
    CPPUNIT_ASSERT(newFieldMap);

    CPPUNIT_ASSERT(newFieldMap->find("m_landmarksId") != newFieldMap->end());
    atoms::Object::sptr landmarks = atoms::Object::dynamicCast((*newFieldMap)["m_landmarksId"]);
    CPPUNIT_ASSERT(landmarks);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Landmarks"), ::fwAtomsPatch::helper::getClassname(landmarks));
    CPPUNIT_ASSERT(landmarks->getAttribute("landmarks"));
    atoms::Map::sptr landmarksMap = atoms::Map::dynamicCast(landmarks->getAttribute("landmarks"));
    CPPUNIT_ASSERT(landmarksMap);
    CPPUNIT_ASSERT_EQUAL(size_t(5), landmarksMap->size());

    size_t count = 0;
    for (const auto& elt: landmarksMap->getValue())
    {
        atoms::Object::csptr atomGroup = atoms::Object::dynamicCast(elt.second);
        CPPUNIT_ASSERT(atomGroup);
        atoms::String::csptr color = atoms::String::dynamicCast(atomGroup->getAttribute("color"));
        CPPUNIT_ASSERT(color);
        CPPUNIT_ASSERT_EQUAL(std::string("1;1;1;1"), color->getValue());
        atoms::Numeric::csptr size = atoms::Numeric::dynamicCast(atomGroup->getAttribute("size"));
        CPPUNIT_ASSERT(size);
        CPPUNIT_ASSERT_EQUAL(1.f, size->getValue<float>());
        atoms::String::csptr shape = atoms::String::dynamicCast(atomGroup->getAttribute("shape"));
        CPPUNIT_ASSERT(shape);
        CPPUNIT_ASSERT_EQUAL(std::string("SPHERE"), shape->getValue());
        atoms::Boolean::csptr visibility = atoms::Boolean::dynamicCast(atomGroup->getAttribute("visibility"));
        CPPUNIT_ASSERT(visibility);
        CPPUNIT_ASSERT_EQUAL(true, visibility->getValue());

        atoms::Sequence::csptr points = atoms::Sequence::dynamicCast(atomGroup->getAttribute("points"));
        CPPUNIT_ASSERT(points);
        CPPUNIT_ASSERT_EQUAL(size_t(1), points->size());
        atoms::String::csptr ptStr = atoms::String::dynamicCast(points->getValue()[0]);

        std::vector<std::string> res;
        const std::string coords = ptStr->getValue();
        ::boost::split(res, coords, ::boost::is_any_of(";"));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pts[count][0], std::stod(res[0]), 0.000001);
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageV09ALAToV10Test::addPoint(atoms::Sequence::sptr seq, const std::array<double, 3>& pt,
                                    const std::string& label)
{
    atoms::Object::sptr point = atoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(point, "::sight::data::Point");
    ::fwAtomsPatch::helper::setVersion(point, "1");
    ::fwAtomsPatch::helper::Object helper(point);

    seq->push_back(point);

    atoms::Sequence::sptr ptSeq = atoms::Sequence::New();
    helper.addAttribute("coord", ptSeq);
    ptSeq->push_back(atoms::Numeric::New(pt[0]));
    ptSeq->push_back(atoms::Numeric::New(pt[1]));
    ptSeq->push_back(atoms::Numeric::New(pt[2]));

    atoms::Map::sptr fieldMap = atoms::Map::New();
    helper.addAttribute("fields", fieldMap);

    atoms::Object::sptr labelObj = atoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(labelObj, "::sight::data::String");
    ::fwAtomsPatch::helper::setVersion(labelObj, "1");
    ::fwAtomsPatch::helper::Object helperLabel(labelObj);
    helperLabel.addAttribute("value", atoms::String::New(label));

    fieldMap->insert("m_labelId", labelObj);
}

//------------------------------------------------------------------------------
} //namespace ut
} //namespace fwMDSemanticPatch
