
#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/DictionaryOrgan.hpp"

fwCampImplementDataMacro((fwData)(DictionaryOrgan))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("organ_name",&::fwData::DictionaryOrgan::m_organName)
        .property("organDataNode",&::fwData::DictionaryOrgan::m_roi4OrganDataNode)
        .property("maskDataNode",&::fwData::DictionaryOrgan::m_maskDataNode)
        .property("meshDataNode",&::fwData::DictionaryOrgan::m_meshDataNode)
        .property("organ_native_exp",&::fwData::DictionaryOrgan::m_roi4OrganNativeExp)
        .property("organ_exp",&::fwData::DictionaryOrgan::m_roi4OrganExp)
        .property("structure_type",&::fwData::DictionaryOrgan::m_sStructureType)
        .property("label",&::fwData::DictionaryOrgan::m_sSALabel)
        .property("is_medical_structure",&::fwData::DictionaryOrgan::m_bIsMedicalStructure)
        .property("color",&::fwData::DictionaryOrgan:: m_ui32AOColor)
        .property("avg_volume",&::fwData::DictionaryOrgan::m_dAvgVolume)
        .property("vol_deviation",&::fwData::DictionaryOrgan::m_dVolStdDeviation)
        ;
}
