/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "filter/dicom/splitter/TagValueSplitter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro(::sight::filter::dicom::splitter::TagValueSplitter);

namespace sight::filter::dicom
{

namespace splitter
{

const std::string TagValueSplitter::s_FILTER_NAME        = "Tag value splitter";
const std::string TagValueSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to a tag value.";

//-----------------------------------------------------------------------------

TagValueSplitter::TagValueSplitter(filter::dicom::IFilter::Key key) :
    ISplitter()
{
    m_tag = DCM_UndefinedTagKey;
}

//-----------------------------------------------------------------------------

TagValueSplitter::~TagValueSplitter()
{
}

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
    const core::log::Logger::sptr& logger
) const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw filter::dicom::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    typedef std::vector<core::memory::BufferObject::sptr> InstanceContainerType;
    typedef std::map<std::string, InstanceContainerType> InstanceGroupContainer;

    // Create a container to store the groups of instances
    InstanceGroupContainer groupContainer;

    OFCondition status;
    DcmDataset* dataset;
    OFString data;

    for(const auto& item : series->getDicomContainer())
    {
        const core::memory::BufferObject::sptr bufferObj = item.second;
        const size_t buffSize                            = bufferObj->getSize();
        core::memory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast<char*>(lock.getBuffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        DcmFileFormat fileFormat;
        fileFormat.transferInit();
        if(!fileFormat.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->getStreamInfo().fsFile.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        dataset = fileFormat.getDataset();

        // Get the value of the instance
        dataset->findAndGetOFStringArray(m_tag, data);
        const std::string value = data.c_str();

        // Add the instance to the group
        groupContainer[value].push_back(bufferObj);
    }

    for(const InstanceGroupContainer::value_type& group : groupContainer)
    {
        // Copy the series
        data::DicomSeries::sptr dicomSeries = data::DicomSeries::New();
        dicomSeries->shallowCopy(series);
        dicomSeries->clearDicomContainer();

        size_t index = 0;
        // Add the paths to the series
        for(const core::memory::BufferObject::sptr& buffer : group.second)
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

} // namespace splitter

} // namespace sight::filter::dicom
