/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>

#include "fwDicomIOFilter/composite/CTImageStorageDefaultComposite.hpp"
#include "fwDicomIOFilter/composite/IComposite.hpp"
#include "fwDicomIOFilter/custom/DefaultDicomFilter.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/splitter/SOPClassUIDSplitter.hpp"

#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::custom::DefaultDicomFilter );

namespace fwDicomIOFilter
{
namespace custom
{

const std::string DefaultDicomFilter::s_FILTER_NAME        = "Default DICOM filter";
const std::string DefaultDicomFilter::s_FILTER_DESCRIPTION = "Default DICOM filter.";

//-----------------------------------------------------------------------------

DefaultDicomFilter::DefaultDicomFilter(::fwDicomIOFilter::IFilter::Key key) : ICustom()
{
}

//-----------------------------------------------------------------------------

DefaultDicomFilter::~DefaultDicomFilter()
{
}

//-----------------------------------------------------------------------------

std::string DefaultDicomFilter::getName() const
{
    return DefaultDicomFilter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string DefaultDicomFilter::getDescription() const
{
    return DefaultDicomFilter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

DefaultDicomFilter::DicomSeriesContainerType DefaultDicomFilter::apply(
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const throw(::fwDicomIOFilter::exceptions::FilterFailure)
{
    DicomSeriesContainerType result;

    //Split series depending on SOPClassUIDs
    ::fwDicomIOFilter::splitter::SOPClassUIDSplitter::sptr sopFilter =
        ::fwDicomIOFilter::splitter::SOPClassUIDSplitter::New();
    DicomSeriesContainerType seriesContainer = sopFilter->apply(series, logger);

    // Apply default filters depending on SOPClassUIDs
    for(const ::fwMedData::DicomSeries::sptr& s :  seriesContainer)
    {
        DicomSeriesContainerType tempo;

        // Create filter depending on SOPClassUID
        ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = s->getSOPClassUIDs();
        std::string sopClassUID = sopClassUIDContainer.begin()->c_str();
        if(sopClassUID == "1.2.840.10008.5.1.4.1.1.88.34")    // FIXME Remove hard coded string
        {
            sopClassUID = "Comprehensive3DSR";
        }
        else
        {
            sopClassUID = dcmFindNameOfUID(sopClassUID.c_str());
        }

        ::fwDicomIOFilter::composite::IComposite::sptr filter;

        // CT Image Storage
        if(sopClassUID == "CTImageStorage" || sopClassUID == "MRImageStorage" ||
           sopClassUID == "SecondaryCaptureImageStorage")
        {
            filter = ::fwDicomIOFilter::composite::CTImageStorageDefaultComposite::New();
        }

        //Apply filter
        if(filter)
        {
            SLM_TRACE("Applying default filter for SOPClassUID: \""+sopClassUID+"\".");
            tempo = filter->forcedApply(s, logger);
        }
        else
        {
            logger->information("Can't apply any filter : \""+sopClassUID+"\" SOPClassUID is not supported.");
            tempo.push_back(s);
        }

        for(::fwMedData::DicomSeries::sptr filteredSeries :  tempo)
        {
            result.push_back(filteredSeries);
        }
    }

    return result;

}

} // namespace custom
} // namespace fwDicomIOFilter
