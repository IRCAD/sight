/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMRTWRITERMANAGER_HPP_
#define _GDCMIO_DICOMRTWRITERMANAGER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwData/Acquisition.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class handles RT documents.
 *
 * It will load and start appropriated writing tools.
 *
 * @note    Currently, it just handles Surface Segmentation Storage.
 *
 * @class   DicomRTWriterManager
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomRTWriterManager : public DicomModuleWriter< ::fwData::Acquisition >
{

public :

    GDCMIO_API DicomRTWriterManager();

    GDCMIO_API ~DicomRTWriterManager();

    /**
     * @brief   Load and start appropriate image writing tools.
     *
     * This is one last manager of the writing chain. The loaded tools will
     * write one or more files.
     *
     * @note    Currently, it just writes one RT document.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     * @param   a_path  Folder path where RT document will be saved.
     */
    GDCMIO_API void write(::gdcm::DataSet &                 a_gDs,
                          const ::boost::filesystem::path & a_path) throw(::fwTools::Failed);

private :

    /**
     * @brief   Return true if the Acquisition has reconstruction and mesh.
     */
    bool hasSurface();

};

}   // namespace writer

}   // namespace gdcmIO

#endif // _GDCMIO_DICOMRTWRITERMANAGER_HPP_
