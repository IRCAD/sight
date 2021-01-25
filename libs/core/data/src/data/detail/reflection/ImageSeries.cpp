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

#define CAMP_COMPILATION

#include "data/detail/reflection/ImageSeries.hpp"
#include "data/ImageSeries.hpp"

#include <data/Image.hpp>

#include <core/reflection/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((sight)(data)(ImageSeries))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "fwMedData")
    .base< ::sight::data::Series>()
    .property("image", &::sight::data::ImageSeries::m_image)
    .property("dicom_reference", &::sight::data::ImageSeries::m_dicomReference)
    .property("contrast_bolus_agent", &::sight::data::ImageSeries::m_contrastBolusAgent)
    .property("contrast_bolus_route", &::sight::data::ImageSeries::m_contrastBolusRoute)
    .property("contrast_bolus_volume", &::sight::data::ImageSeries::m_contrastBolusVolume)
    .property("contrast_bolus_start_time", &::sight::data::ImageSeries::m_contrastBolusStartTime)
    .property("contrast_bolus_stop_time", &::sight::data::ImageSeries::m_contrastBolusStopTime)
    .property("contrast_bolus_total_dose", &::sight::data::ImageSeries::m_contrastBolusTotalDose)
    .property("contrast_bolus_flow_rate", &::sight::data::ImageSeries::m_contrastBolusFlowRate)
    .property("contrast_bolus_flow_duration", &::sight::data::ImageSeries::m_contrastBolusFlowDuration)
    .property("contrast_bolus_ingredient", &::sight::data::ImageSeries::m_contrastBolusIngredient)
    .property("contrast_bolus_ingredient_concentration",
              &::sight::data::ImageSeries::m_contrastBolusIngredientConcentration)
    .property("acquisition_date", &::sight::data::ImageSeries::m_acquisitionDate)
    .property("acquisition_time", &::sight::data::ImageSeries::m_acquisitionTime)
    ;
}
