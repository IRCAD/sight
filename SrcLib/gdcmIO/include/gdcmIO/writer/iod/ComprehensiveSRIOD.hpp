/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IOD_COMPREHENSIVESRIOD_HPP__
#define __GDCMIO_WRITER_IOD_COMPREHENSIVESRIOD_HPP__

#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/writer/iod/InformationObjectDefinition.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace writer
{
namespace iod
{

/**
 * @class ComprehensiveSRIOD
 * @brief ComprehensiveSRIOD class used to write Enhanced Structured Report DICOM files
 */
class GDCMIO_CLASS_API ComprehensiveSRIOD : public ::gdcmIO::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share informations between modules
     */
    GDCMIO_API ComprehensiveSRIOD(SPTR(::gdcmIO::container::DicomInstance)instance,
                                  ::boost::filesystem::path folderPath, bool use3DSR = false);

    /// Destructor
    GDCMIO_API virtual ~ComprehensiveSRIOD();

    /// Write DICOM file
    GDCMIO_API virtual void write(::fwMedData::Series::sptr series);

protected:
    /// True if we must use 3DSR
    bool m_use3DSR;

};

} // namespace iod
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_IOD_COMPREHENSIVESRIOD_HPP__
