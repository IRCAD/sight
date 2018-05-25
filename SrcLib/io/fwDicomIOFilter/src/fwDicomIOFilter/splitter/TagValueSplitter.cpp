/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/TagValueSplitter.hpp"

#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::TagValueSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string TagValueSplitter::s_FILTER_NAME        = "Tag value splitter";
const std::string TagValueSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to a tag value.";

//-----------------------------------------------------------------------------

TagValueSplitter::TagValueSplitter(::fwDicomIOFilter::IFilter::Key key) :
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
    const ::fwMedData::DicomSeries::sptr& series,
    const ::fwLog::Logger::sptr& logger) const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    typedef std::vector< ::fwMemory::BufferObject::sptr > InstanceContainerType;
    typedef std::map< std::string, InstanceContainerType > InstanceGroupContainer;

    // Create a container to store the groups of instances
    InstanceGroupContainer groupContainer;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;
    OFString data;

    for(const auto& item :  series->getDicomContainer())
    {
        const ::fwMemory::BufferObject::sptr bufferObj = item.second;
        const size_t buffSize                          = bufferObj->getSize();
        ::fwMemory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast< char* >( lock.getBuffer() );

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

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

        // Add the instance to the group
        groupContainer[value].push_back(bufferObj);
    }

    for(const InstanceGroupContainer::value_type& group :  groupContainer)
    {
        // Copy the series
        ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::New();
        dicomSeries->deepCopy(series);
        dicomSeries->clearDicomContainer();

        size_t index = 0;
        // Add the paths to the series
        for(const ::fwMemory::BufferObject::sptr& buffer : group.second)
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
        ss << "Series has been split according to the tag value (" <<
            std::hex << std::setfill('0') << std::setw(4) << m_tag.getGroup() << "," <<
            std::hex << std::setfill('0') << std::setw(4) << m_tag.getElement() << ").";
        logger->warning(ss.str());
    }

    return result;

}

} // namespace splitter
} // namespace fwDicomIOFilter
