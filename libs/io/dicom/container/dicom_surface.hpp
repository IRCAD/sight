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

#include <sight/io/dicom/config.hpp>

#include <data/mesh.hpp>
#include <data/reconstruction.hpp>

#include <cstdint>

namespace sight::io::dicom::container
{

/**
 * @brief This class defines one surface mesh item in order to transform into DICOM/Sight form.
 */
class SIGHT_IO_DICOM_CLASS_API dicom_surface
{
public:

    /**
     * Typedef for cell index in DICOM world (32 bits see VR OL).
     * Sight uses uint64_t (see data::mesh::CellId).
     */
    using dicom_cell_value_t = std::uint32_t;

    /**
     * @brief Container types to store points, cells and normals.
     * @{ */
    using dicom_point_buffer_t  = std::vector<float>;
    using dicom_cell_buffer_t   = std::vector<dicom_cell_value_t>;
    using dicom_normal_buffer_t = std::vector<float>;
    /**  @} */

    /**
     * @brief Constructor
     * @param[in] _reconstruction Source reconstruction
     */
    SIGHT_IO_DICOM_API dicom_surface(const data::reconstruction::csptr& _reconstruction);

    /**
     * @brief Constructor
     * @param[in] _point_buffer Points buffer
     * @param[in] _point_buffer_size Points buffer size
     * @param[in] _cell_buffer Cells buffer
     * @param[in] _cell_buffer_size Cells buffer size
     * @param[in] _normal_buffer Normals buffer
     */
    SIGHT_IO_DICOM_API dicom_surface(
        const data::mesh::position_t* _point_buffer,
        data::mesh::size_t _point_buffer_size,
        const dicom_cell_value_t* _cell_buffer,
        data::mesh::size_t _cell_buffer_size,
        const data::mesh::normal_t* _normal_buffer
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~dicom_surface();

    /// Convert DicomSurface container to Sight Mesh
    SIGHT_IO_DICOM_API data::mesh::sptr convert_to_data();

    /// Return point coordinates buffer
    [[nodiscard]] SIGHT_IO_DICOM_API const dicom_point_buffer_t& get_point_buffer() const;
    /// Return cell buffer
    [[nodiscard]] SIGHT_IO_DICOM_API const dicom_cell_buffer_t& get_cell_buffer() const;

    /// Return normal buffer
    [[nodiscard]] SIGHT_IO_DICOM_API const dicom_normal_buffer_t& get_normal_buffer() const;

    /// Return point coordinates buffer size
    [[nodiscard]] SIGHT_IO_DICOM_API std::size_t get_point_buffer_size() const;

    /// Return cell buffer size
    [[nodiscard]] SIGHT_IO_DICOM_API std::size_t get_cell_buffer_size() const;

    /// Return normal buffer size
    [[nodiscard]] SIGHT_IO_DICOM_API std::size_t get_normal_buffer_size() const;

private:

    /**
     * @brief Surface Points Coordinates
     * (List of points coordinates for one surface) (eg : x1,y1,z1, x2,y2,z2, ...)
     */
    dicom_point_buffer_t m_point_buffer;

    /**
     * @brief Surface Mesh Primitives
     * (List of points index for all triangle of one surface) (eg : p1,p3,p2, p2,p3,p4, ...).
     *
     * Primitives will be written in Long Triangle Point Index List (0066,0041)
     * which has a VR equal to OL. VR::OL is a string of 32-bit words.
     */
    dicom_cell_buffer_t m_cell_buffer;

    /**
     * @brief Surface Point Normal Coordinates
     * (List of point normal coordinates for one surface) (eg : x1,y1,z1, x2,y2,z2, ...).
     */
    dicom_normal_buffer_t m_normal_buffer;
};

} // namespace sight::io::dicom::container
