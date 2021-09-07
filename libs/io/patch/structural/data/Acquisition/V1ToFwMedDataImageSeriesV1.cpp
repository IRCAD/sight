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

#include "io/patch/structural/data/Acquisition/V1ToFwMedDataImageSeriesV1.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/tools/dateAndTime.hpp>

#include <io/atoms/patch/helper/Object.hpp>
#include <io/atoms/patch/infos/Logger.hpp>
#include <io/atoms/patch/StructuralCreatorDB.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <sstream>

namespace sight::io::patch::structural
{

namespace data
{

namespace Acquisition
{

V1ToFwMedDataImageSeriesV1::V1ToFwMedDataImageSeriesV1() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Acquisition";
    m_targetClassname = "::sight::data::ImageSeries";
    m_originVersion   = "1";
    m_targetVersion   = "1";
}

// ----------------------------------------------------------------------------

V1ToFwMedDataImageSeriesV1::~V1ToFwMedDataImageSeriesV1()
{
}

// ----------------------------------------------------------------------------

V1ToFwMedDataImageSeriesV1::V1ToFwMedDataImageSeriesV1(const V1ToFwMedDataImageSeriesV1& cpy) :
    io::atoms::patch::
    IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToFwMedDataImageSeriesV1::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    IStructuralPatch::apply(previous, current, newVersions);

    //Update object version
    this->updateVersion(current);

    //Create helper
    io::atoms::patch::helper::Object helper(current);

    helper.removeAttribute("bits_per_pixel");
    helper.removeAttribute("slice_thickness");
    helper.removeAttribute("axe");
    helper.removeAttribute("unsigned_flag");
    helper.removeAttribute("acquisition_index");
    helper.removeAttribute("image_type");
    helper.removeAttribute("image_format");
    helper.removeAttribute("is_main");
    helper.removeAttribute("is_normal_dir");
    helper.removeAttribute("db_id");
    helper.removeAttribute("labo_id");
    helper.removeAttribute("net_id");
    helper.removeAttribute("date_send_to_labo");
    helper.removeAttribute("date_receive_from_labo");
    helper.removeAttribute("date_send_to_dbt");
    helper.removeAttribute("date_disponibility");
    helper.removeAttribute("patient_size");
    helper.removeAttribute("patient_weight");
    helper.removeAttribute("radiations");
    helper.removeAttribute("medical_printer");
    helper.removeAttribute("medical_printer_corp");
    helper.removeAttribute("patient_position");
    helper.removeAttribute("dicom_file_list");
    helper.removeAttribute("path_to_files");
    helper.removeAttribute("struct_anat");
    helper.removeAttribute("creation_date");
    helper.removeAttribute("reconstructions");

    helper.renameAttribute("uid", "instance_uid");

    helper.addAttribute("modality", sight::atoms::String::New(""));

    io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();

    // Create Patient
    helper.addAttribute("patient", creators->create("::sight::data::Patient", "1"));

    // Create Equipment
    helper.addAttribute("equipment", creators->create("::sight::data::Equipment", "1"));

    // Create Study
    helper.addAttribute("study", creators->create("::sight::data::Study", "1"));

    // Create performing_physicians_name
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());

    //===========================================
    // Update time format
    //===========================================

    std::string creationDate = previous->getAttribute("creation_date")->getString();

    // Date and time
    ::boost::posix_time::ptime pdate =
        ::boost::posix_time::time_from_string(creationDate);
    const std::string date = core::tools::getDate(pdate);
    const std::string time = core::tools::getTime(pdate);

    helper.addAttribute("date", sight::atoms::String::New(date));
    helper.addAttribute("time", sight::atoms::String::New(time));
}

} // namespace Acquisition

} // namespace data

} // namespace sight::io::patch::structural
