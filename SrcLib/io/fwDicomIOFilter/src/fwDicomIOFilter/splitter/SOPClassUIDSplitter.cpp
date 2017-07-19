/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/SOPClassUIDSplitter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmimgle/dcmimage.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::SOPClassUIDSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string SOPClassUIDSplitter::s_FILTER_NAME        = "SOPClassUID splitter";
const std::string SOPClassUIDSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to <i>SOPClassUID</i> tag.";

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::SOPClassUIDSplitter(::fwDicomIOFilter::IFilter::Key key) :
    ::fwDicomIOFilter::splitter::TagValueSplitter(key)
{
    this->setTag(DCM_SOPClassUID);
}

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::~SOPClassUIDSplitter()
{
}

//-----------------------------------------------------------------------------

std::string SOPClassUIDSplitter::getName() const
{
    return SOPClassUIDSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string SOPClassUIDSplitter::getDescription() const
{
    return SOPClassUIDSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool SOPClassUIDSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::DicomSeriesContainerType SOPClassUIDSplitter::apply(
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const
{
    DicomSeriesContainerType result = ::fwDicomIOFilter::splitter::TagValueSplitter::apply(series, logger);

    for(const ::fwMedData::DicomSeries::sptr& s :  result)
    {

        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;
        OFString data;

        // Open file
        const std::string& filename = s->getLocalDicomPaths().begin()->second.string();
        status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());

        // Read SOPClassUID
        dataset = fileFormat.getDataset();
        status  = dataset->findAndGetOFStringArray(DCM_SOPClassUID,data);
        FW_RAISE_IF("Unable to read tags: \""+filename+"\"", status.bad());

        ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer;
        sopClassUIDContainer.insert(data.c_str());
        s->setSOPClassUIDs(sopClassUIDContainer);
    }

    if(result.size() > 1)
    {
        logger->warning("The same series instance UID has been used for several instances "
                        "with different SOP class UID. The series has been split.");
    }

    return result;

}

} // namespace splitter
} // namespace fwDicomIOFilter
