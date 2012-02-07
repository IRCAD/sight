/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSRREADER_HPP_
#define _GDCMIO_DICOMSRREADER_HPP_

#include <fwData/Image.hpp>

#include "gdcmIO/reader/DicomInstanceReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class define a reader of SR (Structured Report) document.
 *
 * It will launch specific SR readers on a same DICOM file.
 *
 * @class   DicomSRReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSRReader : public DicomInstanceReader< ::fwData::Image >
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::Reader.
     */
    GDCMIO_API DicomSRReader();

    GDCMIO_API ~DicomSRReader();

    /**
     * @brief   Read one document SR.
     *
     * @note    Currently, it just handles landmarks and distances reading.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::fwTools::Failed);

};

}  // namespace reader

}  // namespace gdcmIO

#endif /* _GDCMIO_DICOMSRREADER_HPP_ */
