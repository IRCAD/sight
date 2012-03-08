/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_DICTIONARY_ORGAN_HPP_
#define _FWDATA_DICTIONARY_ORGAN_HPP_

#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

#include "fwData/Node.hpp"
#include "fwData/String.hpp"

namespace fwData
{
/**
 * @class   DictionaryOrgan
 * @brief   This class defines informations about an organ (structure type, volume, ...)
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API DictionaryOrgan : public Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (DictionaryOrgan)(::fwData::Object), (()), ::fwData::Factory::New< DictionaryOrgan >) ;

    // Generator result---------------------------------------------------------

    fwGettersSettersDocMacro(StructureType, sStructureType, std::string, the organ structure type);

    fwGettersSettersDocMacro(SALabel, sSALabel, std::string, the anatomic structure label (skin, internal organ, ...));

    fwGettersSettersDocMacro(IsMedicalStructure, bIsMedicalStructure, bool, the flag indicating if the organ is valid);

    fwGettersSettersDocMacro(AOColor, ui32AOColor, ::boost::uint32_t, the color in the AllOrgans structure);

    fwGettersSettersDocMacro(AvgVolume, dAvgVolume, double, the average volume of the organ);

    fwGettersSettersDocMacro(VolStdDeviation, dVolStdDeviation, double, the standard deviation of the volume);

    fwGettersSettersDocMacro(NbExams, ui32NbExams, ::boost::uint32_t, the number of exams used to calculate the average volume);

    fwGettersSettersDocMacro(Position, sPosition, std::string, the position of the organ in the organism);

    fwGettersSettersDocMacro(IconPath, fsIconPath, ::boost::filesystem::path, the path of the icon corresponding to the organ);

    fwGettersSettersDocMacro(Red, dRed, double, red value defining the default organ color);

    fwGettersSettersDocMacro(Green, dGreen, double, green value defining the default organ color);

    fwGettersSettersDocMacro(Blue, dBlue, double, blue value defining the default organ color);

    fwGettersSettersDocMacro(Alpha, dAlpha, double, alpha value defining the default organ color);

    fwGettersSettersDocMacro(TexturePath, fsTexturePath, ::boost::filesystem::path, the path of the texture file corresponding to the organ);

    fwGettersSettersDocMacro(AvgTriangleNb, ui32AvgTriangleNb, ::boost::uint32_t, the average number of faces used to represent the organ);

    fwGettersSettersDocMacro(SegMin, dSegMin, double,the min intensity between which the organ should appear in the scan);

    fwGettersSettersDocMacro(SegMax, dSegMax, double, the max intensity between which the organ should appear in the scan);

    fwGettersSettersDocMacro(Modality, sModality, std::string, the modality);

    fwGettersSettersDocMacro(SurfaceElasticity, dSurfaceElasticity, double, the surface elasticity);

    fwGettersSettersDocMacro(SurfaceRupture, dSurfaceRupture, double, the surface rupture value);

    fwGettersSettersDocMacro(InteriorRupture, dInteriorRupture, double, the interior rupture value);

    fwGettersSettersDocMacro(Comment, sComment, std::string, the comment);

    fwGettersSettersDocMacro(BelongsTo, vBelongsTo, std::vector< std::string >, the string indicating to which organ this one belongs to);


    fwGettersSettersDocMacro(OrganName, organName, ::fwData::String::sptr, the name of the organ);

    fwGettersSettersDocMacro(Roi4OrganNativeExp, roi4OrganNativeExp, ::fwData::String::sptr, the string indicating the native expression that describes the roi used to segment organ);

    fwGettersSettersDocMacro(Roi4OrganExp, roi4OrganExp, ::fwData::String::sptr, the string indicating the expression that describes the roi used to segment organ);

    fwGettersSettersDocMacro(Roi4OrganDataNode, roi4OrganDataNode, ::fwData::Node::wptr, the node that represents the roi for organ used to segment organ);

    fwGettersSettersDocMacro(MaskDataNode, maskDataNode, ::fwData::Node::wptr, the node that represents the organ mask);

    fwGettersSettersDocMacro(MeshDataNode, meshDataNode, ::fwData::Node::wptr, the node that represents the organ mesh);

protected :

    /// Constructor
    FWDATA_API DictionaryOrgan();

    /// Destructor
    FWDATA_API virtual ~DictionaryOrgan();

    /// the name of the organ
    ::fwData::String::sptr m_organName;

    /// the node that represents the roi for organ used to segment organ;
    ::fwData::Node::wptr m_roi4OrganDataNode;

    /// the node that represents the organ mask
    ::fwData::Node::wptr m_maskDataNode;

    /// the node that represents the organ mesh
    ::fwData::Node::wptr m_meshDataNode;

    /// the string indicating the native expression that describes the roi used to segment organ
    ::fwData::String::sptr m_roi4OrganNativeExp;

    /// the string indicating the expression that describes the roi used to segment organ
    ::fwData::String::sptr m_roi4OrganExp;

    //! the organ structure type
    std::string m_sStructureType;

    //! the anatomic structure label (skin, internal organ, ...)
    std::string m_sSALabel;

    //! flag indicating if the organ is valid
    bool m_bIsMedicalStructure;

    //! the color in the AllOrgans structure
    ::boost::uint32_t  m_ui32AOColor;

    //! the average volume of the organ
    double m_dAvgVolume;

    //! the standard deviation of the volume
    double m_dVolStdDeviation;

    //! the number of exams used to calculate the average volume
    ::boost::uint32_t  m_ui32NbExams;

    //! the position of the organ in the organism
    std::string m_sPosition;

    //! the path of the icon corresponding to the organ
    ::boost::filesystem::path m_fsIconPath;

    //!@{
    //! @brief the 4 values defining the default organ color
    double m_dRed;
    double m_dGreen;
    double m_dBlue;
    double m_dAlpha;
    //!@}

    //! the path of the texture file corresponding to the organ
    ::boost::filesystem::path m_fsTexturePath;

    //! the average number of faces used to represent the organ
    ::boost::uint32_t  m_ui32AvgTriangleNb;

    //! the min intensity, between which the organ should appear in the scan
    double m_dSegMin;
    //! the max intensity, between which the organ should appear in the scan
    double m_dSegMax;

    //! the modality
    std::string m_sModality;

    //! the surface elasticity
    double m_dSurfaceElasticity;

    //! the surface rupture value
    double m_dSurfaceRupture;

    //! the interior rupture value
    double m_dInteriorRupture;

    //! a comment
    std::string m_sComment;

    //!the string indicating to which organ this one belongs to
    std::vector< std::string > m_vBelongsTo;
};

} // namespace fwData

#endif // _FWDATA_DICTIONARY_ORGAN_HPP_
