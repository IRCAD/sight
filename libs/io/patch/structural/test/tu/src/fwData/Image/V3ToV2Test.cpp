/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/Image/V3ToV2Test.hpp"

#include <atoms/Base.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Image/V3ToV2.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::utdata::Image::V3ToV2Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
{
namespace Image
{

//------------------------------------------------------------------------------

void V3ToV2Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V3ToV2Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V3ToV2Test::applyPatchTest()
{
    auto patch = patch::structural::data::Image::V3ToV2::New();

    sight::atoms::Object::sptr imgObjV3 = sight::atoms::Object::New();
    sight::atoms::Object::sptr imgObjV2;

    io::atoms::patch::helper::setClassname(imgObjV3, "::sight::data::Image");
    io::atoms::patch::helper::setVersion(imgObjV3, "3");

    io::atoms::patch::helper::Object helper(imgObjV3);

    helper.addAttribute("nb_components", sight::atoms::Numeric::New(3));
    helper.addAttribute("pixel_format", sight::atoms::String::New("BGR"));

    imgObjV2 = sight::atoms::Object::dynamicCast(imgObjV3->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[imgObjV3] = imgObjV2;

    CPPUNIT_ASSERT_NO_THROW(patch->apply(imgObjV3, imgObjV2, newVersions));

    CPPUNIT_ASSERT(imgObjV3);
    CPPUNIT_ASSERT(nullptr == imgObjV2->getAttribute("pixel_format"));
}

//------------------------------------------------------------------------------
} //namespace Image
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural
