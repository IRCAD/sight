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

#include "V1ToFwMedDataImageSeriesV1Test.hpp"

#include <atoms/Base.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <utest/Exception.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>
#include <io/patch/structural/data/Acquisition/V1ToFwMedDataImageSeriesV1.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::patch::structural::ut::data::Acquisition::V1ToFwMedDataImageSeriesV1Test );

namespace sight::io::patch::structural
{
namespace ut
{
namespace data
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
    sight::atoms::Object::sptr acqObj = sight::atoms::Object::New();
    sight::atoms::Object::sptr isObj;

    io::atoms::patch::helper::setClassname(acqObj, "::sight::data::Acquisition");
    io::atoms::patch::helper::setVersion(acqObj, "1");

    io::atoms::patch::helper::Object helper(acqObj);

    helper.addAttribute("bits_per_pixel", sight::atoms::String::New(""));
    helper.addAttribute("slice_thickness", sight::atoms::String::New(""));
    helper.addAttribute("axe", sight::atoms::String::New(""));
    helper.addAttribute("unsigned_flag", sight::atoms::String::New(""));
    helper.addAttribute("acquisition_index", sight::atoms::String::New(""));
    helper.addAttribute("image_type", sight::atoms::String::New(""));
    helper.addAttribute("image_format", sight::atoms::String::New(""));
    helper.addAttribute("creation_date", sight::atoms::String::New("2013-Jun-12 14:49:12"));
    helper.addAttribute("is_main", sight::atoms::String::New(""));
    helper.addAttribute("is_normal_dir", sight::atoms::String::New(""));
    helper.addAttribute("uid", sight::atoms::String::New(""));
    helper.addAttribute("db_id", sight::atoms::String::New(""));
    helper.addAttribute("labo_id", sight::atoms::String::New(""));
    helper.addAttribute("net_id", sight::atoms::String::New(""));
    helper.addAttribute("date_send_to_labo", sight::atoms::String::New(""));
    helper.addAttribute("date_receive_from_labo", sight::atoms::String::New(""));
    helper.addAttribute("date_send_to_dbt", sight::atoms::String::New(""));
    helper.addAttribute("date_disponibility", sight::atoms::String::New(""));
    helper.addAttribute("patient_size", sight::atoms::String::New(""));
    helper.addAttribute("patient_weight", sight::atoms::String::New(""));
    helper.addAttribute("radiations", sight::atoms::String::New(""));
    helper.addAttribute("medical_printer", sight::atoms::String::New(""));
    helper.addAttribute("medical_printer_corp", sight::atoms::String::New(""));
    helper.addAttribute("patient_position", sight::atoms::String::New(""));
    helper.addAttribute("description", sight::atoms::String::New(""));
    helper.addAttribute("dicom_file_list", sight::atoms::String::New(""));
    helper.addAttribute("path_to_files", sight::atoms::String::New(""));
    helper.addAttribute("image", sight::atoms::Object::New());
    helper.addAttribute("struct_anat", sight::atoms::Object::sptr());
    helper.addAttribute("reconstructions", sight::atoms::Sequence::New());

    isObj = sight::atoms::Object::dynamicCast(acqObj->clone());

    io::atoms::patch::IPatch::NewVersionsType newVersions;
    newVersions[acqObj] = isObj;

    patch::structural::data::Acquisition::V1ToFwMedDataImageSeriesV1::sptr patch;
    patch = patch::structural::data::Acquisition::V1ToFwMedDataImageSeriesV1::New();
    CPPUNIT_ASSERT_NO_THROW(patch->apply(acqObj, isObj, newVersions));

    CPPUNIT_ASSERT(isObj);
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::ImageSeries"), io::atoms::patch::helper::getClassname(isObj));
    CPPUNIT_ASSERT_EQUAL(std::string("1"), io::atoms::patch::helper::getVersion(isObj));

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

    sight::atoms::String::sptr date = isObj->getAttribute< sight::atoms::String >("date");
    CPPUNIT_ASSERT_EQUAL(std::string("20130612"), date->getString());
    sight::atoms::String::sptr time = isObj->getAttribute< sight::atoms::String >("time");
    CPPUNIT_ASSERT_EQUAL(std::string("144912"), time->getString());
}

//------------------------------------------------------------------------------
} //namespace Acquisition
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural
