/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_IMAGE_HPP__
#define __FWGDCMIO_READER_IE_IMAGE_HPP__

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

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
    FWGDCMIO_API Image(const SPTR(::fwMedData::DicomSeries)& dicomSeries,
                       const SPTR(::gdcm::Reader)& reader,
                       const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                       const ::fwData::Image::sptr& image,
                       const ::fwLog::Logger::sptr& logger = nullptr,
                       ProgressCallback progress = nullptr,
                       CancelRequestedCallback cancel = nullptr);

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
                          unsigned short bitsAllocated,
                          unsigned short newBitsAllocated,
                          bool performRescale) throw(::fwGdcmIO::exception::Failed);

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

#endif /* __FWGDCMIO_READER_IE_IMAGE_HPP__ */
