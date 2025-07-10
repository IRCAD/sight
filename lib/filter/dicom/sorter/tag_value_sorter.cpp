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

#include "filter/dicom/sorter/tag_value_sorter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::sorter::tag_value_sorter);

namespace sight::filter::dicom::sorter
{

const std::string tag_value_sorter::FILTER_NAME        = "Tag value sorter";
const std::string tag_value_sorter::FILTER_DESCRIPTION =
    "Sort instances using a tag value.";

//-----------------------------------------------------------------------------

tag_value_sorter::tag_value_sorter() :
    m_tag(DCM_UndefinedTagKey)
{
}

//-----------------------------------------------------------------------------

tag_value_sorter::~tag_value_sorter()
= default;

//-----------------------------------------------------------------------------

std::string tag_value_sorter::get_name() const
{
    return tag_value_sorter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string tag_value_sorter::get_description() const
{
    return tag_value_sorter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool tag_value_sorter::is_configuration_required() const
{
    return true;
}

//-----------------------------------------------------------------------------

tag_value_sorter::dicom_series_container_t tag_value_sorter::apply(
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

    data::dicom_series::dicom_container_t sorted_dicom;

    OFCondition status;
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

        Sint32 index = 0;
        dataset->findAndGetSint32(m_tag, index);

        sorted_dicom[std::size_t(index)] = buffer_obj;
    }

    if(sorted_dicom.size() != _series->get_dicom_container().size())
    {
        const std::string msg = "Unable to sort the series using the specified tag. The tag may be missing in "
                                "some instances or several instances may have the same tag value.";
        throw sight::filter::dicom::exceptions::filter_failure(msg);
    }

    _series->clear_dicom_container();
    for(const auto& item : sorted_dicom)
    {
        _series->add_binary(item.first, item.second);
    }

    result.push_back(_series);

    std::stringstream ss;
    ss << "The instances have been sorted using the value of tag ("
    << std::hex << std::setfill('0') << std::setw(4) << m_tag.getGroup() << ","
    << std::hex << std::setfill('0') << std::setw(4) << m_tag.getElement() << ").";
    _logger->information(ss.str());

    return result;
}

} // namespace sight::filter::dicom::sorter
