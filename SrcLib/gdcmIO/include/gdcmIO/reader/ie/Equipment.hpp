/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_EQUIPMENT_HPP__
#define __GDCMIO_READER_IE_EQUIPMENT_HPP__

#include <fwMedData/Equipment.hpp>

#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Equipment
 * @brief Equipment Information Entity class
 */
class GDCMIO_CLASS_API Equipment : public ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Equipment >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read equipment.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] equipment Equipment data
     */
    GDCMIO_API Equipment(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                         SPTR(::gdcm::Reader)reader,
                         SPTR(::gdcmIO::container::DicomInstance)instance,
                         ::fwMedData::Equipment::sptr equipment);

    /// Destructor
    GDCMIO_API virtual ~Equipment();

    /**
     * @brief Read General Equipment Module tags
     * @see PS 3.3 C.7.5.1
     */
    GDCMIO_API virtual void readGeneralEquipmentModule();


};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_IE_EQUIPMENT_HPP__ */
