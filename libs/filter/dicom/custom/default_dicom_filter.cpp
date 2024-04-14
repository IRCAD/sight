/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "filter/dicom/custom/default_dicom_filter.hpp"

#include "filter/dicom/composite/base.hpp"
#include "filter/dicom/composite/ct_image_storage_default_composite.hpp"
#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/registry/macros.hpp"
#include "filter/dicom/splitter/sop_class_uid_splitter.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::custom::default_dicom_filter);

namespace sight::filter::dicom::custom
{

const std::string default_dicom_filter::FILTER_NAME        = "Default DICOM filter";
const std::string default_dicom_filter::FILTER_DESCRIPTION = "Default DICOM filter.";

//-----------------------------------------------------------------------------

std::string default_dicom_filter::get_name() const
{
    return default_dicom_filter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string default_dicom_filter::get_description() const
{
    return default_dicom_filter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

default_dicom_filter::dicom_series_container_t default_dicom_filter::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
)
const
{
    dicom_series_container_t result;

    //Split series depending on sop_classUIDs
    auto sop_filter =
        std::make_shared<sight::filter::dicom::splitter::sop_class_uid_splitter>();
    dicom_series_container_t series_container = sop_filter->apply(_series, _logger);

    // Apply default filters depending on sop_classUIDs
    for(const data::dicom_series::sptr& s : series_container)
    {
        dicom_series_container_t tempo;

        // Create filter depending on sop_classUID
        data::dicom_series::sop_class_uid_container_t sop_class_uid_container = s->get_sop_class_ui_ds();
        std::string sop_class_uid                                             = *sop_class_uid_container.begin();
        if(sop_class_uid == "1.2.840.10008.5.1.4.1.1.88.34") // FIXME Remove hard coded string
        {
            sop_class_uid = "Comprehensive3DSR";
        }
        else
        {
            sop_class_uid = dcmFindNameOfUID(sop_class_uid.c_str());
        }

        sight::filter::dicom::composite::base::sptr filter;

        // CT Image Storage
        if(sop_class_uid == "CTImageStorage" || sop_class_uid == "MRImageStorage"
           || sop_class_uid == "SecondaryCaptureImageStorage")
        {
            filter = std::make_shared<sight::filter::dicom::composite::ct_image_storage_default_composite>();
        }

        //Apply filter
        if(filter)
        {
            tempo = filter->forced_apply(s, _logger);
        }
        else
        {
            _logger->information("Can't apply any filter : \"" + sop_class_uid + "\" sop_classUID is not supported.");
            tempo.push_back(s);
        }

        for(const data::dicom_series::sptr& filtered_series : tempo)
        {
            result.push_back(filtered_series);
        }
    }

    return result;
}

} // namespace sight::filter::dicom::custom
