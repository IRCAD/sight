/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/TagValueInstanceRemoveSplitter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::TagValueInstanceRemoveSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string TagValueInstanceRemoveSplitter::s_FILTER_NAME        = "Tag value instance remove splitter";
const std::string TagValueInstanceRemoveSplitter::s_FILTER_DESCRIPTION =
    "Remove instances according to a tag value.";

//-----------------------------------------------------------------------------

TagValueInstanceRemoveSplitter::TagValueInstanceRemoveSplitter(::fwDicomIOFilter::IFilter::Key key) : ISplitter()
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

    typedef std::vector< std::string > InstanceContainerType;

    // Create a container to store the instances
    InstanceContainerType instances;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;
    OFString data;

    for(const ::fwMedData::DicomSeries::DicomPathContainerType::value_type& file :  series->getLocalDicomPaths())
    {
        const std::string& filename = file.second.string();
        status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());
        dataset = fileFormat.getDataset();

        // Get the value of the instance
        dataset->findAndGetOFStringArray(m_tag,data);
        ::std::string value = data.c_str();

        if(value != m_tagValue)
        {
            instances.push_back(filename.c_str());
        }
        else
        {
            logger->warning("An instance has been removed from the series.");
        }
    }

    // Update series
    ::fwMedData::DicomSeries::DicomPathContainerType dicomPathContainer;
    series->setLocalDicomPaths(dicomPathContainer);
    unsigned int index = 0;
    for(std::string file :  instances)
    {
        series->addDicomPath(index++, file);
    }
    series->setNumberOfInstances(static_cast<unsigned int>(series->getLocalDicomPaths().size()));
    result.push_back(series);

    return result;

}

} // namespace splitter
} // namespace fwDicomIOFilter
