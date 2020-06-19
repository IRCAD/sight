/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwData/Acquisition/V1ToFwMedDataImageSeriesV1Test.hpp"

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>
#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwStructuralPatch/fwData/Acquisition/V1ToFwMedDataImageSeriesV1.hpp>

#include <fwTest/Exception.hpp>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwData::Acquisition::V1ToFwMedDataImageSeriesV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Acquisition
{


//------------------------------------------------------------------------------

void V1ToFwMedDataImageSeriesV1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V1ToFwMedDataImageSeriesV1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V1ToFwMedDataImageSeriesV1Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr acqObj = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr isObj;

    ::fwAtomsPatch::helper::setClassname(acqObj, "::fwData::Acquisition");
    ::fwAtomsPatch::helper::setVersion(acqObj, "1");

    ::fwAtomsPatch::helper::Object helper(acqObj);

    helper.addAttribute("bits_per_pixel", ::fwAtoms::String::New(""));
    helper.addAttribute("slice_thickness", ::fwAtoms::String::New(""));
    helper.addAttribute("axe", ::fwAtoms::String::New(""));
    helper.addAttribute("unsigned_flag", ::fwAtoms::String::New(""));
    helper.addAttribute("acquisition_index", ::fwAtoms::String::New(""));
    helper.addAttribute("image_type", ::fwAtoms::String::New(""));
    helper.addAttribute("image_format", ::fwAtoms::String::New(""));
    helper.addAttribute("creation_date", ::fwAtoms::String::New("2013-Jun-12 14:49:12"));
    helper.addAttribute("is_main", ::fwAtoms::String::New(""));
    helper.addAttribute("is_normal_dir", ::fwAtoms::String::New(""));
    helper.addAttribute("uid", ::fwAtoms::String::New(""));
    helper.addAttribute("db_id", ::fwAtoms::String::New(""));
    helper.addAttribute("labo_id", ::fwAtoms::String::New(""));
    helper.addAttribute("net_id", ::fwAtoms::String::New(""));
    helper.addAttribute("date_send_to_labo", ::fwAtoms::String::New(""));
    helper.addAttribute("date_receive_from_labo", ::fwAtoms::String::New(""));
    helper.addAttribute("date_send_to_dbt", ::fwAtoms::String::New(""));
    helper.addAttribute("date_disponibility", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_size", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_weight", ::fwAtoms::String::New(""));
    helper.addAttribute("radiations", ::fwAtoms::String::New(""));
    helper.addAttribute("medical_printer", ::fwAtoms::String::New(""));
    helper.addAttribute("medical_printer_corp", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_position", ::fwAtoms::String::New(""));
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    helper.addAttribute("dicom_file_list", ::fwAtoms::String::New(""));
    helper.addAttribute("path_to_files", ::fwAtoms::String::New(""));
    helper.addAttribute("image", ::fwAtoms::Object::New());
    helper.addAttribute("struct_anat", ::fwAtoms::Object::sptr());
    helper.addAttribute("reconstructions", ::fwAtoms::Sequence::New());

    isObj = ::fwAtoms::Object::dynamicCast(acqObj->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[acqObj] = isObj;

    ::fwStructuralPatch::fwData::Acquisition::V1ToFwMedDataImageSeriesV1::sptr patch;
    patch = ::fwStructuralPatch::fwData::Acquisition::V1ToFwMedDataImageSeriesV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(acqObj, isObj, newVersions));

    CPPUNIT_ASSERT(isObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::fwMedData::ImageSeries"), ::fwAtomsPatch::helper::getClassname(isObj));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), ::fwAtomsPatch::helper::getVersion(isObj));

    CPPUNIT_ASSERT(!isObj->getAttribute("bits_per_pixel"));
    CPPUNIT_ASSERT(!isObj->getAttribute("slice_thickness"));
    CPPUNIT_ASSERT(!isObj->getAttribute("axe"));
    CPPUNIT_ASSERT(!isObj->getAttribute("unsigned_flag"));
    CPPUNIT_ASSERT(!isObj->getAttribute("acquisition_index"));
    CPPUNIT_ASSERT(!isObj->getAttribute("image_type"));
    CPPUNIT_ASSERT(!isObj->getAttribute("image_format"));
    CPPUNIT_ASSERT(!isObj->getAttribute("is_main"));
    CPPUNIT_ASSERT(!isObj->getAttribute("is_normal_dir"));
    CPPUNIT_ASSERT(!isObj->getAttribute("db_id"));
    CPPUNIT_ASSERT(!isObj->getAttribute("labo_id"));
    CPPUNIT_ASSERT(!isObj->getAttribute("net_id"));
    CPPUNIT_ASSERT(!isObj->getAttribute("date_send_to_labo"));
    CPPUNIT_ASSERT(!isObj->getAttribute("date_receive_from_labo"));
    CPPUNIT_ASSERT(!isObj->getAttribute("date_send_to_dbt"));
    CPPUNIT_ASSERT(!isObj->getAttribute("date_disponibility"));
    CPPUNIT_ASSERT(!isObj->getAttribute("patient_size"));
    CPPUNIT_ASSERT(!isObj->getAttribute("patient_weight"));
    CPPUNIT_ASSERT(!isObj->getAttribute("radiations"));
    CPPUNIT_ASSERT(!isObj->getAttribute("medical_printer"));
    CPPUNIT_ASSERT(!isObj->getAttribute("medical_printer_corp"));
    CPPUNIT_ASSERT(!isObj->getAttribute("patient_position"));
    CPPUNIT_ASSERT(!isObj->getAttribute("dicom_file_list"));
    CPPUNIT_ASSERT(!isObj->getAttribute("path_to_files"));
    CPPUNIT_ASSERT(!isObj->getAttribute("struct_anat"));
    CPPUNIT_ASSERT(!isObj->getAttribute("creation_date"));
    CPPUNIT_ASSERT(!isObj->getAttribute("reconstructions"));

    CPPUNIT_ASSERT(isObj->getAttribute("instance_uid"));
    CPPUNIT_ASSERT(isObj->getAttribute("modality"));
    CPPUNIT_ASSERT(isObj->getAttribute("patient"));
    CPPUNIT_ASSERT(isObj->getAttribute("equipment"));
    CPPUNIT_ASSERT(isObj->getAttribute("study"));
    CPPUNIT_ASSERT(isObj->getAttribute("performing_physicians_name"));
    CPPUNIT_ASSERT(isObj->getAttribute("date"));
    CPPUNIT_ASSERT(isObj->getAttribute("time"));

    ::fwAtoms::String::sptr date = isObj->getAttribute< ::fwAtoms::String >("date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130612"), date->getString());
    ::fwAtoms::String::sptr time = isObj->getAttribute< ::fwAtoms::String >("time");
    CPPUNIT_ASSERT_EQUAL(std::string("144912"), time->getString());
}

//------------------------------------------------------------------------------
} //namespace Acquisition
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch
