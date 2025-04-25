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

#include "filter/dicom/splitter/sop_class_uid_splitter.hpp"

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::sop_class_uid_splitter);

namespace sight::filter::dicom::splitter
{

const std::string sop_class_uid_splitter::FILTER_NAME        = "sop_classUID splitter";
const std::string sop_class_uid_splitter::FILTER_DESCRIPTION =
    "Split instances according to <i>sop_classUID</i> tag.";

//-----------------------------------------------------------------------------

sop_class_uid_splitter::sop_class_uid_splitter()
{
    this->set_tag(DCM_SOPClassUID);
}

//-----------------------------------------------------------------------------

sop_class_uid_splitter::~sop_class_uid_splitter()
= default;

//-----------------------------------------------------------------------------

std::string sop_class_uid_splitter::get_name() const
{
    return sop_class_uid_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string sop_class_uid_splitter::get_description() const
{
    return sop_class_uid_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool sop_class_uid_splitter::is_configuration_required() const
{
    return false;
}

//-----------------------------------------------------------------------------

sop_class_uid_splitter::dicom_series_container_t sop_class_uid_splitter::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
) const
{
    dicom_series_container_t result = sight::filter::dicom::splitter::tag_value_splitter::apply(_series, _logger);

    for(const data::dicom_series::sptr& dicom_series : result)
    {
        DcmFileFormat file_format;
        OFCondition status;
        DcmDataset* dataset = nullptr;
        OFString data;

        // Open first instance
        const auto first_item                              = dicom_series->get_dicom_container().begin();
        const core::memory::buffer_object::sptr buffer_obj = first_item->second;
        const std::size_t buff_size                        = buffer_obj->size();
        const std::string dicom_path                       = buffer_obj->get_stream_info().fs_file.string();
        core::memory::buffer_object::lock_t lock(buffer_obj);
        char* buffer = static_cast<char*>(lock.buffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buff_size));
        is.setEos();

        file_format.transferInit();
        if(!file_format.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << dicom_path << "' "
                << "(slice: '" << first_item->first << "')"
            );
        }

        file_format.loadAllDataIntoMemory();
        file_format.transferEnd();

        // Read sop_classUID
        dataset = file_format.getDataset();
        status  = dataset->findAndGetOFStringArray(DCM_SOPClassUID, data);
        SIGHT_THROW_IF("Unable to read tags: \"" + dicom_path + "\"", status.bad());

        data::dicom_series::sop_class_uid_container_t sop_class_uid_container;
        sop_class_uid_container.insert(data.c_str());
        dicom_series->set_sop_class_ui_ds(sop_class_uid_container);
    }

    if(result.size() > 1)
    {
        _logger->warning(
            "The same series instance UID has been used for several instances "
            "with different SOP class UID. The series has been split."
        );
    }

    return result;
}

} // namespace sight::filter::dicom::splitter
