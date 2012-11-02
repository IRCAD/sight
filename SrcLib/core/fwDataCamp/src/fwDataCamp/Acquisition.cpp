/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>

#include "fwDataCamp/Acquisition.hpp"

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwData)(Acquisition))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("bits_per_pixel", &::fwData::Acquisition::m_ui8BitsPerPixel)
        .property("slice_thickness", &::fwData::Acquisition::m_fSliceThickness)
        .property("axe", &::fwData::Acquisition::m_ui8Axe)
        .property("unsigned_flag", &::fwData::Acquisition::m_bUnsignedFlag)
        .property("acquisition_index", &::fwData::Acquisition::m_ui32AcquisitionIndex)
        .property("image_type", &::fwData::Acquisition::m_sImageType)
        .property("image_format", &::fwData::Acquisition::m_sImageFormat)
        .property("creation_date", &::fwData::Acquisition::m_ptCreationDate)
        .property("is_main", &::fwData::Acquisition::m_bIsMain)
        .property("is_normal_dir", &::fwData::Acquisition::m_bIsNormalDir)
        .property("uid", &::fwData::Acquisition::m_sUID)
        .property("db_id", &::fwData::Acquisition::m_i32DbID)
        .property("labo_id", &::fwData::Acquisition::m_ui32LaboID)
        .property("net_id", &::fwData::Acquisition::m_ui32NetID)
        .property("date_send_to_labo", &::fwData::Acquisition::m_ptDateSendToLaboAt)
        .property("date_receive_from_labo", &::fwData::Acquisition::m_ptDateReceiveFromLaboAt)
        .property("date_send_to_dbt", &::fwData::Acquisition::m_ptDateSendToBDDAt)
        .property("date_disponibility", &::fwData::Acquisition::m_ptDateDisponibilityAt)
        .property("patient_size", &::fwData::Acquisition::m_dPatientSize)
        .property("patient_weight", &::fwData::Acquisition::m_dPatientWeight)
        .property("radiations", &::fwData::Acquisition::m_dRadiations)
        .property("medical_printer", &::fwData::Acquisition::m_sMedicalPrinter)
        .property("medical_printer_corp", &::fwData::Acquisition::m_sMedicalPrinterCorp)
        .property("patient_position", &::fwData::Acquisition::m_sPatientPosition)
        .property("description", &::fwData::Acquisition::m_description)
        .property("dicom_file_list", &::fwData::Acquisition::m_dicomFileList)
        .property("path_to_files", &::fwData::Acquisition::m_pathToFiles)
        .property("image", &::fwData::Acquisition::m_attrImage)
        .property("struct_anat", &::fwData::Acquisition::m_attrStructAnat)
        .property("reconstructions", &::fwData::Acquisition::m_attrReconstructions)
        ;

}
