/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "filter/dicom/splitter/tag_value_instance_remove_splitter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::tag_value_instance_remove_splitter);

namespace sight::filter::dicom::splitter
{

const std::string tag_value_instance_remove_splitter::FILTER_NAME        = "Tag value instance remove splitter";
const std::string tag_value_instance_remove_splitter::FILTER_DESCRIPTION =
    "Remove instances according to a tag value.";

//-----------------------------------------------------------------------------

tag_value_instance_remove_splitter::tag_value_instance_remove_splitter() :
    m_tag(DCM_UndefinedTagKey)
{
}

//-----------------------------------------------------------------------------

tag_value_instance_remove_splitter::~tag_value_instance_remove_splitter()
= default;

//-----------------------------------------------------------------------------

std::string tag_value_instance_remove_splitter::get_name() const
{
    return tag_value_instance_remove_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string tag_value_instance_remove_splitter::get_description() const
{
    return tag_value_instance_remove_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool tag_value_instance_remove_splitter::is_configuration_required() const
{
    return true;
}

//-----------------------------------------------------------------------------

tag_value_instance_remove_splitter::dicom_series_container_t tag_value_instance_remove_splitter::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
)
const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw sight::filter::dicom::exceptions::filter_failure(msg);
    }

    dicom_series_container_t result;

    using instance_container_t = std::vector<core::memory::buffer_object::sptr>;

    // Create a container to store the instances
    instance_container_t instances;

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

        if(value != m_tag_value)
        {
            instances.push_back(buffer_obj);
        }
        else
        {
            _logger->warning("An instance has been removed from the series.");
        }
    }

    // Update series
    _series->clear_dicom_container();
    std::size_t index = 0;
    for(const auto& buffer : instances)
    {
        _series->add_binary(index++, buffer);
    }

    _series->set_number_of_instances(_series->get_dicom_container().size());
    result.push_back(_series);

    return result;
}

} // namespace sight::filter::dicom::splitter
