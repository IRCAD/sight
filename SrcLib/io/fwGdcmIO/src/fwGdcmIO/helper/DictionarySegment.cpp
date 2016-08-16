/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DictionarySegment.hpp"

#include <fwCore/spyLog.hpp>

#include <gdcmString.h>

#include <boost/algorithm/string/find.hpp> // For searching a substring
#include <boost/algorithm/string/trim.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/functional/hash.hpp>

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

/**
 * @brief Functor which compare two DicomCodedAttribute.
 */
struct DicomCodedAttributeOrderer
{
    /**
     * @brief Operator to compare DicomCodedAttributes with their Code Values (CV).
     */
    bool operator()(const ::fwGdcmIO::container::DicomCodedAttribute* a,
                    const ::fwGdcmIO::container::DicomCodedAttribute* b) const
    {
        return a->getCodeValue() < b->getCodeValue();
    }
};

typedef ::boost::bimaps::bimap< ::boost::bimaps::unordered_set_of< std::string >,
                                ::boost::bimaps::multiset_of< const ::fwGdcmIO::container::DicomCodedAttribute*,
                                                              DicomCodedAttributeOrderer > > attributesTranslator;

//------------------------------------------------------------------------------

//**************************************************************//
//** Anatomical region coded attributes **//
//**************************************************************//
// Set of DicomCodedAttribute to write in DICOM file

//////////////////////////////////////////////////////////////////
//** Organs **//
//////////////////////////////////////////////////////////////////
static const ::fwGdcmIO::container::DicomCodedAttribute* s_BRAIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-A0100", "SRT", "Brain");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_HEART =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-32000", "SRT", "Heart");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_COLON =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-59300", "SRT", "Colon");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_DIAPHRAGM =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-D3400", "SRT", "Diaphragm");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_DUODENUM =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-58200", "SRT", "Duodenum");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_STOMACH =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-57000", "SRT", "Stomach");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_LIVER =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-62002", "SRT", "Liver");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SMALL_INTESTINE =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-58000", "SRT", "Small Intestine");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_ENTIRE_BODY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-D0010", "SRT", "Entire Body");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_ESOPHAGUS =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-56000", "SRT", "Esophagus");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_BONE =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-D016E", "SRT", "Bone");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_PANCREAS =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-65000", "SRT", "Pancreas");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_PARATHYROID =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-B7000", "SRT", "Parathyroid");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_LUNG =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-28000", "SRT", "Lung");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_PROSTATE =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-9200B", "SRT", "Prostate");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SPLEEN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-C3000", "SRT", "Spleen");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_KIDNEY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-71000", "SRT", "Kidney");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_ADRENAL_GLAND =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-B3000", "SRT", "Adrenal Gland");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_TRACHEA =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-25000", "SRT", "Trachea");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_THYROID =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-B6000", "SRT", "Thyroid");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_GALL_BLADDER =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-63000", "SRT", "Gall Bladder");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_BLADDER =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-74000", "SRT", "Bladder");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_WRIST_JOINT =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-15460", "SRT", "Wrist Joint");

//////////////////////////////////////////////////////////////////
//** Network **//
//////////////////////////////////////////////////////////////////

//** Arteries **//
static const ::fwGdcmIO::container::DicomCodedAttribute* s_ABDOMINAL_AORTA =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-42501", "SRT", "Abdominal Aorta");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_THORACIC_AORTA =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-4600A", "SRT", "Thoracic Aorta");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-41000", "SRT", "Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_CAROTID_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-45010", "SRT", "Carotid Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_FEMORAL_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-47400", "SRT", "Femoral Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_HEPATIC_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-46420", "SRT", "Hepatic Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_MESENTERIC_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-46500", "SRT", "Mesenteric Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_PULMONARY_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-44000", "SRT", "Pulmonary Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_RENAL_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-46600", "SRT", "Renal Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SUBCLAVIAN_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-46100", "SRT", "Subclavian Artery");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SPLENIC_ARTERY =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-46460", "SRT", "Splenic Artery");

