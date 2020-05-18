/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
const
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
