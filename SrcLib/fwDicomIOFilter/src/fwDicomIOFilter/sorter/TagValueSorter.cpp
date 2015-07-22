/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/sorter/TagValueSorter.hpp"

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

bool TagValueSorter::isConfigurationRequired()
{
    return true;
}

//-----------------------------------------------------------------------------

TagValueSorter::DicomSeriesContainerType TagValueSorter::apply(
    ::fwDicomData::DicomSeries::sptr series) const throw(::fwDicomIOFilter::exceptions::FilterFailure)
{

    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    typedef std::map< double, std::string > SortedFileMapType;
    SortedFileMapType sortedFiles;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;
    BOOST_FOREACH(const ::fwDicomData::DicomSeries::DicomPathContainerType::value_type& file,
                  series->getLocalDicomPaths())
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

    ::fwDicomData::DicomSeries::DicomPathContainerType dicomPathContainer;
    series->setLocalDicomPaths(dicomPathContainer);

    BOOST_FOREACH(SortedFileMapType::value_type file, sortedFiles)
    {
        series->addDicomPath(file.first, file.second);
    }

    result.push_back(series);
    return result;

}

} // namespace sorter
} // namespace fwDicomIOFilter
