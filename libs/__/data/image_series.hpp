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

#pragma once

#include "data/config.hpp"
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
class DATA_CLASS_API image_series final : public image,
                                          public series,
                                          public has_fiducials
{
public:

    SIGHT_DECLARE_CLASS(image_series, image);

    /**
     * @brief Creates the series.
     * @param _key private construction key.
     */
    DATA_API image_series();

    /// Destroys the series.
    DATA_API ~image_series() noexcept override = default;

    /// Gets the DICOM reference used to generate valid Dicom Segmentation.
    dicom_series::csptr get_dicom_reference() const;

    /// Sets the DICOM reference used to generate valid Dicom Segmentation.
    void set_dicom_reference(const dicom_series::csptr& _reference);

    /// Getter/Setter of DICOM VOI LUT Module Module related attributes
    /// @note the definition is in Series.cpp
    /// @note Image also implements a getter/setter for this attribute that is overriden here
    /// @{
    DATA_API void set_window_center(const std::vector<double>& _window_centers) override;
    DATA_API std::vector<double> window_center() const noexcept override;
    DATA_API void set_window_width(const std::vector<double>& _window_widths) override;
    DATA_API std::vector<double> window_width() const noexcept override;
    /// @}

    /// Getter/Setter of DICOM Image Pixel Module
    /// @note the definition is in Series.cpp
    /// @note Image also somewhat implements a getter/setter for this attribute
    /// @{
    DATA_API void set_rows(const std::optional<std::uint16_t>& _rows       = std::nullopt) override;
    DATA_API void set_columns(const std::optional<std::uint16_t>& _columns = std::nullopt) override;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const image_series& _other) const noexcept;
    DATA_API bool operator!=(const image_series& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    /**
       @{
     * @brief Resize the image and allocate the memory if needed.
     *
     * @param size array of size in each direction (x,y,z)
     * @param type type of a single pixel component value
     * @param format specify the ordering and the meaning of a pixel components
     *
     * If the data array owns its buffer, this method will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    DATA_API std::size_t resize(const size_t& _size, const core::type& _type, enum pixel_format _format) override;
    /// @}

    DATA_API fiducials_series::csptr get_fiducials() const;
    DATA_API fiducials_series::sptr get_fiducials();

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
