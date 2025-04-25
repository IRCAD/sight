/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINT

#include "filter/dicom/splitter/tag_value_splitter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::tag_value_splitter);

namespace sight::filter::dicom::splitter
{

const std::string tag_value_splitter::FILTER_NAME        = "Tag value splitter";
const std::string tag_value_splitter::FILTER_DESCRIPTION =
    "Split instances according to a tag value.";

//-----------------------------------------------------------------------------

tag_value_splitter::tag_value_splitter() :
    m_tag(DCM_UndefinedTagKey)
{
}

//-----------------------------------------------------------------------------

tag_value_splitter::~tag_value_splitter()
= default;

//-----------------------------------------------------------------------------

std::string tag_value_splitter::get_name() const
{
    return tag_value_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string tag_value_splitter::get_description() const
{
    return tag_value_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool tag_value_splitter::is_configuration_required() const
{
    return true;
}

//-----------------------------------------------------------------------------

tag_value_splitter::dicom_series_container_t tag_value_splitter::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
) const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw sight::filter::dicom::exceptions::filter_failure(msg);
    }

    dicom_series_container_t result;

    using instance_container_t     = std::vector<core::memory::buffer_object::sptr>;
    using instance_group_container = std::map<std::string, instance_container_t>;

    // Create a container to store the groups of instances
    instance_group_container group_container;

    OFCondition status;
    OFString data;

    for(const auto& item : _series->get_dicom_container())
    {
        const core::memory::buffer_object::sptr buffer_obj = item.second;
        const std::size_t buff_size                        = buffer_obj->size();
        core::memory::buffer_object::lock_t lock(buffer_obj);
        char* buffer = static_cast<char*>(lock.buffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buff_size));
        is.setEos();

        DcmFileFormat file_format;
        file_format.transferInit();
        if(!file_format.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << buffer_obj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        file_format.loadAllDataIntoMemory();
        file_format.transferEnd();

        DcmDataset* dataset = file_format.getDataset();

        // Get the value of the instance
        dataset->findAndGetOFStringArray(m_tag, data);
        const std::string value = data.c_str(); // NOLINT(readability-redundant-string-cstr)

        // Add the instance to the group
        group_container[value].push_back(buffer_obj);
    }

    for(const auto& group : group_container)
    {
        // Copy the series
        data::dicom_series::sptr dicom_series = std::make_shared<data::dicom_series>();
        dicom_series->shallow_copy(_series);
        dicom_series->clear_dicom_container();

        std::size_t index = 0;
        // Add the paths to the series
        for(const core::memory::buffer_object::sptr& buffer : group.second)
        {
            dicom_series->add_binary(index++, buffer);
        }

        // Set number of instances
        dicom_series->set_number_of_instances(dicom_series->get_dicom_container().size());

        result.push_back(dicom_series);
    }

    if(result.size() > 1)
    {
        std::stringstream ss;
        ss << "Series has been split according to the tag value ("
        << std::hex << std::setfill('0') << std::setw(4) << m_tag.getGroup() << ","
        << std::hex << std::setfill('0') << std::setw(4) << m_tag.getElement() << ").";
        _logger->warning(ss.str());
    }

    return result;
}

} // namespace sight::filter::dicom::splitter