//** Veins **//
static const ::fwGdcmIO::container::DicomCodedAttribute* s_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48000", "SRT", "Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SUPERIOR_VENA_CAVA =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48610", "SRT", "Superior Vena Cava");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_INFERIOR_VENA_CAVA =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48710", "SRT", "Inferior Vena Cava");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_HEPATIC_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48720", "SRT", "Hepatic Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_INTERNAL_JUGULAR_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48170", "SRT", "Internal Jugular Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_MESENTERIC_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-4884A", "SRT", "Mesenteric Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_PORTAL_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48810", "SRT", "Portal Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_PULMONARY_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48581", "SRT", "Pulmonary Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_RENAL_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48740", "SRT", "Renal Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SUBCLAVIAN_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48330", "SRT", "Subclavian Vein");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SPLENIC_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48890", "SRT", "Splenic Vein");

//** Miscellaneous **//
static const ::fwGdcmIO::container::DicomCodedAttribute* s_BILE_DUCT = new ::fwGdcmIO::container::DicomCodedAttribute(
    "T-60610", "SRT", "Bile Duct");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_BRONCHUS = new ::fwGdcmIO::container::DicomCodedAttribute(
    "T-26000", "SRT", "Bronchus");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_URETER = new ::fwGdcmIO::container::DicomCodedAttribute(
    "T-73800", "SRT", "Ureter");

//////////////////////////////////////////////////////////////////
//** Functional **//
//////////////////////////////////////////////////////////////////
static const ::fwGdcmIO::container::DicomCodedAttribute* s_SIGMOID_COLON =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-59470", "SRT", "Sigmoid colon");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_RECTUM =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-59600", "SRT", "Rectum");

//////////////////////////////////////////////////////////////////
//** Miscellaneous **//
//////////////////////////////////////////////////////////////////
static const ::fwGdcmIO::container::DicomCodedAttribute* s_CYST = new ::fwGdcmIO::container::DicomCodedAttribute(
    "M-3340A", "SRT", "Cyst");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_TODEFINE = new ::fwGdcmIO::container::DicomCodedAttribute(
    "", "", "");

// List to translate FW4SPL organ names into DicomCodedAttribute
static const attributesTranslator s_ANATOMICAL_REGIONS = boost::assign::list_of< attributesTranslator::relation >
                                                             ("Brain", s_BRAIN)
                                                             ("Heart", s_HEART)
                                                             ("Colon", s_COLON)
                                                             ("Diaphragm", s_DIAPHRAGM)
                                                             ("Duodenum", s_DUODENUM)
                                                             ("Stomach", s_STOMACH)
                                                             ("Liver", s_LIVER)
                                                             ("Small_Intestine", s_SMALL_INTESTINE)
                                                             ("Entire_Body", s_ENTIRE_BODY)
                                                             ("Esophagus", s_ESOPHAGUS)
                                                             ("Bone", s_BONE)
                                                             ("Pancreas", s_PANCREAS)
                                                             ("Parathyroid", s_PARATHYROID)
                                                             ("Lung", s_LUNG)
                                                             ("Prostate", s_PROSTATE)
                                                             ("Spleen", s_SPLEEN)
                                                             ("Kidney", s_KIDNEY)
                                                             ("Adrenal_Gland", s_ADRENAL_GLAND)
                                                             ("Trachea", s_TRACHEA)
                                                             ("Thyroid", s_THYROID)
                                                             ("Gall_Bladder", s_GALL_BLADDER)
                                                             ("Bladder", s_BLADDER)
                                                             ("Wrist_Joint", s_WRIST_JOINT)
                                                             ("Abdominal_Aorta", s_ABDOMINAL_AORTA)
                                                             ("Thoracic_Aorta", s_THORACIC_AORTA)
                                                             ("Artery", s_ARTERY)
                                                             ("Carotid_Artery", s_CAROTID_ARTERY)
                                                             ("Femoral_Artery", s_FEMORAL_ARTERY)
                                                             ("Hepatic_Artery", s_HEPATIC_ARTERY)
                                                             ("Mesenteric_Artery", s_MESENTERIC_ARTERY)
                                                             ("Pulmonary_Artery", s_PULMONARY_ARTERY)
                                                             ("Renal_Artery", s_RENAL_ARTERY)
                                                             ("Subclavian_Artery", s_SUBCLAVIAN_ARTERY)
                                                             ("Splenic_Artery", s_SPLENIC_ARTERY)
                                                             ("Vein", s_VEIN)
                                                             ("Superior_Vena_Cava", s_SUPERIOR_VENA_CAVA)
                                                             ("Inferior_Vena_Cava", s_INFERIOR_VENA_CAVA)
                                                             ("Hepatic_Vein", s_HEPATIC_VEIN)
                                                             ("Internal_Jugular_Vein", s_INTERNAL_JUGULAR_VEIN)
                                                             ("Mesenteric_Vein", s_MESENTERIC_VEIN)
                                                             ("Portal_Vein", s_PORTAL_VEIN)
                                                             ("Pulmonary_Vein", s_PULMONARY_VEIN)
                                                             ("Renal_Vein", s_RENAL_VEIN)
                                                             ("Subclavian_Vein", s_SUBCLAVIAN_VEIN)
                                                             ("Splenic_Vein", s_SPLENIC_VEIN)
                                                             ("Bile_Duct", s_BILE_DUCT)
                                                             ("Bronchus", s_BRONCHUS)
                                                             ("Ureter", s_URETER)
                                                             ("Sigmoid_Colon", s_SIGMOID_COLON)
                                                             ("Rectum", s_RECTUM)
                                                             ("Cyst", s_CYST)
                                                             ("ToDefine", s_TODEFINE);

