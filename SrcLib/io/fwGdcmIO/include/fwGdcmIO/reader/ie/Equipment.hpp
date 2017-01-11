/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_EQUIPMENT_HPP__
#define __FWGDCMIO_READER_IE_EQUIPMENT_HPP__

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwMedData/Equipment.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief Equipment Information Entity class
 */
class FWGDCMIO_CLASS_API Equipment : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Equipment >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read equipment.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] equipment Equipment data
     * @param[in] logger Logger
     * @param[in] callback Progress callback
     * @param[in] cancelled cancel information
     */
    FWGDCMIO_API Equipment(SPTR(::fwMedData::DicomSeries)dicomSeries,
                           SPTR(::gdcm::Reader)reader,
                           SPTR(::fwGdcmIO::container::DicomInstance)instance,
                           ::fwMedData::Equipment::sptr equipment,
                           ::fwLog::Logger::sptr logger,
                           const ProgressCallback& callback,
                           const bool& cancelled);

    /// Destructor
    FWGDCMIO_API virtual ~Equipment();

    /**
     * @brief Read General Equipment Module tags
     * @see PS 3.3 C.7.5.1
     */
    FWGDCMIO_API virtual void readGeneralEquipmentModule();


};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_IE_EQUIPMENT_HPP__ */
