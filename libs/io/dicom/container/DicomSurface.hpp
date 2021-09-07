/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/config.hpp"

#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

#include <cstdint>

namespace sight::io::dicom
{

namespace container
{

/**
 * @brief This class defines one surface mesh item in order to transform into DICOM/Sight form.
 */
class IO_DICOM_CLASS_API DicomSurface
{
public:

    /**
     * Typedef for cell index in DICOM world (32 bits see VR OL).
     * Sight uses uint64_t (see data::Mesh::CellId).
     */
    typedef std::uint32_t DicomCellValueType;

    /**
     * @brief Container types to store points, cells and normals.
     * @{ */
    typedef std::vector<float> DicomPointBufferType;
    typedef std::vector<DicomCellValueType> DicomCellBufferType;
    typedef std::vector<float> DicomNormalBufferType;
    /**  @} */

    /**
     * @brief Constructor
     * @param[in] reconstruction Source reconstruction
     */
    IO_DICOM_API DicomSurface(const data::Reconstruction::csptr& reconstruction);

    /**
     * @brief Constructor
     * @param[in] pointBuffer Points buffer
     * @param[in] pointBufferSize Points buffer size
     * @param[in] cellBuffer Cells buffer
     * @param[in] cellBufferSize Cells buffer size
     * @param[in] normalBuffer Normals buffer
     */
    IO_DICOM_API DicomSurface(
        const data::Mesh::PointValueType* pointBuffer,
        const data::Mesh::Size pointBufferSize,
        const DicomCellValueType* cellBuffer,
        const data::Mesh::Size cellBufferSize,
        const data::Mesh::NormalValueType* normalBuffer
    );

    /// Destructor
    IO_DICOM_API ~DicomSurface();

    /// Convert DicomSurface container to Sight Mesh
    IO_DICOM_API data::Mesh::sptr convertToData();

    /// Return point coordinates buffer
    IO_DICOM_API const DicomPointBufferType& getPointBuffer() const;
    /// Return cell buffer
    IO_DICOM_API const DicomCellBufferType& getCellBuffer() const;

    /// Return normal buffer
    IO_DICOM_API const DicomNormalBufferType& getNormalBuffer() const;

    /// Return point coordinates buffer size
    IO_DICOM_API std::size_t getPointBufferSize() const;

    /// Return cell buffer size
    IO_DICOM_API std::size_t getCellBufferSize() const;

    /// Return normal buffer size
    IO_DICOM_API std::size_t getNormalBufferSize() const;

private:

    /**
     * @brief Surface Points Coordinates
     * (List of points coordinates for one surface) (eg : x1,y1,z1, x2,y2,z2, ...)
     */
    DicomPointBufferType m_pointBuffer;

    /**
     * @brief Surface Mesh Primitives
     * (List of points index for all triangle of one surface) (eg : p1,p3,p2, p2,p3,p4, ...).
     *
     * Primitives will be written in Long Triangle Point Index List (0066,0041)
     * which has a VR equal to OL. VR::OL is a string of 32-bit words.
     */
    DicomCellBufferType m_cellBuffer;

    /**
     * @brief Surface Point Normal Coordinates
     * (List of point normal coordinates for one surface) (eg : x1,y1,z1, x2,y2,z2, ...).
     */
    DicomNormalBufferType m_normalBuffer;
};

} //namespace container

} //namespace sight::io::dicom
