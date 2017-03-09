/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageV09ALAToV10Test.hpp"

#include <fwMDSemanticPatch/V09ALA/V10/fwData/Image.hpp>

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

#include <fwStructuralPatch/PatchLoader.hpp>

#include <fwTest/Exception.hpp>

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

    ::fwAtoms::Object::sptr image = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr patchedObj;

    ::fwAtomsPatch::helper::setClassname(image, "::fwData::Image");
    ::fwAtomsPatch::helper::setVersion(image, "2");

    ::fwAtomsPatch::helper::Object helperImage(image);

    ::fwAtoms::Map::sptr fieldMap = ::fwAtoms::Map::New();

    helperImage.addAttribute("fields", fieldMap);

    // create PointList
    ::fwAtoms::Object::sptr pl = ::fwAtoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(pl, "::fwData::PointList");
    ::fwAtomsPatch::helper::setVersion(pl, "1");
    ::fwAtomsPatch::helper::Object helperPL(pl);

    ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();
    helperPL.addAttribute("points", seq);
    this->addPoint(seq, P1, "point_1");
    this->addPoint(seq, P2, "point_2");
    this->addPoint(seq, P3, "point_3");
    this->addPoint(seq, P4, "point_4");
    this->addPoint(seq, P5, "point_5");

    fieldMap->insert("m_imageLandmarksId", pl);

    patchedObj = ::fwAtoms::Object::dynamicCast(image->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[image] = patchedObj;

    ::fwMDSemanticPatch::V09ALA::V10::fwData::Image::sptr patcher =
        ::fwMDSemanticPatch::V09ALA::V10::fwData::Image::New();
    CPPUNIT_ASSERT_NO_THROW(patcher->apply(image, patchedObj, newVersions));

    CPPUNIT_ASSERT(patchedObj->getAttribute("fields"));
    ::fwAtoms::Map::sptr newFieldMap = ::fwAtoms::Map::dynamicCast(patchedObj->getAttribute("fields"));
    CPPUNIT_ASSERT(newFieldMap);

    CPPUNIT_ASSERT(newFieldMap->find("m_landmarksId") != newFieldMap->end());
    ::fwAtoms::Object::sptr landmarks = ::fwAtoms::Object::dynamicCast((*newFieldMap)["m_landmarksId"]);
    CPPUNIT_ASSERT(landmarks);
    CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Landmarks"), ::fwAtomsPatch::helper::getClassname(landmarks));
    CPPUNIT_ASSERT(landmarks->getAttribute("landmarks"));
    ::fwAtoms::Map::sptr landmarksMap = ::fwAtoms::Map::dynamicCast(landmarks->getAttribute("landmarks"));
    CPPUNIT_ASSERT(landmarksMap);
    CPPUNIT_ASSERT_EQUAL(size_t(5), landmarksMap->size());

    size_t count = 0;
    for (const auto& elt: landmarksMap->getValue())
    {
        ::fwAtoms::Object::csptr atomGroup = ::fwAtoms::Object::dynamicCast(elt.second);
        CPPUNIT_ASSERT(atomGroup);
        ::fwAtoms::String::csptr color = ::fwAtoms::String::dynamicCast(atomGroup->getAttribute("color"));
        CPPUNIT_ASSERT(color);
        CPPUNIT_ASSERT_EQUAL(std::string("1;1;1;1"), color->getValue());
        ::fwAtoms::Numeric::csptr size = ::fwAtoms::Numeric::dynamicCast(atomGroup->getAttribute("size"));
        CPPUNIT_ASSERT(size);
        CPPUNIT_ASSERT_EQUAL(1.f, size->getValue<float>());
        ::fwAtoms::String::csptr shape = ::fwAtoms::String::dynamicCast(atomGroup->getAttribute("shape"));
        CPPUNIT_ASSERT(shape);
        CPPUNIT_ASSERT_EQUAL(std::string("SPHERE"), shape->getValue());
        ::fwAtoms::Boolean::csptr visibility = ::fwAtoms::Boolean::dynamicCast(atomGroup->getAttribute("visibility"));
        CPPUNIT_ASSERT(visibility);
        CPPUNIT_ASSERT_EQUAL(true, visibility->getValue());

        ::fwAtoms::Sequence::csptr points = ::fwAtoms::Sequence::dynamicCast(atomGroup->getAttribute("points"));
        CPPUNIT_ASSERT(points);
        CPPUNIT_ASSERT_EQUAL(size_t(1), points->size());
        ::fwAtoms::String::csptr ptStr = ::fwAtoms::String::dynamicCast(points->getValue()[0]);

        std::vector<std::string> res;
        const std::string coords = ptStr->getValue();
        ::boost::split(res, coords, ::boost::is_any_of(";"));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(pts[count][0], std::stod(res[0]), 0.000001);
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageV09ALAToV10Test::addPoint(::fwAtoms::Sequence::sptr seq, const std::array<double, 3>& pt,
                                    const std::string& label)
{
    ::fwAtoms::Object::sptr point = ::fwAtoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(point, "::fwData::Point");
    ::fwAtomsPatch::helper::setVersion(point, "1");
    ::fwAtomsPatch::helper::Object helper(point);

    seq->push_back(point);

    ::fwAtoms::Sequence::sptr ptSeq = ::fwAtoms::Sequence::New();
    helper.addAttribute("coord", ptSeq);
    ptSeq->push_back(::fwAtoms::Numeric::New(pt[0]));
    ptSeq->push_back(::fwAtoms::Numeric::New(pt[1]));
    ptSeq->push_back(::fwAtoms::Numeric::New(pt[2]));

    ::fwAtoms::Map::sptr fieldMap = ::fwAtoms::Map::New();
    helper.addAttribute("fields", fieldMap);

    ::fwAtoms::Object::sptr labelObj = ::fwAtoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(labelObj, "::fwData::String");
    ::fwAtomsPatch::helper::setVersion(labelObj, "1");
    ::fwAtomsPatch::helper::Object helperLabel(labelObj);
    helperLabel.addAttribute("value", ::fwAtoms::String::New(label));

    fieldMap->insert("m_labelId", labelObj);
}

//------------------------------------------------------------------------------
} //namespace ut
} //namespace fwMDSemanticPatch
