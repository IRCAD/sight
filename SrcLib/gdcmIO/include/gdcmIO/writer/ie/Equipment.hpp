/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_EQUIPMENT_HPP__
#define __GDCMIO_WRITER_IE_EQUIPMENT_HPP__

#include <fwMedData/Equipment.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class Equipment
 * @brief Equipment Information Entity class
 */
class GDCMIO_CLASS_API Equipment : public ::gdcmIO::writer::ie::InformationEntity< ::fwMedData::Equipment >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] equipment Equipment data
     */
    GDCMIO_API Equipment(SPTR(::gdcm::Writer)writer,
                         SPTR(::gdcmIO::container::DicomInstance)instance,
                         ::fwMedData::Equipment::sptr equipment);

    /// Destructor
    GDCMIO_API virtual ~Equipment();

    /**
     * @brief Write General Equipment Module tags
     * @see PS 3.3 C.7.5.1
     */
    GDCMIO_API virtual void writeGeneralEquipmentModule();

    /**
     * @brief Write Enhanced General Equipment Module tags
     * @see PS 3.3 C.7.5.2
     */
    GDCMIO_API virtual void writeEnhancedGeneralEquipmentModule();


};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_IE_EQUIPMENT_HPP__ */