//**************************************************************//
//** Segmented Property category coded attributes **//
//**************************************************************//

// Set of DicomCodedAttribute to write in DICOM file
//static const DicomCodedAttribute * s_TISSUE = new DicomCodedAttribute("T-D0050", "SRT", "Tissue");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_ANATSTRUCT =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-D000A", "SRT", "Anatomical Structure");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_PHYSIC_OBJECT =
    new ::fwGdcmIO::container::DicomCodedAttribute("A-00004", "SRT", "Physical object");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_ALTEREDSTRUCT =
    new ::fwGdcmIO::container::DicomCodedAttribute("M-01000", "SRT", "Morphologically Altered Structure");

static const attributesTranslator s_PROPERTY_CATEGORIES = boost::assign::list_of< attributesTranslator::relation >
                                                              ("Anat_Struct", s_ANATSTRUCT)
                                                              ("Physic_Object", s_PHYSIC_OBJECT)
                                                              ("Altered_Struct", s_ALTEREDSTRUCT)
                                                              ("ToDefine", s_TODEFINE);

//**************************************************************//
//** Segmented Property type coded attributes **//
//**************************************************************//
// Set of DicomCodedAttribute to write in DICOM file

//////////////////////////////////////////////////////////////////
//** Organs **//
//////////////////////////////////////////////////////////////////
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_LIVER = new ::fwGdcmIO::container::DicomCodedAttribute(
    "T-62000", "SRT", "Liver");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_PROSTATE = new ::fwGdcmIO::container::DicomCodedAttribute(
    "T-92000", "SRT", "Prostate");

//////////////////////////////////////////////////////////////////
//** Network **//
//////////////////////////////////////////////////////////////////

//** Arteries **//
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_ABDOMINAL_AORTA =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-42500", "SRT", "Abdominal Aorta");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_THORACIC_AORTA =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-42070", "SRT", "Thoracic Aorta");

//** Veins **//
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_PULMONARY_VEIN =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-48500", "SRT", "Pulmonary Vein");

//** Miscellaneous **//
//////////////////////////////////////////////////////////////////
//** Lesions **//
//////////////////////////////////////////////////////////////////
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_CYST = new ::fwGdcmIO::container::DicomCodedAttribute(
    "M-3340A", "SRT", "Cyst");

