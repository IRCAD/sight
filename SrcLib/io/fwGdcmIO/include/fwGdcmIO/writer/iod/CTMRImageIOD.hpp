/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IOD_CTMRIMAGEIOD_HPP__
#define __FWGDCMIO_WRITER_IOD_CTMRIMAGEIOD_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/writer/iod/InformationObjectDefinition.hpp"

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

/**
 * @brief CTMRImageIOD class used to write CTImage and MRImage DICOM files
 */
class FWGDCMIO_CLASS_API CTMRImageIOD : public ::fwGdcmIO::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     */
    FWGDCMIO_API CTMRImageIOD(SPTR(::fwGdcmIO::container::DicomInstance)instance, ::boost::filesystem::path folderPath);

    /// Destructor
    FWGDCMIO_API virtual ~CTMRImageIOD();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(::fwMedData::Series::sptr series);

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IOD_CTMRIMAGEIOD_HPP__
