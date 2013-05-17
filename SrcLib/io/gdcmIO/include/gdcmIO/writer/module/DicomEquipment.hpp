/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMEQUIPMENT_HPP__
#define __GDCMIO_WRITER_MODULE_DICOMEQUIPMENT_HPP__

#include <fwData/macros.hpp>

#include <fwMedData/Equipment.hpp>

#include <fwTools/DynamicType.hpp>

#include "gdcmIO/writer/module/DicomModule.hxx"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class write equipment modules.
 *
 * It can write the General Equipment Module and the enhanced one.
 *
 * @see     DICOM PS 3.3 C.7.5.1
 *
 * @class   DicomEquipment.
 */
class GDCMIO_CLASS_API DicomEquipment : public DicomModule< ::fwMedData::Equipment >
{

public:

    GDCMIO_API DicomEquipment();

    GDCMIO_API virtual ~DicomEquipment();

    /**
     * @brief   Write equipment data into a data set.
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API virtual void write(::gdcm::DataSet & a_gDs);

    /**
     * @brief   Write Enhanced General Equipment Module.
     *
     * @see     PS 3.3 C.7.5.2
     *
     * @param   a_gDs   Root of a data set for a DICOM file.
     */
    GDCMIO_API void writeEnhancedGeneralEquipement(::gdcm::DataSet & a_gDs);


    GDCMIO_API fwGettersSettersDocMacro(IsEnhanced, isEnhanced, bool, Defined if the requiered module is enhanced or not);

private:

    /**
     * - if true : write Enhanced General Equipment Module (PS 3.3 C.7.5.2) and General Equipment Module.
     * - if false : write General Equipment Module  (PS 3.3 C.7.5.1).
     */
    bool  m_isEnhanced;

};

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_MODULE_DICOMEQUIPMENT_HPP__ */
