/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSERIESWRITER_HPP_
#define _GDCMIO_DICOMSERIESWRITER_HPP_

#include <fwData/macros.hpp>
#include <fwData/Study.hpp>

#include <fwTools/DynamicType.hpp>

#include "gdcmIO/writer/DicomModuleWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class write equipment modules.
 *
 * It can write the General Equipment Module and the enhanced one.
 *
 * @see     DICOM PS 3.3 C.7.5.1
 *
 * @class   DicomEquipmentWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomEquipmentWriter : public DicomModuleWriter< ::fwData::Study >
{

public:

    GDCMIO_API DicomEquipmentWriter();

    GDCMIO_API virtual ~DicomEquipmentWriter();

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

    GDCMIO_API fwGettersSettersDocMacro(InstanceHasImage, instanceHasImage, bool, Defined if instance has an image and so pixel padding has to be specified);

    /**
     * @brief   Set the pixel type of the image taken from the equipment.
     */
    GDCMIO_API void setImagePyxelType(const ::fwTools::DynamicType & a_type);

private:

//    short getPixelPaddingValue() const;
    unsigned short getPixelPaddingValue() const { return 0; } // FIXME: hack me

    bool                    m_isEnhanced;       ///< if true : write Enhanced General Equipment Module (PS 3.3 C.7.5.2) and General Equipment Module
                                                ///< if false : write General Equipment Module  (PS 3.3 C.7.5.1)

    bool                    m_instanceHasImage; ///< Flag which defines if instance has an image.

    ::fwTools::DynamicType  m_type;             ///< Pixel type of an image
};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMSERIESWRITER_HPP_ */
