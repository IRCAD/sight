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

#include "filter/dicom/splitter/TagValueSplitter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::TagValueSplitter);

namespace sight::filter::dicom::splitter
{

const std::string TagValueSplitter::s_FILTER_NAME        = "Tag value splitter";
const std::string TagValueSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to a tag value.";

//-----------------------------------------------------------------------------

TagValueSplitter::TagValueSplitter() :
    m_tag(DCM_UndefinedTagKey)
{
}

//-----------------------------------------------------------------------------

TagValueSplitter::~TagValueSplitter()
= default;

//-----------------------------------------------------------------------------

std::string TagValueSplitter::getName() const
{
    return TagValueSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TagValueSplitter::getDescription() const
{
    return TagValueSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TagValueSplitter::isConfigurationRequired() const
{
    return true;
}

//-----------------------------------------------------------------------------

TagValueSplitter::DicomSeriesContainerType TagValueSplitter::apply(
    const data::DicomSeries::sptr& series,
    const core::log::logger::sptr& logger
) const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw sight::filter::dicom::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    using InstanceContainerType  = std::vector<core::memory::buffer_object::sptr>;
    using InstanceGroupContainer = std::map<std::string, InstanceContainerType>;

    // Create a container to store the groups of instances
    InstanceGroupContainer groupContainer;

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

        // Add the instance to the group
        groupContainer[value].push_back(bufferObj);
    }

    for(const InstanceGroupContainer::value_type& group : groupContainer)
    {
        // Copy the series
        data::DicomSeries::sptr dicomSeries = std::make_shared<data::DicomSeries>();
        dicomSeries->shallow_copy(series);
        dicomSeries->clearDicomContainer();

        std::size_t index = 0;
        // Add the paths to the series
        for(const core::memory::buffer_object::sptr& buffer : group.second)
        {
            dicomSeries->addBinary(index++, buffer);
        }

        // Set number of instances
        dicomSeries->setNumberOfInstances(dicomSeries->getDicomContainer().size());

        result.push_back(dicomSeries);
    }

    if(result.size() > 1)
    {
        std::stringstream ss;
        ss << "Series has been split according to the tag value ("
        << std::hex << std::setfill('0') << std::setw(4) << m_tag.getGroup() << ","
        << std::hex << std::setfill('0') << std::setw(4) << m_tag.getElement() << ").";
        logger->warning(ss.str());
    }

    return result;
}

} // namespace sight::filter::dicom::splitter
