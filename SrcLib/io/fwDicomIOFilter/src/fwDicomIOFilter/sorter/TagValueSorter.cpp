/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/sorter/TagValueSorter.hpp"

#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::sorter::TagValueSorter );

namespace fwDicomIOFilter
{
namespace sorter
{

const std::string TagValueSorter::s_FILTER_NAME        = "Tag value sorter";
const std::string TagValueSorter::s_FILTER_DESCRIPTION =
    "Sort instances using a tag value.";

//-----------------------------------------------------------------------------

TagValueSorter::TagValueSorter(::fwDicomIOFilter::IFilter::Key key) :
    ISorter()
{
    m_tag = DCM_UndefinedTagKey;
}

//-----------------------------------------------------------------------------

TagValueSorter::~TagValueSorter()
{
}

//-----------------------------------------------------------------------------

std::string TagValueSorter::getName() const
{
    return TagValueSorter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TagValueSorter::getDescription() const
{
    return TagValueSorter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TagValueSorter::isConfigurationRequired() const
{
    return true;
}

//-----------------------------------------------------------------------------

TagValueSorter::DicomSeriesContainerType TagValueSorter::apply(
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    ::fwMedData::DicomSeries::DicomContainerType sortedDicom;

    OFCondition status;
    DcmDataset* dataset;
    for(const auto& item :  series->getDicomContainer())
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

        Sint32 index = 0;
        dataset->findAndGetSint32(m_tag, index);

        sortedDicom[size_t(index)] = bufferObj;
    }

    if(sortedDicom.size() != series->getDicomContainer().size())
    {
        const std::string msg = "Unable to sort the series using the specified tag. The tag may be missing in "
                                "some instances or several instances may have the same tag value.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    series->clearDicomContainer();
    for(const auto& item : sortedDicom)
    {
        series->addBinary(item.first, item.second);
    }

    result.push_back(series);

    std::stringstream ss;
    ss << "The instances have been sorted using the value of tag (" <<
        std::hex << std::setfill('0') << std::setw(4) << m_tag.getGroup() << "," <<
        std::hex << std::setfill('0') << std::setw(4) << m_tag.getElement() << ").";
    logger->information(ss.str());

    return result;
}

} // namespace sorter
} // namespace fwDicomIOFilter
