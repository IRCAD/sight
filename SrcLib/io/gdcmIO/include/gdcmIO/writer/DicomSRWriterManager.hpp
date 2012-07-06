/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSRWRITERMANAGER_HPP_
#define _GDCMIO_DICOMSRWRITERMANAGER_HPP_

#include <fwData/Image.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class handles SR content to save.
 *
 * It will call SR writer.
 *
 * @class   DicomSRWriterManager
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSRWriterManager : public DicomModuleWriter< ::fwData::Image >
{

public :

    GDCMIO_API DicomSRWriterManager();

    GDCMIO_API ~DicomSRWriterManager();

    /**
     * @brief   Load and start appropriate SR writing tools.
     *
     * This is one of last managers of the writing chain. The loaded tools will
     * write one file.
     *
     * @note    Currently, it just writes one SR document.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     * @param   a_path  Folder path where SR document will be saved.
     */
    GDCMIO_API void write(::gdcm::DataSet & a_gDs, const ::boost::filesystem::path & a_path);

};

}   // namespace writer

}   // namespace gdcmIO

#endif // _GDCMIO_DICOMSRWRITERMANAGER_HPP_
