/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "series.hpp"

#include <data/dicom_series.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

namespace sight::data::dicom
{

// ----------------------------------------------------------------------------

data::image_series::sptr series::convert_to_image_series(const data::dicom_series::csptr& series)
{
    data::image_series::sptr result = std::make_shared<data::image_series>();
    result->data::series::deep_copy(series);

    return result;
}

// ----------------------------------------------------------------------------

data::model_series::sptr series::convert_to_model_series(const data::dicom_series::csptr& series)
{
    data::model_series::sptr result = std::make_shared<data::model_series>();
    result->data::series::deep_copy(series);

    return result;
}

// ----------------------------------------------------------------------------

} // namespace sight::data::dicom
