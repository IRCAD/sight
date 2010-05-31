/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Contributors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef _FWCOMED_FIELDHELPER_MEDICALIMAGEHELPERS_HPP_
#define _FWCOMED_FIELDHELPER_MEDICALIMAGEHELPERS_HPP_

#include <utility> // std::pair

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/TransfertFunction.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @namespace   ::fwComEd::fieldHelper
 * @brief       This namespace contains helpers for fields.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 */
namespace fieldHelper
{

/**
 * @class   MedicalImageHelpers
 * @brief   This class contains helpers for medical image fields.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class FWCOMED_CLASS_API MedicalImageHelpers
{
public :

    /**
     * @brief       Check transfer function and min/max fields
     *
     * If the image has not transfer function, window min and window max fields, they are added.
     *
     * If the transfer function min and max were not the same as window min and window max, they were modified.
     *
     * @param[in]   _pImg   image which must contain the fields.
     *
     * @return      Return.first == true if we add or modify Min/Max field
     * @return      Return.second == true if we add or modify TF field
     */
    FWCOMED_API static std::pair<bool,bool> checkMinMaxTF( ::fwData::Image::sptr _pImg );


    /**
     * @brief       Update window min and max objects from transfer function.
     * @param[in]   min min data object
     * @param[in]   max max data object
     * @param[in]   tF  Transfert function data object
     */
    FWCOMED_API static void updateMinMaxFromTF( ::fwData::Integer::sptr min, ::fwData::Integer::sptr max, ::fwData::TransfertFunction::sptr tF);

    /**
     * @brief       Update window min and max fields from transfer function field.
     * @param[in]   _pImg   image which contains the fields.
     */
    FWCOMED_API static void updateMinMaxFromTF( ::fwData::Image::sptr _pImg );


    /**
     * @brief       Update transfer function min/max from min/max objects.
     * @param[in]   min min data object
     * @param[in]   max max data object
     * @param[in]   tF  Transfert function data object
     */
    FWCOMED_API static void updateTFFromMinMax( ::fwData::Integer::sptr min, ::fwData::Integer::sptr max, ::fwData::TransfertFunction::sptr tF);

    /**
     * @brief       Update transfer function min/max from window min/max fields.
     * @param[in]   _pImg   image which contains the fields.
     */
    FWCOMED_API static void updateTFFromMinMax( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image has a landmark field.
     *
     * If the image hasn't landmark field, it is added and the method return true.
     *
     * @param[in]   _pImg   image which must contains the field.
     * @return      true if the field is added.
     */
    FWCOMED_API static bool checkLandmarks( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image is valid.
     *
     * The image size must be > 1 for each dimension.
     *
     * @param[in]   _pImg   image checked.
     * @return      true if the image is valid.
     */
    FWCOMED_API static bool checkImageValidity( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image has slice index fields.
     *
     * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
     *
     * @param       _pImg   image which must contains the field.
     * @return      true if the fields are added.
     */
    FWCOMED_API static bool checkImageSliceIndex( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Get the X,Y,Z slice position.
     * @param[in]   _pImg   current image.
     * @return      X,Y,Z  slice position of the current image if exist field exist
     *              else install field and return (imgSizeX/2,imgSizeY/2,imgSizeZ/2).
     */
    FWCOMED_API static ::fwData::Point::sptr   getImageSliceIndices( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Set default transfer function (grey level).
     *
     * If the default transfer function doesn't exist, it is created.
     *
     * @param[in]   _pImg image which contains the transfer function field.
     */
    FWCOMED_API static void setBWTF( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image has a comment field.
     *
     * If the image has no comment field, this is added with default name ("Original image").
     *
     * @param[in]   _pImg image which contains the comment field.
     * @return      true if the comment field is added.
     */
    FWCOMED_API static bool checkComment( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check window min/max and transfer function field. And set default transfer function.
     *
     * Check if the transfer function min and max are the same has window min and window max. And modify it if necessary.
     *
     * Set the default transfer function (grey level).
     *
     * @param[in]   _pImg   image which must contain the fields.
     *
     * @return      Return.first == true if we add or modify Min/Max field
     * @return      Return.second == true if we add or modify TF field
     */
    FWCOMED_API static std::pair<bool, bool> checkMinMaxTFAndSetBWTF( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check window min/max and transfer function field and set default transfer function for each image in the patientDB.
     *
     * Check if the transfer function min and max are the same has window min and window max. And modify it if necessary.
     *
     * Set the default transfer function (grey level).
     *
     * @param[in]   _pPatientDB     patientDB containing images.
     *
     * @return      Return.first == true if we add or modify Min/Max field
     * @return      Return.second == true if we add or modify TF field
     */
    FWCOMED_API static std::pair<bool, bool> checkMinMaxTFAndSetBWTF( ::fwData::PatientDB::sptr _pPatientDB );

    /**
     * @brief       Set a label in a field of the image.
     *
     * For each image in the patient the label is incremented (I0, I1, I2,...).
     *
     * @param[in]   pPatient    patient which contains the image.
     * @param[in]   pImage      image to set the label.
     */
    FWCOMED_API static void setImageLabel( ::fwData::Patient::sptr pPatient, ::fwData::Image::sptr pImage);

    /**
     * @brief       Set an opacity in a (Float) field of the image.
     *
     * Notion of opacity. Used for negatoscope management at first time.
     * The opacity is assumed to belong to the [0.0,1.0] range and is initialized to 1.0
     *
     * @param[in]   _pImg      image to set the label.
     */
    FWCOMED_API static bool checkOpacity( ::fwData::Image::sptr _pImg);
};

} // fieldHelper
} // fwComEd


#endif // _FWCOMED_FIELDHELPER_MEDICALIMAGEHELPERS_HPP_

