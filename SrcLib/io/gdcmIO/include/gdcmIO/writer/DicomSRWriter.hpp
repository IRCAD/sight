/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSRWRITER_HPP_
#define _GDCMIO_DICOMSRWRITER_HPP_

#include <fwData/Image.hpp>

#include "gdcmIO/writer/DicomInstanceWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class write one document SR with different contents.
 * It will call specific writers (like DicomLandmarkWriter) and complete
 * the data set of the document SR.
 *
 * @note    Currently, it just handles landmarks and distances writing.
 *
 * @class   DicomSRWriter
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSRWriter : public DicomInstanceWriter< ::fwData::Image >
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::Writer.
     */
    GDCMIO_API DicomSRWriter();

    GDCMIO_API virtual ~DicomSRWriter();

    /**
     * @brief   Write the root SR container and add to it SR contents.
     *
     * @note    setDataSet() must be called before this method.
     *
     * @see     PS 3.3 form C.17.1
     */
    GDCMIO_API virtual void write() throw (::fwTools::Failed);

private :

    /**
     * @brief   Write SR Document Series Module.
     *
     * @see     PS 3.3 C.17.1
     */
    void writeSRDocumentSeries();

    /**
     * @brief   Write SR Document General Module.
     *
     * @see     PS 3.3 C.17.2
     */
    void writeSRDocumentGeneral();

};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMSRWRITER_HPP_ */
