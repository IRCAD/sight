/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMLANDMARKREADER_HPP_
#define _GDCMIO_DICOMLANDMARKREADER_HPP_

#include <gdcmIO/reader/DicomInstanceReader.hxx>
#include <gdcmIO/DicomLandmark.hpp>

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class define a reader of landmark(s) container taken from a DICOM file.
 *
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is based on TID 320 (taken from PS 3.16).
 *
 * @class   DicomLandmarkReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomLandmarkReader : public DicomInstanceReader< fwData::Image >
{

public :

    GDCMIO_API DicomLandmarkReader();

    GDCMIO_API ~DicomLandmarkReader();

    /**
     * @brief   Read landmark(s) from a DICOM file.
     *
     * @note    setReader() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::fwTools::Failed);

private :

    /**
     * @brief    Read landmark(s) from its data set.
     * It will read one landmark with a non contiguous content.
     *
     * @param   a_ds            Data set of landmark.
     * @param   a_continuity    Continuity of content.
     */
    void readLandmark(const ::gdcm::DataSet & a_ds, const bool a_continuity = false) throw (::fwTools::Failed);


    ::boost::shared_ptr< DicomLandmark > m_dicomLandmarks;  ///< Store and transform into fwData
};

}  // namespace reader

}  // namespace gdcmIO

#endif /* _GDCMIO_DICOMLANDMARKREADER_HPP_ */
