/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwDicomTools/config.hpp"

#include <core/macros.hpp>

namespace sight::data
{
class DicomSeries;
}

namespace sight::data
{
class ImageSeries;
class ModelSeries;
}

namespace fwDicomTools
{

/**
 * @brief DICOM Series Helper
 */
class FWDICOMTOOLS_CLASS_API Series
{
public:
    /**
     * @brief Convert a DicomSeries to an ImageSeries
     * @param[in] series DicomSeries that must be converted
     */
    FWDICOMTOOLS_API static SPTR(data::ImageSeries) convertToImageSeries(
        const CSPTR(data::DicomSeries)& series);

    /**
     * @brief Convert a DicomSeries to a ModelSeries
     * @param[in] series DicomSeries that must be converted
     */
    FWDICOMTOOLS_API static SPTR(data::ModelSeries) convertToModelSeries(
        const CSPTR(data::DicomSeries)& series);

};

} //fwDicomTools
