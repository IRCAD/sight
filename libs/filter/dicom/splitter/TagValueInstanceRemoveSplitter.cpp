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

#include "filter/dicom/splitter/TagValueInstanceRemoveSplitter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::TagValueInstanceRemoveSplitter);

namespace sight::filter::dicom::splitter
{

const std::string TagValueInstanceRemoveSplitter::s_FILTER_NAME        = "Tag value instance remove splitter";
const std::string TagValueInstanceRemoveSplitter::s_FILTER_DESCRIPTION =
    "Remove instances according to a tag value.";

//-----------------------------------------------------------------------------

TagValueInstanceRemoveSplitter::TagValueInstanceRemoveSplitter() :
    m_tag(DCM_UndefinedTagKey)
{
}

//-----------------------------------------------------------------------------

TagValueInstanceRemoveSplitter::~TagValueInstanceRemoveSplitter()
= default;

//-----------------------------------------------------------------------------

std::string TagValueInstanceRemoveSplitter::getName() const
{
    return TagValueInstanceRemoveSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TagValueInstanceRemoveSplitter::getDescription() const
{
    return TagValueInstanceRemoveSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TagValueInstanceRemoveSplitter::isConfigurationRequired() const
{
    return true;
}

//-----------------------------------------------------------------------------

TagValueInstanceRemoveSplitter::DicomSeriesContainerType TagValueInstanceRemoveSplitter::apply(
    const data::dicom_series::sptr& series,
    const core::log::logger::sptr& logger
)
const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw sight::filter::dicom::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    using InstanceContainerType = std::vector<core::memory::buffer_object::sptr>;

    // Create a container to store the instances
    InstanceContainerType instances;

    OFCondition status;
    OFString data;

    for(const auto& item : series->getDicomContainer())
    {
        const core::memory::buffer_object::sptr bufferObj = item.second;
        const std::size_t buffSize                        = bufferObj->size();
        core::memory::buffer_object::lock_t lock(bufferObj);
        char* buffer = static_cast<char*>(lock.buffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        DcmFileFormat fileFormat;
        fileFormat.transferInit();
        if(!fileFormat.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        DcmDataset* dataset = fileFormat.getDataset();

        // Get the value of the instance
        dataset->findAndGetOFStringArray(m_tag, data);
        const std::string value = data.c_str(); // NOLINT(readability-redundant-string-cstr)

        if(value != m_tagValue)
        {
            instances.push_back(bufferObj);
        }
        else
        {
            logger->warning("An instance has been removed from the series.");
        }
    }

    // Update series
    series->clearDicomContainer();
    std::size_t index = 0;
    for(const auto& buffer : instances)
    {
        series->addBinary(index++, buffer);
    }

    series->setNumberOfInstances(series->getDicomContainer().size());
    result.push_back(series);

    return result;
}

} // namespace sight::filter::dicom::splitter
