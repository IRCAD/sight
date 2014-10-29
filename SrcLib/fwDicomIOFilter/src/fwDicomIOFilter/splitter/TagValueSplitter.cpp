/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
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
#include "fwDicomIOFilter/splitter/TagValueSplitter.hpp"

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::TagValueSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string TagValueSplitter::s_FILTER_NAME = "Tag value splitter";
const std::string TagValueSplitter::s_FILTER_DESCRIPTION =
        "Split instances according to a tag value.";

//-----------------------------------------------------------------------------

TagValueSplitter::TagValueSplitter(::fwDicomIOFilter::IFilter::Key key) : ISplitter()
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

bool TagValueSplitter::isConfigurationRequired()
{
    return true;
}

//-----------------------------------------------------------------------------

TagValueSplitter::DicomSeriesContainerType TagValueSplitter::apply(
        ::fwDicomData::DicomSeries::sptr series) const throw(::fwDicomIOFilter::exceptions::FilterFailure)
{
    if(m_tag == DCM_UndefinedTagKey)
    {
        const std::string msg = "Unable to split the series, the specified tag is not valid.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    DicomSeriesContainerType result;

    typedef std::vector< std::string > InstanceContainerType;
    typedef std::map< std::string, InstanceContainerType > InstanceGroupContainer;

    // Create a container to store the groups of instances
    InstanceGroupContainer groupContainer;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;
    OFString data;

    BOOST_FOREACH(const ::fwDicomData::DicomSeries::DicomPathContainerType::value_type& file, series->getLocalDicomPaths())
    {
        const std::string& filename = file.second.string();
        status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());
        dataset = fileFormat.getDataset();

        // Get the value of the instance
        dataset->findAndGetOFString(m_tag,data);
        ::std::string value = data.c_str();

        // Add the instance to the group
        groupContainer[value].push_back(filename.c_str());
    }

    BOOST_FOREACH(InstanceGroupContainer::value_type group, groupContainer)
    {
        // Copy the series
        ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::New();
        dicomSeries->deepCopy(series);
        ::fwDicomData::DicomSeries::DicomPathContainerType dicomPathContainer;
        dicomSeries->setLocalDicomPaths(dicomPathContainer);
        unsigned int index = 0;

        // Add the paths to the series
        BOOST_FOREACH(std::string file, group.second)
        {
            dicomSeries->addDicomPath(index++, file);
        }

        // Set number of instances
        dicomSeries->setNumberOfInstances(dicomSeries->getLocalDicomPaths().size());

        result.push_back(dicomSeries);
    }

    return result;

}

} // namespace splitter
} // namespace fwDicomIOFilter
