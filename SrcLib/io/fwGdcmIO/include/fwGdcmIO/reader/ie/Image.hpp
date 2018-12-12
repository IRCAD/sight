/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "fwGdcmIO/exception/Failed.hpp"
#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwData/Image.hpp>

#include <boost/numeric/ublas/matrix.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief Image Information Entity class
 */
class FWGDCMIO_CLASS_API Image : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API Image(const CSPTR(::fwMedData::DicomSeries)& dicomSeries,
                       const SPTR(::gdcm::Reader)& reader,
                       const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                       const ::fwData::Image::sptr& image,
                       const ::fwLog::Logger::sptr& logger = nullptr,
                       ProgressCallback progress           = nullptr,
                       CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Image();

    /**
     * @brief Read Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    FWGDCMIO_API virtual void readImagePlaneModule();

    /**
     * @brief Read Image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    FWGDCMIO_API virtual void readImagePixelModule();

    /**
     * @brief Read VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    FWGDCMIO_API virtual void readVOILUTModule();

    /// Enable buffer rotation
    void setBufferRotationEnabled(bool enabled)
    {
        m_enableBufferRotation = enabled;
    }

protected:

    typedef ::boost::numeric::ublas::matrix< double, ::boost::numeric::ublas::row_major,
                                             std::vector<double> > MatrixType;
    typedef ::boost::numeric::ublas::vector< double > VectorType;

    /**
     * @brief Read image buffer
     * @param[in] dimensions Image dimensions
     * @param[in] bitsAllocated Number of bits allocated before rescale
     * @param[in] newBitsAllocated Number of bits allocated after rescale
     * @param[in] performRescale Set to true when a rescale must be perfromed
     * @return Global raw buffer of the image.
     */
    char* readImageBuffer(const std::vector<unsigned int>& dimensions,
                          const unsigned short bitsAllocated,
                          const unsigned short newBitsAllocated,
                          const bool performRescale) throw(::fwGdcmIO::exception::Failed);

    /**
     * @brief Correct image buffer according to image orientation
     * @param[in,out] buffer Image buffer
     * @param[in] dimensions Image dimensions
     * @param[in] bitsAllocated Number of bits allocated before rescale
     */
    char* correctImageOrientation(char* buffer, const std::vector<unsigned int>& dimensions,
                                  unsigned short bitsAllocated);

    /**
     * @brief Compute the inverse of matrix
     * @param[in] matrix Matrix that must be inverted
     */
    MatrixType computeInverseMatrix(MatrixType matrix);

    /// Enable buffer rotation
    bool m_enableBufferRotation;
};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
