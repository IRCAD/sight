/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmString.h>

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string/trim.hpp>


#include <boost/assign/list_of.hpp>

#include <boost/algorithm/string/find.hpp>  // For searching a substring

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include "gdcmIO/DicomDictionarySegment.hpp"

namespace gdcmIO

{

/**
 * @brief   Functor which compare two DicomCodedAttribute.
 */
struct DicomCodedAttributeOrderer
{
    /**
     * @brief   Operator to compare DicomCodedAttributes with their Code Values (CV).
     */
    bool operator()(const DicomCodedAttribute * d1, const DicomCodedAttribute * d2) const
    {
        return d1->CV < d2->CV;
    }
};


// BOOST 1.38 -> ::boost::bimaps::bimap<....>    note bimapS  ## BOOST 1.39 -> ::boost::bimap::bimap<....>
typedef ::boost::bimaps::bimap< ::boost::bimaps::unordered_set_of< std::string >,
                                ::boost::bimaps::multiset_of< const DicomCodedAttribute *, DicomCodedAttributeOrderer >
                              > attributesTranslator;




//**************************************************************//
//**                                                          **//
//**            Anatomical region coded attributes            **//
//**                                                          **//
//**************************************************************//
// Set of DicomCodedAttribute to write in DICOM file

//////////////////////////////////////////////////////////////////
//**                        Organs                            **//
//////////////////////////////////////////////////////////////////
static const DicomCodedAttribute * BRAIN    = new DicomCodedAttribute("T-A0100", "SRT", "Brain");
static const DicomCodedAttribute * HEART    = new DicomCodedAttribute("T-32000", "SRT", "Heart");
static const DicomCodedAttribute * COLON    = new DicomCodedAttribute("T-59300", "SRT", "Colon");
static const DicomCodedAttribute * DIAPHRAGM= new DicomCodedAttribute("T-D3400", "SRT", "Diaphragm");
static const DicomCodedAttribute * DUODENUM = new DicomCodedAttribute("T-58200", "SRT", "Duodenum");
static const DicomCodedAttribute * STOMACH  = new DicomCodedAttribute("T-57000", "SRT", "Stomach");
static const DicomCodedAttribute * LIVER    = new DicomCodedAttribute("T-62002", "SRT", "Liver");
static const DicomCodedAttribute * SMALL_INTESTINE  = new DicomCodedAttribute("T-58000", "SRT", "Small Intestine");
static const DicomCodedAttribute * ENTIRE_BODY      = new DicomCodedAttribute("T-D0010", "SRT", "Entire Body");
static const DicomCodedAttribute * ESOPHAGUS= new DicomCodedAttribute("T-56000", "SRT", "Esophagus");
static const DicomCodedAttribute * BONE     = new DicomCodedAttribute("T-D016E", "SRT", "Bone");
static const DicomCodedAttribute * PANCREAS = new DicomCodedAttribute("T-65000", "SRT", "Pancreas");
static const DicomCodedAttribute * PARATHYROID      = new DicomCodedAttribute("T-B7000", "SRT", "Parathyroid");
static const DicomCodedAttribute * LUNG     = new DicomCodedAttribute("T-28000", "SRT", "Lung");
static const DicomCodedAttribute * PROSTATE = new DicomCodedAttribute("T-9200B", "SRT", "Prostate");
static const DicomCodedAttribute * SPLEEN   = new DicomCodedAttribute("T-C3000", "SRT", "Spleen");
static const DicomCodedAttribute * KIDNEY   = new DicomCodedAttribute("T-71000", "SRT", "Kidney");
static const DicomCodedAttribute * ADRENAL_GLAND    = new DicomCodedAttribute("T-B3000", "SRT", "Adrenal Gland");
static const DicomCodedAttribute * TRACHEA  = new DicomCodedAttribute("T-25000", "SRT", "Trachea");
static const DicomCodedAttribute * THYROID  = new DicomCodedAttribute("T-B6000", "SRT", "Thyroid");
static const DicomCodedAttribute * GALL_BLADDER     = new DicomCodedAttribute("T-63000", "SRT", "Gall Bladder");
static const DicomCodedAttribute * BLADDER  = new DicomCodedAttribute("T-74000", "SRT", "Bladder");
static const DicomCodedAttribute * WRIST_JOINT      = new DicomCodedAttribute("T-15460", "SRT", "Wrist Joint");

//////////////////////////////////////////////////////////////////
//**                        Network                           **//
//////////////////////////////////////////////////////////////////

//**                        Arteries                          **//
static const DicomCodedAttribute * ABDOMINAL_AORTA   = new DicomCodedAttribute("T-42501", "SRT", "Abdominal Aorta");
static const DicomCodedAttribute * THORACIC_AORTA    = new DicomCodedAttribute("T-4600A", "SRT", "Thoracic Aorta");
static const DicomCodedAttribute * ARTERY            = new DicomCodedAttribute("T-41000", "SRT", "Artery");
static const DicomCodedAttribute * CAROTID_ARTERY    = new DicomCodedAttribute("T-45010", "SRT", "Carotid Artery");
static const DicomCodedAttribute * FEMORAL_ARTERY    = new DicomCodedAttribute("T-47400", "SRT", "Femoral Artery");
static const DicomCodedAttribute * HEPATIC_ARTERY    = new DicomCodedAttribute("T-46420", "SRT", "Hepatic Artery");
static const DicomCodedAttribute * MESENTERIC_ARTERY = new DicomCodedAttribute("T-46500", "SRT", "Mesenteric Artery");
static const DicomCodedAttribute * PULMONARY_ARTERY  = new DicomCodedAttribute("T-44000", "SRT", "Pulmonary Artery");
static const DicomCodedAttribute * RENAL_ARTERY      = new DicomCodedAttribute("T-46600", "SRT", "Renal Artery");
static const DicomCodedAttribute * SUBCLAVIAN_ARTERY = new DicomCodedAttribute("T-46100", "SRT", "Subclavian Artery");
static const DicomCodedAttribute * SPLENIC_ARTERY    = new DicomCodedAttribute("T-46460", "SRT", "Splenic Artery");

//**                        Veins                             **//
static const DicomCodedAttribute * VEIN                  = new DicomCodedAttribute("T-48000", "SRT", "Vein");
static const DicomCodedAttribute * SUPERIOR_VENA_CAVA    = new DicomCodedAttribute("T-48610", "SRT", "Superior Vena Cava");
static const DicomCodedAttribute * INFERIOR_VENA_CAVA    = new DicomCodedAttribute("T-48710", "SRT", "Inferior Vena Cava");
static const DicomCodedAttribute * HEPATIC_VEIN          = new DicomCodedAttribute("T-48720", "SRT", "Hepatic Vein");
static const DicomCodedAttribute * INTERNAL_JUGULAR_VEIN = new DicomCodedAttribute("T-48170", "SRT", "Internal Jugular Vein");
static const DicomCodedAttribute * MESENTERIC_VEIN       = new DicomCodedAttribute("T-4884A", "SRT", "Mesenteric Vein");
static const DicomCodedAttribute * PORTAL_VEIN           = new DicomCodedAttribute("T-48810", "SRT", "Portal Vein");
static const DicomCodedAttribute * PULMONARY_VEIN        = new DicomCodedAttribute("T-48581", "SRT", "Pulmonary Vein");
static const DicomCodedAttribute * RENAL_VEIN            = new DicomCodedAttribute("T-48740", "SRT", "Renal Vein");
static const DicomCodedAttribute * SUBCLAVIAN_VEIN       = new DicomCodedAttribute("T-48330", "SRT", "Subclavian Vein");
static const DicomCodedAttribute * SPLENIC_VEIN          = new DicomCodedAttribute("T-48890", "SRT", "Splenic Vein");

//**                        Miscellaneous                     **//
static const DicomCodedAttribute * BILE_DUCT = new DicomCodedAttribute("T-60610", "SRT", "Bile Duct");
static const DicomCodedAttribute * BRONCHUS  = new DicomCodedAttribute("T-26000", "SRT", "Bronchus");
static const DicomCodedAttribute * URETER    = new DicomCodedAttribute("T-73800", "SRT", "Ureter");

//////////////////////////////////////////////////////////////////
//**                        Functional                        **//
//////////////////////////////////////////////////////////////////
static const DicomCodedAttribute * SIGMOID_COLON    = new DicomCodedAttribute("T-59470", "SRT", "Sigmoid colon");
static const DicomCodedAttribute * RECTUM   = new DicomCodedAttribute("T-59600", "SRT", "Rectum");

//////////////////////////////////////////////////////////////////
//**                        Miscellaneous                     **//
//////////////////////////////////////////////////////////////////
static const DicomCodedAttribute * CYST = new DicomCodedAttribute("M-3340A", "SRT", "Cyst");
static const DicomCodedAttribute * TODEFINE = new DicomCodedAttribute("", "", "");

// List to translate FW4SPL organ names into DicomCodedAttribute
static const attributesTranslator anatReg = boost::assign::list_of< attributesTranslator::relation >
("Brain",                 BRAIN)
("Heart",                 HEART)
("Colon",                 COLON)
("Diaphragm",             DIAPHRAGM)
("Duodenum",              DUODENUM)
("Stomach",               STOMACH)
("Liver",                 LIVER)
("Small_Intestine",       SMALL_INTESTINE)
("Entire_Body",           ENTIRE_BODY)
("Esophagus",             ESOPHAGUS)
("Bone",                  BONE)
("Pancreas",              PANCREAS)
("Parathyroid",           PARATHYROID)
("Lung",                  LUNG)
("Prostate",              PROSTATE)
("Spleen",                SPLEEN)
("Kidney",                KIDNEY)
("Adrenal_Gland",         ADRENAL_GLAND)
("Trachea",               TRACHEA)
("Thyroid",               THYROID)
("Gall_Bladder",          GALL_BLADDER)
("Bladder",               BLADDER)
("Wrist_Joint",           WRIST_JOINT)
("Abdominal_Aorta",       ABDOMINAL_AORTA)
("Thoracic_Aorta",        THORACIC_AORTA)
("Artery",                ARTERY)
("Carotid_Artery",        CAROTID_ARTERY)
("Femoral_Artery",        FEMORAL_ARTERY)
("Hepatic_Artery",        HEPATIC_ARTERY)
("Mesenteric_Artery",     MESENTERIC_ARTERY)
("Pulmonary_Artery",      PULMONARY_ARTERY)
("Renal_Artery",          RENAL_ARTERY)
("Subclavian_Artery",     SUBCLAVIAN_ARTERY)
("Splenic_Artery",        SPLENIC_ARTERY)
("Vein",                  VEIN)
("Superior_Vena_Cava",    SUPERIOR_VENA_CAVA)
("Inferior_Vena_Cava",    INFERIOR_VENA_CAVA)
("Hepatic_Vein",          HEPATIC_VEIN)
("Internal_Jugular_Vein", INTERNAL_JUGULAR_VEIN)
("Mesenteric_Vein",       MESENTERIC_VEIN)
("Portal_Vein",           PORTAL_VEIN)
("Pulmonary_Vein",        PULMONARY_VEIN)
("Renal_Vein",            RENAL_VEIN)
("Subclavian_Vein",       SUBCLAVIAN_VEIN)
("Splenic_Vein",          SPLENIC_VEIN)
("Bile_Duct",             BILE_DUCT)
("Bronchus",              BRONCHUS)
("Ureter",                URETER)
("Sigmoid_Colon",         SIGMOID_COLON)
("Rectum",                RECTUM)
("Cyst",                  CYST)
("ToDefine",              TODEFINE)
;


//**************************************************************//
//**                                                          **//
//**        Segmented Property category coded attributes      **//
//**                                                          **//
//**************************************************************//

// Set of DicomCodedAttribute to write in DICOM file
//static const DicomCodedAttribute * TISSUE           = new DicomCodedAttribute("T-D0050", "SRT", "Tissue");
static const DicomCodedAttribute * ANATSTRUCT       = new DicomCodedAttribute("T-D000A", "SRT", "Anatomical Structure");
static const DicomCodedAttribute * PHYSIC_OBJECT    = new DicomCodedAttribute("A-00004", "SRT", "Physical object");
static const DicomCodedAttribute * ALTEREDSTRUCT    = new DicomCodedAttribute("M-01000", "SRT", "Morphologically Altered Structure");


static const attributesTranslator propCat = boost::assign::list_of< attributesTranslator::relation >
("Anat_Struct",    ANATSTRUCT)
("Physic_Object",  PHYSIC_OBJECT)
("Altered_Struct", ALTEREDSTRUCT)
("ToDefine",       TODEFINE)
;



//**************************************************************//
//**                                                          **//
//**        Segmented Property type coded attributes          **//
//**                                                          **//
//**************************************************************//
// Set of DicomCodedAttribute to write in DICOM file

//////////////////////////////////////////////////////////////////
//**                        Organs                            **//
//////////////////////////////////////////////////////////////////
static const DicomCodedAttribute * T_LIVER  = new DicomCodedAttribute("T-62000", "SRT", "Liver");
static const DicomCodedAttribute * T_PROSTATE   = new DicomCodedAttribute("T-92000", "SRT", "Prostate");

//////////////////////////////////////////////////////////////////
//**                        Network                           **//
//////////////////////////////////////////////////////////////////

//**                        Arteries                          **//
static const DicomCodedAttribute * T_ABDOMINAL_AORTA= new DicomCodedAttribute("T-42500", "SRT", "Abdominal Aorta");
static const DicomCodedAttribute * T_THORACIC_AORTA = new DicomCodedAttribute("T-42070", "SRT", "Thoracic Aorta");

//**                        Veins                             **//
static const DicomCodedAttribute * T_PULMONARY_VEIN = new DicomCodedAttribute("T-48500", "SRT", "Pulmonary Vein");

//**                        Miscellaneous                     **//

//////////////////////////////////////////////////////////////////
//**                        Lesions                           **//
//////////////////////////////////////////////////////////////////
static const DicomCodedAttribute * T_CYST   = new DicomCodedAttribute("M-3340A", "SRT", "Cyst");

//////////////////////////////////////////////////////////////////
//**                        Objects                           **//
//////////////////////////////////////////////////////////////////
static const DicomCodedAttribute * T_NEEDLE     = new DicomCodedAttribute("A-30360", "SRT", "Needle");
static const DicomCodedAttribute * T_CATHETER   = new DicomCodedAttribute("A-26800", "SRT", "Catheter");
static const DicomCodedAttribute * T_MARKER     = new DicomCodedAttribute("A-00D7B", "SRT", "Opaque Marker");
static const DicomCodedAttribute * T_PROSTHESIS = new DicomCodedAttribute("A-04000", "SRT", "Prosthesis");
static const DicomCodedAttribute * T_CLOTHING   = new DicomCodedAttribute("A-00916", "SRT", "Clothing");

//////////////////////////////////////////////////////////////////
//**                        Functional                        **//
//////////////////////////////////////////////////////////////////
static const DicomCodedAttribute * T_TRANSVERSE = new DicomCodedAttribute("T-59440", "SRT", "Transverse Colon");

//////////////////////////////////////////////////////////////////
//**                        Miscellaneous                     **//
//////////////////////////////////////////////////////////////////

static const attributesTranslator propType = boost::assign::list_of< attributesTranslator::relation >
("T_Liver",           T_LIVER )
("T_Prostate",        T_PROSTATE )
("T_Abdominal_Aorta", T_ABDOMINAL_AORTA )
("T_Thoracic_Aorta",  T_THORACIC_AORTA )
("T_Pulmonary_Vein",  T_PULMONARY_VEIN )
("T_Cyst",            T_CYST )
("T_Needle",          T_NEEDLE )
("T_Catheter",        T_CATHETER )
("T_Marker",          T_MARKER )
("T_Prosthesis",      T_PROSTHESIS)
("T_Clothing",        T_CLOTHING )
("T_Transverse",      T_TRANSVERSE )
("ToDefine",          TODEFINE )
;

//------------------------------------------------------------------------------

const DicomCodedAttribute * DicomDictionarySegment::guessAnatRegionFromLabel(const std::string& a_label)
{
    // Delete possible space as last character
    std::string cleanedLabel = ::boost::algorithm::trim_copy(a_label);

    if(!cleanedLabel.empty())
    {
        // Search and translate label into anatomic region sequence
        if ( anatReg.left.find( cleanedLabel ) != anatReg.left.end() )
        {
            return anatReg.left.at(cleanedLabel);
        }
    }
    return TODEFINE;
}

//------------------------------------------------------------------------------

const DicomCodedAttribute * DicomDictionarySegment::guessPropCategoryFromLabel(const std::string& a_label)
{
    // Delete possible space as last character
    std::string cleanedLabel = ::boost::algorithm::trim_copy(a_label);

    if(!cleanedLabel.empty())
    {
        // Search and translate label into segmented property category sequence
        if ( propCat.left.find( cleanedLabel ) != propCat.left.end() )
        {
            return propCat.left.at(cleanedLabel);
        }
    }
    return TODEFINE;
}

//------------------------------------------------------------------------------

const DicomCodedAttribute * DicomDictionarySegment::guessPropTypeFromLabel(const std::string& a_label)
{
    // Delete possible space as last character
    std::string cleanedLabel = ::boost::algorithm::trim_copy(a_label);

    if(!cleanedLabel.empty())
    {
        // Search and translate label into segmented property type sequence
        if ( propType.left.find( cleanedLabel ) != propType.left.end() )
        {
            return propType.left.at(cleanedLabel);
        }
        else if ( propCat.left.find( cleanedLabel ) != propCat.left.end() )
        {
            return propCat.left.at(cleanedLabel);
        }
    }
    return TODEFINE;
}

}
