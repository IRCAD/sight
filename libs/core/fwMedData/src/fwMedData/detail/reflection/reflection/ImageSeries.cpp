/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwMedData/detail/reflection/ImageSeries.hpp"

#include <fwMedData/ImageSeries.hpp>

#include <fwCamp/UserObject.hpp>

#include <fwData/Image.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(ImageSeries))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("image", &::fwMedData::ImageSeries::m_image)
    .property("dicom_reference", &::fwMedData::ImageSeries::m_dicomReference)
    .property("contrast_bolus_agent", &::fwMedData::ImageSeries::m_contrastBolusAgent)
    .property("contrast_bolus_route", &::fwMedData::ImageSeries::m_contrastBolusRoute)
    .property("contrast_bolus_volume", &::fwMedData::ImageSeries::m_contrastBolusVolume)
    .property("contrast_bolus_start_time", &::fwMedData::ImageSeries::m_contrastBolusStartTime)
    .property("contrast_bolus_stop_time", &::fwMedData::ImageSeries::m_contrastBolusStopTime)
    .property("contrast_bolus_total_dose", &::fwMedData::ImageSeries::m_contrastBolusTotalDose)
    .property("contrast_bolus_flow_rate", &::fwMedData::ImageSeries::m_contrastBolusFlowRate)
    .property("contrast_bolus_flow_duration", &::fwMedData::ImageSeries::m_contrastBolusFlowDuration)
    .property("contrast_bolus_ingredient", &::fwMedData::ImageSeries::m_contrastBolusIngredient)
    .property("contrast_bolus_ingredient_concentration",
              &::fwMedData::ImageSeries::m_contrastBolusIngredientConcentration)
    .property("acquisition_date", &::fwMedData::ImageSeries::m_acquisitionDate)
    .property("acquisition_time", &::fwMedData::ImageSeries::m_acquisitionTime)
    ;
}
