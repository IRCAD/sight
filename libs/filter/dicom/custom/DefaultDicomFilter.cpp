/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "filter/dicom/custom/DefaultDicomFilter.hpp"

#include "filter/dicom/composite/CTImageStorageDefaultComposite.hpp"
#include "filter/dicom/composite/IComposite.hpp"
#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"
#include "filter/dicom/splitter/SOPClassUIDSplitter.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro(::sight::filter::dicom::custom::DefaultDicomFilter);

namespace sight::filter::dicom
{

namespace custom
{

const std::string DefaultDicomFilter::s_FILTER_NAME        = "Default DICOM filter";
const std::string DefaultDicomFilter::s_FILTER_DESCRIPTION = "Default DICOM filter.";

//-----------------------------------------------------------------------------

DefaultDicomFilter::DefaultDicomFilter(filter::dicom::IFilter::Key key) :
    ICustom()
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
    const data::DicomSeries::sptr& series,
    const core::log::Logger::sptr& logger
)
const
{
    DicomSeriesContainerType result;

    //Split series depending on SOPClassUIDs
    filter::dicom::splitter::SOPClassUIDSplitter::sptr sopFilter =
        filter::dicom::splitter::SOPClassUIDSplitter::New();
    DicomSeriesContainerType seriesContainer = sopFilter->apply(series, logger);

    // Apply default filters depending on SOPClassUIDs
    for(const data::DicomSeries::sptr& s : seriesContainer)
    {
        DicomSeriesContainerType tempo;

        // Create filter depending on SOPClassUID
        data::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer = s->getSOPClassUIDs();
        std::string sopClassUID                                          = sopClassUIDContainer.begin()->c_str();
        if(sopClassUID == "1.2.840.10008.5.1.4.1.1.88.34") // FIXME Remove hard coded string
        {
            sopClassUID = "Comprehensive3DSR";
        }
        else
        {
            sopClassUID = dcmFindNameOfUID(sopClassUID.c_str());
        }

        filter::dicom::composite::IComposite::sptr filter;

        // CT Image Storage
        if(sopClassUID == "CTImageStorage" || sopClassUID == "MRImageStorage"
           || sopClassUID == "SecondaryCaptureImageStorage")
        {
            filter = filter::dicom::composite::CTImageStorageDefaultComposite::New();
        }

        //Apply filter
        if(filter)
        {
            tempo = filter->forcedApply(s, logger);
        }
        else
        {
            logger->information("Can't apply any filter : \"" + sopClassUID + "\" SOPClassUID is not supported.");
            tempo.push_back(s);
        }

        for(data::DicomSeries::sptr filteredSeries : tempo)
        {
            result.push_back(filteredSeries);
        }
    }

    return result;
}

} // namespace custom

} // namespace sight::filter::dicom
