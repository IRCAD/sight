/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "data/ModelSeries/V1ToV2Test.hpp"

#include <fwStructuralPatch/data/ModelSeries/V1ToV2.hpp>

#include <atoms/Base.hpp>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::ModelSeries::V1ToV2Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
{
namespace ModelSeries
{

//------------------------------------------------------------------------------

void V1ToV2Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V1ToV2Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V1ToV2Test::applyPatchTest()
{
    atoms::Object::sptr modelSeriesV1 = atoms::Object::New();
    atoms::Object::sptr modelSeriesV2;

    ::fwAtomsPatch::helper::setClassname(modelSeriesV1, "::sight::data::ModelSeries");
    ::fwAtomsPatch::helper::setVersion(modelSeriesV1, "1");

    ::fwAtomsPatch::helper::Object helper(modelSeriesV1);

    helper.addAttribute("reconstruction_db", atoms::Sequence::New());

    helper.addAttribute("patient", atoms::Object::sptr());
    helper.addAttribute("study", atoms::Object::sptr());
    helper.addAttribute("equipment", atoms::Object::sptr());
    helper.addAttribute("instance_uid", atoms::String::New("UID"));
    helper.addAttribute("modality", atoms::String::New("OT"));
    helper.addAttribute("date", atoms::String::New("0"));
    helper.addAttribute("time", atoms::String::New("0"));
    helper.addAttribute("performing_physicians_name", atoms::Sequence::New());
    helper.addAttribute("description", atoms::String::New("Description"));

    // Check ModelSeries V2 has a new attribute "dicom_reference"
    {
        modelSeriesV2 = atoms::Object::dynamicCast(modelSeriesV1->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[modelSeriesV1] = modelSeriesV2;

        ::fwStructuralPatchdata::ModelSeries::V1ToV2::sptr v1ToV2Patch;
        v1ToV2Patch = ::fwStructuralPatchdata::ModelSeries::V1ToV2::New();
        CPPUNIT_ASSERT_NO_THROW(v1ToV2Patch->apply(modelSeriesV1, modelSeriesV2, newVersions));

        CPPUNIT_ASSERT(modelSeriesV2);
        CPPUNIT_ASSERT_EQUAL(std::string("2"), ::fwAtomsPatch::helper::getVersion(modelSeriesV2));
        const auto attributes = modelSeriesV2->getAttributes();
        CPPUNIT_ASSERT(attributes.find("dicom_reference") != attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace ModelSeries
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch
