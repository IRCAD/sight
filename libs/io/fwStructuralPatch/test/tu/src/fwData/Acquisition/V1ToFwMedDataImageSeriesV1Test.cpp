/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/Acquisition/V1ToFwMedDataImageSeriesV1Test.hpp"

#include <fwStructuralPatch/data/Acquisition/V1ToFwMedDataImageSeriesV1.hpp>

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::utdata::Acquisition::V1ToFwMedDataImageSeriesV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace sight::data
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
    atoms::Object::sptr acqObj = atoms::Object::New();
    atoms::Object::sptr isObj;

    ::fwAtomsPatch::helper::setClassname(acqObj, "::sight::data::Acquisition");
    ::fwAtomsPatch::helper::setVersion(acqObj, "1");

    ::fwAtomsPatch::helper::Object helper(acqObj);

    helper.addAttribute("bits_per_pixel", atoms::String::New(""));
    helper.addAttribute("slice_thickness", atoms::String::New(""));
    helper.addAttribute("axe", atoms::String::New(""));
    helper.addAttribute("unsigned_flag", atoms::String::New(""));
    helper.addAttribute("acquisition_index", atoms::String::New(""));
    helper.addAttribute("image_type", atoms::String::New(""));
    helper.addAttribute("image_format", atoms::String::New(""));
    helper.addAttribute("creation_date", atoms::String::New("2013-Jun-12 14:49:12"));
    helper.addAttribute("is_main", atoms::String::New(""));
    helper.addAttribute("is_normal_dir", atoms::String::New(""));
    helper.addAttribute("uid", atoms::String::New(""));
    helper.addAttribute("db_id", atoms::String::New(""));
    helper.addAttribute("labo_id", atoms::String::New(""));
    helper.addAttribute("net_id", atoms::String::New(""));
    helper.addAttribute("date_send_to_labo", atoms::String::New(""));
    helper.addAttribute("date_receive_from_labo", atoms::String::New(""));
    helper.addAttribute("date_send_to_dbt", atoms::String::New(""));
    helper.addAttribute("date_disponibility", atoms::String::New(""));
    helper.addAttribute("patient_size", atoms::String::New(""));
    helper.addAttribute("patient_weight", atoms::String::New(""));
    helper.addAttribute("radiations", atoms::String::New(""));
    helper.addAttribute("medical_printer", atoms::String::New(""));
    helper.addAttribute("medical_printer_corp", atoms::String::New(""));
    helper.addAttribute("patient_position", atoms::String::New(""));
    helper.addAttribute("description", atoms::String::New(""));
    helper.addAttribute("dicom_file_list", atoms::String::New(""));
    helper.addAttribute("path_to_files", atoms::String::New(""));
    helper.addAttribute("image", atoms::Object::New());
    helper.addAttribute("struct_anat", atoms::Object::sptr());
    helper.addAttribute("reconstructions", atoms::Sequence::New());

    isObj = atoms::Object::dynamicCast(acqObj->clone());

    ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
    newVersions[acqObj] = isObj;

    ::fwStructuralPatchdata::Acquisition::V1ToFwMedDataImageSeriesV1::sptr patch;
    patch = ::fwStructuralPatchdata::Acquisition::V1ToFwMedDataImageSeriesV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(acqObj, isObj, newVersions));

    CPPUNIT_ASSERT(isObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::ImageSeries"), ::fwAtomsPatch::helper::getClassname(isObj));
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

    atoms::String::sptr date = isObj->getAttribute< atoms::String >("date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130612"), date->getString());
    atoms::String::sptr time = isObj->getAttribute< atoms::String >("time");
    CPPUNIT_ASSERT_EQUAL(std::string("144912"), time->getString());
}

//------------------------------------------------------------------------------
} //namespace Acquisition
} //namespace sight::data
} //namespace ut
} //namespace fwStructuralPatch
