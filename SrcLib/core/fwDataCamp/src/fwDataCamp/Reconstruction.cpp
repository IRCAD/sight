#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Reconstruction.hpp"

fwCampImplementDataMacro((fwData)(Reconstruction))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("is_visible", &::fwData::Reconstruction::m_bIsVisible)
        .property("reconstruction_format", &::fwData::Reconstruction::m_sReconstructionFormat)
        .property("organ_name", &::fwData::Reconstruction::m_sOrganName)
        .property("structure_type", &::fwData::Reconstruction::m_sStructureType)
        .property("is_closed", &::fwData::Reconstruction::m_bIsClosed)
        .property("is_automatic", &::fwData::Reconstruction::m_bIsAutomatic)
        .property("avg_volume", &::fwData::Reconstruction::m_dAvgVolume)
        .property("vol_deviation", &::fwData::Reconstruction::m_dVolStdDeviation)
        .property("vol_pct_confidence", &::fwData::Reconstruction::m_dVolPctConfidence)
        .property("reconstruction_time", &::fwData::Reconstruction::m_sReconstructionTime)
        .property("mask_generated", &::fwData::Reconstruction::m_bMaskGenerated)
        .property("level", &::fwData::Reconstruction::m_ui8Level)
        .property("label", &::fwData::Reconstruction::m_ui16Label)
        .property("generated_3D", &::fwData::Reconstruction::m_bGenerated3D)
        .property("type_3D", &::fwData::Reconstruction::m_sType3D)
        .property("db_id", &::fwData::Reconstruction::m_i32DbID)
        .property("material", &::fwData::Reconstruction::m_attrMaterial)
        .property("image", &::fwData::Reconstruction::m_attrImage)
        .property("mesh", &::fwData::Reconstruction::m_attrMesh)
        ;
}
