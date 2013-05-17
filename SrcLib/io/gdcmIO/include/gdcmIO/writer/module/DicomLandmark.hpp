/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMLANDMARK_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMLANDMARK_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/writer/module/DicomModule.hxx"
#include "gdcmIO/container/DicomLandmark.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class define a writer of landmarks.
 *
 * It can write an appropriated container and insert landmark(s) into it
 * following DICOM SR Document norm.
 *
 * @class   DicomLandmark
 */
class GDCMIO_CLASS_API DicomLandmark : public DicomModule< ::fwData::Image >
{

public :

    /**
     * @brief   Constructor.
     */
    GDCMIO_API DicomLandmark();

    GDCMIO_API virtual ~DicomLandmark();

    /**
     * @brief   Write landmark(s) into a DICOM data set.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API void write(::gdcm::DataSet & a_gDs) throw (::gdcmIO::exception::Failed);

private :

    /**
     * @brief    Write one landmark in a separate Structured Reporting form.
     *
     * @param   idx     Index of landmark
     * @param   a_gSq   Sequence of items where new landmark will be inserted
     */
    void writeLandmark(const unsigned int                               idx,
                       ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >  a_gSq);


    ::boost::shared_ptr< ::gdcmIO::container::DicomLandmark > m_landmarks;   ///< Landmarks container and converter
};

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_MODULE_DICOMLANDMARK_HPP__ */
