/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include <sight/data/config.hpp>

#include <core/macros.hpp>

namespace sight::data
{

class dicom_series;

} // namespace sight::data

namespace sight::data
{

class image_series;
class model_series;

} // namespace sight::data

namespace sight::data::dicom
{

/**
 * @brief DICOM series Helper
 */
class SIGHT_DATA_CLASS_API series
{
public:

    /**
     * @brief Convert a dicom_series to an image_series
     * @param[in] _series dicom_series that must be converted
     */
    SIGHT_DATA_API static SPTR(data::image_series) convert_to_image_series(const CSPTR(data::dicom_series) & _series);

    /**
     * @brief Convert a dicom_series to a model_series
     * @param[in] _series dicom_series that must be converted
     */
    SIGHT_DATA_API static SPTR(data::model_series) convert_to_model_series(const CSPTR(data::dicom_series) & _series);
};

} // namespace sight::data::dicom