//////////////////////////////////////////////////////////////////
//** Objects **//
//////////////////////////////////////////////////////////////////
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_NEEDLE =
    new ::fwGdcmIO::container::DicomCodedAttribute("A-30360", "SRT", "Needle");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_CATHETER =
    new ::fwGdcmIO::container::DicomCodedAttribute("A-26800", "SRT", "Catheter");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_MARKER =
    new ::fwGdcmIO::container::DicomCodedAttribute("A-00D7B", "SRT", "Opaque Marker");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_PROSTHESIS =
    new ::fwGdcmIO::container::DicomCodedAttribute("A-04000", "SRT", "Prosthesis");
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_CLOTHING =
    new ::fwGdcmIO::container::DicomCodedAttribute("A-00916", "SRT", "Clothing");

//////////////////////////////////////////////////////////////////
//** Functional **//
//////////////////////////////////////////////////////////////////
static const ::fwGdcmIO::container::DicomCodedAttribute* s_T_TRANSVERSE =
    new ::fwGdcmIO::container::DicomCodedAttribute("T-59440", "SRT", "Transverse Colon");

//////////////////////////////////////////////////////////////////
//** Miscellaneous **//
//////////////////////////////////////////////////////////////////

static const attributesTranslator s_PROPERTY_TYPES =
    boost::assign::list_of< attributesTranslator::relation >
        ("T_Liver", s_T_LIVER)
        ("T_Prostate", s_T_PROSTATE)
        ("T_Abdominal_Aorta", s_T_ABDOMINAL_AORTA)
        ("T_Thoracic_Aorta", s_T_THORACIC_AORTA)
        ("T_Pulmonary_Vein", s_T_PULMONARY_VEIN)
        ("T_Cyst", s_T_CYST)
        ("T_Needle", s_T_NEEDLE)
        ("T_Catheter", s_T_CATHETER)
        ("T_Marker", s_T_MARKER)
        ("T_Prosthesis", s_T_PROSTHESIS)
        ("T_Clothing", s_T_CLOTHING)
        ("T_Transverse", s_T_TRANSVERSE)
        ("ToDefine", s_TODEFINE);

//------------------------------------------------------------------------------

const ::fwGdcmIO::container::DicomCodedAttribute* DictionarySegment::guessAnatRegionFromLabel(const std::string& label)
{
    // Delete possible space as last character
    std::string cleanedLabel = ::boost::algorithm::trim_copy(label);

    if (!cleanedLabel.empty())
    {
        // Search and translate label into anatomic region sequence
        if (s_ANATOMICAL_REGIONS.left.find(cleanedLabel) != s_ANATOMICAL_REGIONS.left.end())
        {
            return s_ANATOMICAL_REGIONS.left.at(cleanedLabel);
        }
    }
    return s_TODEFINE;
}

//------------------------------------------------------------------------------

const ::fwGdcmIO::container::DicomCodedAttribute* DictionarySegment::guessPropCategoryFromLabel(
    const std::string& label)
{
    // Delete possible space as last character
    std::string cleanedLabel = ::boost::algorithm::trim_copy(label);

    if (!cleanedLabel.empty())
    {
        // Search and translate label into segmented property category sequence
        if (s_PROPERTY_CATEGORIES.left.find(cleanedLabel) != s_PROPERTY_CATEGORIES.left.end())
        {
            return s_PROPERTY_CATEGORIES.left.at(cleanedLabel);
        }
    }
    return s_TODEFINE;
}

//------------------------------------------------------------------------------

const ::fwGdcmIO::container::DicomCodedAttribute* DictionarySegment::guessPropTypeFromLabel(const std::string& label)
{
    // Delete possible space as last character
    std::string cleanedLabel = ::boost::algorithm::trim_copy(label);

    if (!cleanedLabel.empty())
    {
        // Search and translate label into segmented property type sequence
        if (s_PROPERTY_TYPES.left.find(cleanedLabel) != s_PROPERTY_TYPES.left.end())
        {
            return s_PROPERTY_TYPES.left.at(cleanedLabel);
        }
        else if (s_PROPERTY_CATEGORIES.left.find(cleanedLabel) != s_PROPERTY_CATEGORIES.left.end())
        {
            return s_PROPERTY_CATEGORIES.left.at(cleanedLabel);
        }
    }
    return s_TODEFINE;
}

} //namespace helper
} //namespace fwGdcmIO
