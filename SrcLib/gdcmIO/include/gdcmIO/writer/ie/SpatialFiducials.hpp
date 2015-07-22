/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_SPATIALFIDUCIALS_HPP__
#define __GDCMIO_WRITER_IE_SPATIALFIDUCIALS_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class SpatialFiducials
 * @brief Spatial Fiducials Information Entity class
 */
class GDCMIO_CLASS_API SpatialFiducials : public ::gdcmIO::writer::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     */
    GDCMIO_API SpatialFiducials(SPTR(::gdcm::Writer)writer,
                                SPTR(::gdcmIO::container::DicomInstance)instance,
                                ::fwData::Image::sptr series);

    /// Destructor
    GDCMIO_API virtual ~SpatialFiducials();

    /**
     * @brief Write Spatial Fiducials Module tags
     * @see PS 3.3 C.21.2
     */
    GDCMIO_API virtual void writeSpatialFiducialsModule();

    /**
     * @brief Write Common Instance Reference Module tags
     * @see PS 3.3 C.12.2
     */
    GDCMIO_API virtual void writeCommonInstanceReferenceModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    GDCMIO_API virtual void writeSOPCommonModule();

protected:

    /**
     * @brief Write image landmarks into sequence
     */
    void writeLandmarks(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence) throw(::gdcmIO::exception::Failed);

    /**
     * @brief Write image distances into sequence
     */
    void writeDistances(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence);

    /**
     * @brief Add a frame number to the referenced image sequence
     * @param[in] frameNumber Frame Number
     * @param[in] referencedImageSequence Destination sequence
     */
    void addReferencedImage(int frameNumber,
                            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > referencedImageSequence) throw(::gdcmIO::
                                                                                                           exception::
                                                                                                           Failed);
};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_IE_SPATIALFIDUCIALS_HPP__
