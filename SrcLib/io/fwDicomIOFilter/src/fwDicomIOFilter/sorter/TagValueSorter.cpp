/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/sorter/TagValueSorter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::sorter::TagValueSorter );

namespace fwDicomIOFilter
{
namespace sorter
{

const std::string TagValueSorter::s_FILTER_NAME        = "Tag value sorter";
const std::string TagValueSorter::s_FILTER_DESCRIPTION =
    "Sort instances using a tag value.";

//-----------------------------------------------------------------------------

TagValueSorter::TagValueSorter(::fwDicomIOFilter::IFilter::Key key) : ISorter()
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

    typedef std::map< unsigned int, std::string > SortedFileMapType;
    SortedFileMapType sortedFiles;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;
    for(const ::fwMedData::DicomSeries::DicomPathContainerType::value_type& file :  series->getLocalDicomPaths())
    {
        const std::string& filename = file.second.string();
        status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+file.second.string()+"\"", status.bad());
        dataset = fileFormat.getDataset();

        Sint32 index = 0;
        dataset->findAndGetSint32(m_tag, index);

        sortedFiles[index] = file.second.string();
    }

    if(sortedFiles.size() != series->getLocalDicomPaths().size())
    {
        const std::string msg = "Unable to sort the series using the specified tag. The tag may be missing in "
                                "some instances or several instances may have the same tag value.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    ::fwMedData::DicomSeries::DicomPathContainerType dicomPathContainer;
    series->setLocalDicomPaths(dicomPathContainer);

    for(SortedFileMapType::value_type file :  sortedFiles)
    {
        series->addDicomPath(file.first, file.second);
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
