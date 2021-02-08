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

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <boost/algorithm/string.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/semantic/V09ALA/V10/data/Image.hpp>
#include <io/patch/structural/PatchLoader.hpp>

#include <array>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::semantic::ut::ImageV09ALAToV10Test );

namespace sight::io::patch::semantic
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageV09ALAToV10Test::setUp()
{
    // Set up context before running a test.
    io::patch::structural::PatchLoader::loadPatches();
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

    sight::atoms::Object::sptr image = sight::atoms::Object::New();
    sight::atoms::Object::sptr patchedObj;

    io::atoms::patch::helper::setClassname(image, "::sight::data::Image");
    io::atoms::patch::helper::setVersion(image, "2");

    io::atoms::patch::helper::Object helperImage(image);

    sight::atoms::Map::sptr fieldMap = sight::atoms::Map::New();

    helperImage.addAttribute("fields", fieldMap);

    // create PointList
    sight::atoms::Object::sptr pl = sight::atoms::Object::New();
    io::atoms::patch::helper::setClassname(pl, "::sight::data::PointList");
    io::atoms::patch::helper::setVersion(pl, "1");
    io::atoms::patch::helper::Object helperPL(pl);

    sight::atoms::Sequence::sptr seq = sight::atoms::Sequence::New();
    helperPL.addAttribute("points", seq);
    this->addPoint(seq, P1, "point_1");
    this->addPoint(seq, P2, "point_2");
    this->addPoint(seq, P3, "point_3");
    this->addPoint(seq, P4, "point_4");
    this->addPoint(seq, P5, "point_5");

    fieldMap->insert("m_imageLandmarksId", pl);

    patchedObj = sight::atoms::Object::dynamicCast(image->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[image] = patchedObj;

    io::patch::semantic::V09ALA::V10::data::Image::sptr patcher =
        io::patch::semantic::V09ALA::V10::data::Image::New();
    CPPUNIT_ASSERT_NO_THROW(patcher->apply(image, patchedObj, newVersions));

    CPPUNIT_ASSERT(patchedObj->getAttribute("fields"));
    sight::atoms::Map::sptr newFieldMap = sight::atoms::Map::dynamicCast(patchedObj->getAttribute("fields"));
    CPPUNIT_ASSERT(newFieldMap);

    CPPUNIT_ASSERT(newFieldMap->find("m_landmarksId") != newFieldMap->end());
    sight::atoms::Object::sptr landmarks = sight::atoms::Object::dynamicCast((*newFieldMap)["m_landmarksId"]);
    CPPUNIT_ASSERT(landmarks);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Landmarks"), io::atoms::patch::helper::getClassname(landmarks));
    CPPUNIT_ASSERT(landmarks->getAttribute("landmarks"));
    sight::atoms::Map::sptr landmarksMap = sight::atoms::Map::dynamicCast(landmarks->getAttribute("landmarks"));
    CPPUNIT_ASSERT(landmarksMap);
    CPPUNIT_ASSERT_EQUAL(size_t(5), landmarksMap->size());

    size_t count = 0;
    for (const auto& elt: landmarksMap->getValue())
    {
        sight::atoms::Object::csptr atomGroup = sight::atoms::Object::dynamicCast(elt.second);
        CPPUNIT_ASSERT(atomGroup);
        sight::atoms::String::csptr color = sight::atoms::String::dynamicCast(atomGroup->getAttribute("color"));
        CPPUNIT_ASSERT(color);
        CPPUNIT_ASSERT_EQUAL(std::string("1;1;1;1"), color->getValue());
        sight::atoms::Numeric::csptr size = sight::atoms::Numeric::dynamicCast(atomGroup->getAttribute("size"));
        CPPUNIT_ASSERT(size);
        CPPUNIT_ASSERT_EQUAL(1.f, size->getValue<float>());
        sight::atoms::String::csptr shape = sight::atoms::String::dynamicCast(atomGroup->getAttribute("shape"));
        CPPUNIT_ASSERT(shape);
        CPPUNIT_ASSERT_EQUAL(std::string("SPHERE"), shape->getValue());
        sight::atoms::Boolean::csptr visibility =
            sight::atoms::Boolean::dynamicCast(atomGroup->getAttribute("visibility"));
        CPPUNIT_ASSERT(visibility);
        CPPUNIT_ASSERT_EQUAL(true, visibility->getValue());

        sight::atoms::Sequence::csptr points = sight::atoms::Sequence::dynamicCast(atomGroup->getAttribute("points"));
        CPPUNIT_ASSERT(points);
        CPPUNIT_ASSERT_EQUAL(size_t(1), points->size());
        sight::atoms::String::csptr ptStr = sight::atoms::String::dynamicCast(points->getValue()[0]);

        std::vector<std::string> res;
        const std::string coords = ptStr->getValue();
        ::boost::split(res, coords, ::boost::is_any_of(";"));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pts[count][0], std::stod(res[0]), 0.000001);
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageV09ALAToV10Test::addPoint(sight::atoms::Sequence::sptr seq, const std::array<double, 3>& pt,
                                    const std::string& label)
{
    sight::atoms::Object::sptr point = sight::atoms::Object::New();
    io::atoms::patch::helper::setClassname(point, "::sight::data::Point");
    io::atoms::patch::helper::setVersion(point, "1");
    io::atoms::patch::helper::Object helper(point);

    seq->push_back(point);

    sight::atoms::Sequence::sptr ptSeq = sight::atoms::Sequence::New();
    helper.addAttribute("coord", ptSeq);
    ptSeq->push_back(sight::atoms::Numeric::New(pt[0]));
    ptSeq->push_back(sight::atoms::Numeric::New(pt[1]));
    ptSeq->push_back(sight::atoms::Numeric::New(pt[2]));

    sight::atoms::Map::sptr fieldMap = sight::atoms::Map::New();
    helper.addAttribute("fields", fieldMap);

    sight::atoms::Object::sptr labelObj = sight::atoms::Object::New();
    io::atoms::patch::helper::setClassname(labelObj, "::sight::data::String");
    io::atoms::patch::helper::setVersion(labelObj, "1");
    io::atoms::patch::helper::Object helperLabel(labelObj);
    helperLabel.addAttribute("value", sight::atoms::String::New(label));

    fieldMap->insert("m_labelId", labelObj);
}

//------------------------------------------------------------------------------
} //namespace ut
} //namespace sight::io::patch::semantic
