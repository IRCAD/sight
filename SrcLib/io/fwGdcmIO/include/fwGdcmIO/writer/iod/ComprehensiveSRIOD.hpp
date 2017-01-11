/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IOD_COMPREHENSIVESRIOD_HPP__
#define __FWGDCMIO_WRITER_IOD_COMPREHENSIVESRIOD_HPP__

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
 * @brief ComprehensiveSRIOD class used to write Enhanced Structured Report DICOM files
 */
class FWGDCMIO_CLASS_API ComprehensiveSRIOD : public ::fwGdcmIO::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share informations between modules
     */
    FWGDCMIO_API ComprehensiveSRIOD(SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                    ::boost::filesystem::path folderPath, bool use3DSR = false);

    /// Destructor
    FWGDCMIO_API virtual ~ComprehensiveSRIOD();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(::fwMedData::Series::sptr series);

protected:
    /// True if we must use 3DSR
    bool m_use3DSR;

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IOD_COMPREHENSIVESRIOD_HPP__
