/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "fwDicomIOFilter/splitter/TagValueInstanceRemoveSplitter.hpp"

#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::TagValueInstanceRemoveSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string TagValueInstanceRemoveSplitter::s_FILTER_NAME        = "Tag value instance remove splitter";
const std::string TagValueInstanceRemoveSplitter::s_FILTER_DESCRIPTION =
    "Remove instances according to a tag value.";

//-----------------------------------------------------------------------------

TagValueInstanceRemoveSplitter::TagValueInstanceRemoveSplitter(::fwDicomIOFilter::IFilter::Key key) :
    ISplitter()
{
    m_tag      = DCM_UndefinedTagKey;
    m_tagValue = "";
}

//-----------------------------------------------------------------------------

TagValueInstanceRemoveSplitter::~TagValueInstanceRemoveSplitter()
{
}

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
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    typedef std::vector< ::fwMemory::BufferObject::sptr > InstanceContainerType;

    // Create a container to store the instances
    InstanceContainerType instances;

    OFCondition status;
    DcmDataset* dataset;
    OFString data;

    for(const auto& item : series->getDicomContainer())
    {
        const ::fwMemory::BufferObject::sptr bufferObj = item.second;
        const size_t buffSize                          = bufferObj->getSize();
        ::fwMemory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast< char* >( lock.getBuffer() );

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        DcmFileFormat fileFormat;
        fileFormat.transferInit();
        if (!fileFormat.read(is).good())
        {
            FW_RAISE("Unable to read Dicom file '"<< bufferObj->getStreamInfo().fsFile.string() <<"' "<<
                     "(slice: '" << item.first << "')");
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        dataset = fileFormat.getDataset();

        // Get the value of the instance
        dataset->findAndGetOFStringArray(m_tag, data);
        const std::string value = data.c_str();

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
    size_t index = 0;
    for(const auto& buffer : instances)
    {
        series->addBinary(index++, buffer);
    }
    series->setNumberOfInstances(series->getDicomContainer().size());
    result.push_back(series);

    return result;
}

} // namespace splitter
} // namespace fwDicomIOFilter
