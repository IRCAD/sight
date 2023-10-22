/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "filter/dicom/config.hpp"
#include "filter/dicom/modifier/base.hpp"

#include <data/dicom_series.hpp>

namespace sight::filter::dicom::modifier
{

/**
 * @brief Filter that uses the ImagepositionPatient tag to sort the instances.
 * The position increases along the direction given by cross product of the ImageOrientationPatient vectors.
 */
class FILTER_DICOM_CLASS_API SliceThicknessModifier : public base
{
public:

    SIGHT_DECLARE_CLASS(SliceThicknessModifier, base, sight::filter::dicom::factory::make<SliceThicknessModifier>);

    /// Destructor
    FILTER_DICOM_API ~SliceThicknessModifier() override = default;

    /// Override
    FILTER_DICOM_API dicom_series_container_t apply(
        const data::dicom_series::sptr& _series,
        const core::log::logger::sptr& _logger
    ) const override;

    /// Return the name of the filter
    FILTER_DICOM_API std::string getName() const override;

    /// Return the description of the filter
    FILTER_DICOM_API std::string getDescription() const override;

protected:

    /**
     * @brief Compute the Z coordinate of the slice according to the ImagePositionPatient and ImageOrientationPatient
     *  tags.
     *  @param[in] bufferObj BufferObject containing the slice
     */
    FILTER_DICOM_API virtual double getInstanceZPosition(const core::memory::buffer_object::sptr& _buffer_obj) const;

    /**
     * @brief Get the SliceThickness value from an instance.
     *  @param[in] bufferObj BufferObject containing the slice
     */
    FILTER_DICOM_API virtual double getSliceThickness(const core::memory::buffer_object::sptr& _buffer_obj) const;

    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;
};

} // namespace sight::filter::dicom::modifier
