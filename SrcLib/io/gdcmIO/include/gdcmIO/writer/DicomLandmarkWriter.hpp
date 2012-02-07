/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMLANDMARKWRITER_HPP_
#define _GDCMIO_DICOMLANDMARKWRITER_HPP_

#include <fwData/Image.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"
#include "gdcmIO/DicomLandmark.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class define a writer of landmarks.
 *
 * It can write an appropriated container and insert landmark(s) into it
 * following DICOM SR Document norm.
 *
 * @class   DicomLandmarkWriter
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomLandmarkWriter : public DicomModuleWriter< fwData::Image >
{

public :

    /**
     * @brief   Constructor.
     */
    GDCMIO_API DicomLandmarkWriter();

    GDCMIO_API virtual ~DicomLandmarkWriter();

    /**
     * @brief   Write landmark(s) into a DICOM data set.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API void write(::gdcm::DataSet & a_gDs) throw (::fwTools::Failed);

private :

    /**
     * @brief    Write one landmark in a separate Structured Reporting form.
     *
     * @param   idx     Index of landmark
     * @param   a_gSq   Sequence of items where new landmark will be inserted
     */
    void writeLandmark(const unsigned int                               idx,
                       ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >  a_gSq);


    ::boost::shared_ptr< DicomLandmark > m_landmarks;   ///< Landmarks container and converter
};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMLANDMARKWRITER_HPP_ */
