/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <sight/filter/dicom/config.hpp>

#include "filter/dicom/modifier/base.hpp"

#include <data/dicom_series.hpp>

namespace sight::filter::dicom::modifier
{

/**
 * @brief filter that uses the ImagepositionPatient tag to sort the instances.
 * The position increases along the direction given by cross product of the ImageOrientationPatient vectors.
 */
class SIGHT_FILTER_DICOM_CLASS_API slice_thickness_modifier : public base
{
public:

    SIGHT_DECLARE_CLASS(slice_thickness_modifier, base, sight::filter::dicom::factory::make<slice_thickness_modifier>);

    /// Destructor
    SIGHT_FILTER_DICOM_API ~slice_thickness_modifier() override = default;

    /// Override
    SIGHT_FILTER_DICOM_API dicom_series_container_t apply(
        const data::dicom_series::sptr& _series,
        const core::log::logger::sptr& _logger
    ) const override;

    /// Return the name of the filter
    SIGHT_FILTER_DICOM_API std::string get_name() const override;

    /// Return the description of the filter
    SIGHT_FILTER_DICOM_API std::string get_description() const override;

protected:

    /**
     * @brief Compute the Z coordinate of the slice according to the ImagePositionPatient and ImageOrientationPatient
     *  tags.
     *  @param[in] _buffer_obj BufferObject containing the slice
     */
    SIGHT_FILTER_DICOM_API virtual double get_instance_z_position(const core::memory::buffer_object::sptr& _buffer_obj)
    const;

    /**
     * @brief Get the SliceThickness value from an instance.
     *  @param[in] _buffer_obj BufferObject containing the slice
     */
    SIGHT_FILTER_DICOM_API virtual double get_slice_thickness(const core::memory::buffer_object::sptr& _buffer_obj)
    const;

    /// filter name
    static const std::string FILTER_NAME;

    /// filter description
    static const std::string FILTER_DESCRIPTION;
};

} // namespace sight::filter::dicom::modifier
