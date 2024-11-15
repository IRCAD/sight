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

    /// Getter/Setter of DICOM Image Plane Module related attributes
    /// ...and Multi-frame Functional Groups Module
    /// @warning column major order
    /// @{
    SIGHT_DATA_API void set_image_orientation_patient(
        const std::vector<double>& _image_orientation_patient,
        const std::optional<std::size_t>& _frame_index = std::nullopt
    ) override;

    /// Override the origin setter to also update DICOM image position patient (row major)
    SIGHT_DATA_API void set_orientation(const orientation_t& _orientation) override;
    /// @}

    /// Overriden methods used to also set fiducial data
    /// @{
    SIGHT_DATA_API void set_patient_id(const std::string& _patient_id) override;
    SIGHT_DATA_API void set_patient_name(const std::string& _patient_name) override;
    SIGHT_DATA_API void set_study_instance_uid(const std::string& _study_instance_uid) override;
    /// @}

    // Overriden to make sure the spacing matches the set thickness
    SIGHT_DATA_API void set_slice_thickness(const std::optional<double>& _slice_thickness = std::nullopt) override;

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

    /**
     * @brief helper function to convert back and to dicom orientation.
     *
     * @note DICOM uses only 6 direction cosines in column major order. We use 9 in row major order.
     *
     * @{
     */
    inline static std::vector<double> to_dicom_orientation(const orientation_t& _orientation);
    inline static orientation_t from_dicom_orientation(const std::vector<double>& _orientation);
    /// @}

private:

    /// Contains the DICOM reference used to generate a valid DICOM Segmentation.
    dicom_series::sptr m_dicom_reference;
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

//------------------------------------------------------------------------------

inline std::vector<double> image_series::to_dicom_orientation(const image::orientation_t& _orientation)
{
    return {
        _orientation[0], _orientation[3], _orientation[6],
        _orientation[1], _orientation[4], _orientation[7]
    };
}

//------------------------------------------------------------------------------

inline image::orientation_t image_series::from_dicom_orientation(const std::vector<double>& _orientation)
{
    const glm::dvec3 u(_orientation[0], _orientation[1], _orientation[2]);
    const glm::dvec3 v(_orientation[3], _orientation[4], _orientation[5]);
    const glm::dvec3 w = glm::cross(u, v);

    return {
        _orientation[0], _orientation[3], w[0],
        _orientation[1], _orientation[4], w[1],
        _orientation[2], _orientation[5], w[2],
    };
}

} //end namespace sight::data.
