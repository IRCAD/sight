/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_INSTANCE_DICOMSR_HPP__
#define __GDCMIO_WRITER_INSTANCE_DICOMSR_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/writer/instance/DicomInstance.hxx"

namespace gdcmIO
{
namespace writer
{
namespace instance
{

/**
 * @brief   This class write one document SR with different contents.
 * It will call specific writers (like DicomLandmark) and complete
 * the data set of the document SR.
 *
 * @note    Currently, it just handles landmarks and distances writing.
 *
 * @class   DicomSR
 */
class GDCMIO_CLASS_API DicomSR : public DicomInstance< ::fwData::Image >
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::Writer.
     */
    GDCMIO_API DicomSR();

    GDCMIO_API virtual ~DicomSR();

    /**
     * @brief   Write the root SR container and add to it SR contents.
     *
     * @note    setDataSet() must be called before this method.
     *
     * @see     PS 3.3 form C.17.1
     */
    GDCMIO_API virtual void write() throw (::gdcmIO::exception::Failed);

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

} // namespace instance
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_INSTANCE_DICOMSR_HPP__ */
