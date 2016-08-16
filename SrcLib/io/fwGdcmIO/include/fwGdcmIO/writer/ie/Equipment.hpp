/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IE_EQUIPMENT_HPP__
#define __FWGDCMIO_WRITER_IE_EQUIPMENT_HPP__

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <fwMedData/Equipment.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Equipment Information Entity class
 */
class FWGDCMIO_CLASS_API Equipment : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Equipment >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] equipment Equipment data
     */
    FWGDCMIO_API Equipment(SPTR(::gdcm::Writer)writer,
                           SPTR(::fwGdcmIO::container::DicomInstance)instance,
                           ::fwMedData::Equipment::sptr equipment);

    /// Destructor
    FWGDCMIO_API virtual ~Equipment();

    /**
     * @brief Write General Equipment Module tags
     * @see PS 3.3 C.7.5.1
     */
    FWGDCMIO_API virtual void writeGeneralEquipmentModule();

    /**
     * @brief Write Enhanced General Equipment Module tags
     * @see PS 3.3 C.7.5.2
     */
    FWGDCMIO_API virtual void writeEnhancedGeneralEquipmentModule();


};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_WRITER_IE_EQUIPMENT_HPP__ */
