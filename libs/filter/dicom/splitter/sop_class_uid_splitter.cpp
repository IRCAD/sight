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

#include "filter/dicom/exceptions/FilterFailure.hpp"
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

const std::string sop_class_uid_splitter::s_FILTER_NAME        = "sop_classUID splitter";
const std::string sop_class_uid_splitter::s_FILTER_DESCRIPTION =
    "Split instances according to <i>sop_classUID</i> tag.";

//-----------------------------------------------------------------------------

sop_class_uid_splitter::sop_class_uid_splitter()
{
    this->setTag(DCM_SOPClassUID);
}

//-----------------------------------------------------------------------------

sop_class_uid_splitter::~sop_class_uid_splitter()
= default;

//-----------------------------------------------------------------------------

std::string sop_class_uid_splitter::getName() const
{
    return sop_class_uid_splitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string sop_class_uid_splitter::getDescription() const
{
    return sop_class_uid_splitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool sop_class_uid_splitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

sop_class_uid_splitter::DicomSeriesContainerType sop_class_uid_splitter::apply(
    const data::dicom_series::sptr& series,
    const core::log::logger::sptr& logger
) const
{
    DicomSeriesContainerType result = sight::filter::dicom::splitter::TagValueSplitter::apply(series, logger);

    for(const data::dicom_series::sptr& dicomSeries : result)
    {
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset = nullptr;
        OFString data;

        // Open first instance
        const auto firstItem                              = dicomSeries->getDicomContainer().begin();
        const core::memory::buffer_object::sptr bufferObj = firstItem->second;
        const std::size_t buffSize                        = bufferObj->size();
        const std::string dicomPath                       = bufferObj->get_stream_info().fs_file.string();
        core::memory::buffer_object::lock_t lock(bufferObj);
        char* buffer = static_cast<char*>(lock.buffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        fileFormat.transferInit();
        if(!fileFormat.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << dicomPath << "' "
                << "(slice: '" << firstItem->first << "')"
            );
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        // Read sop_classUID
        dataset = fileFormat.getDataset();
        status  = dataset->findAndGetOFStringArray(DCM_SOPClassUID, data);
        SIGHT_THROW_IF("Unable to read tags: \"" + dicomPath + "\"", status.bad());

        data::dicom_series::sop_classUIDContainerType sopClassUIDContainer;
        sopClassUIDContainer.insert(data.c_str());
        dicomSeries->setSOPClassUIDs(sopClassUIDContainer);
    }

    if(result.size() > 1)
    {
        logger->warning(
            "The same series instance UID has been used for several instances "
            "with different SOP class UID. The series has been split."
        );
    }

    return result;
}

} // namespace sight::filter::dicom::splitter
