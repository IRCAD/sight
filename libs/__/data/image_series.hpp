/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/data/config.hpp>

#include "data/dicom_series.hpp"
#include "data/factory/new.hpp"
#include "data/image.hpp"
#include "data/series.hpp"
#include "data/types.hpp"

#include "fiducials_series.hpp"
#include "has_fiducials.hpp"

namespace sight::data
{

class image;

} // namespace sight::data

namespace sight::data
{

/**
 * @brief Holds a medical images data.
 */
class SIGHT_DATA_CLASS_API image_series final : public image,
                                                public series,
                                                public has_fiducials
{
public:

    SIGHT_DECLARE_CLASS(image_series, image);

    /**
     * @brief Creates the series.
     */
    SIGHT_DATA_API image_series();

    /// Destroys the series.
    SIGHT_DATA_API ~image_series() noexcept override = default;

    /// Gets the DICOM reference used to generate valid Dicom Segmentation.
    dicom_series::csptr get_dicom_reference() const;

    /// Sets the DICOM reference used to generate valid Dicom Segmentation.
    void set_dicom_reference(const dicom_series::csptr& _reference);

    /// Getter/Setter of DICOM VOI LUT Module Module related attributes
    /// @note the definition is in Series.cpp
    /// @note Image also implements a getter/setter for this attribute that is overriden here
    /// @{
    SIGHT_DATA_API void set_window_center(const std::vector<double>& _window_centers) override;
    SIGHT_DATA_API std::vector<double> window_center() const noexcept override;
    SIGHT_DATA_API void set_window_width(const std::vector<double>& _window_widths) override;
    SIGHT_DATA_API std::vector<double> window_width() const noexcept override;
    /// @}

    /// Getter/Setter of DICOM Image Pixel Module
    /// @note the definition is in Series.cpp
    /// @note Image also somewhat implements a getter/setter for this attribute
    /// @{
    SIGHT_DATA_API void set_rows(const std::optional<std::uint16_t>& _rows       = std::nullopt) override;
    SIGHT_DATA_API void set_columns(const std::optional<std::uint16_t>& _columns = std::nullopt) override;
    /// @}

    /// Getter/Setter of DICOM Image Plane Module related attributes
    /// ...and Multi-frame Functional Groups Module
    /// @{
    SIGHT_DATA_API void set_image_position_patient(
        const std::vector<double>& _image_position_patient,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) override;

    /// Override the origin setter to also update DICOM image position patient
    SIGHT_DATA_API void set_origin(const origin_t& _origin) override;
    /// @}

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const image_series& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const image_series& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    /**
       @{
     * @brief Resize the image and allocate the memory if needed.
     *
     * @param _size array of size in each direction (x,y,z)
     * @param _type type of a single pixel component value
     * @param _format specify the ordering and the meaning of a pixel components
     *
     * If the data array owns its buffer, this method will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    SIGHT_DATA_API std::size_t resize(const size_t& _size, const core::type& _type, enum pixel_format _format) override;
    /// @}

    SIGHT_DATA_API fiducials_series::csptr get_fiducials() const;
    SIGHT_DATA_API fiducials_series::sptr get_fiducials();

private:

    /// Contains the DICOM reference used to generate a valid DICOM Segmentation.
    dicom_series::sptr m_dicom_reference;

    /// Contains the associated Spatial Fiducials file
    fiducials_series::sptr m_fiducials_series {std::make_shared<fiducials_series>()};
};

//-----------------------------------------------------------------------------

inline dicom_series::csptr image_series::get_dicom_reference() const
{
    return m_dicom_reference;
}

//-----------------------------------------------------------------------------

inline void image_series::set_dicom_reference(const dicom_series::csptr& _reference)
{
    m_dicom_reference = std::const_pointer_cast<dicom_series>(_reference);
}

} //end namespace sight::data.
