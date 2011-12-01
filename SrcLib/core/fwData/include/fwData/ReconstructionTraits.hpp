/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_RECONSTRUCTIONTRAITS_HPP__
#define __FWDATA_RECONSTRUCTIONTRAITS_HPP__

#include "fwData/Node.hpp"
#include "fwData/StructureTraits.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class   ReconstructionTraits
 * @brief   This class defines Reconstruction traits
 * @author  IRCAD (Research and Development Team).
 * @date    2007-20011.
 */
class FWDATA_CLASS_API ReconstructionTraits : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ReconstructionTraits)(::fwData::Object), (()), ::fwTools::Factory::New< ReconstructionTraits >) ;

    fwGettersSettersDocMacro(Identifier, identifier, std::string, the ROIs identifier);

    fwGettersSettersDocMacro(MaskNode, maskOpNode, ::fwData::Node::wptr, the reconstruction operator mask node);

    fwGettersSettersDocMacro(MeshNode, meshOpNode, ::fwData::Node::wptr, the reconstruction operator mesh node);

    fwGettersSettersDocMacro(StructureTraits, structureTraits, StructureTraits::wptr, the associated structure traits);

protected :

    /**
     * @brief constructor
     */
    FWDATA_API ReconstructionTraits();

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~ReconstructionTraits() throw();

private:

    /// ROI identifier
    std::string m_identifier;

    /// reconstruction mask operator node
    ::fwData::Node::wptr m_maskOpNode;

    /// reconstruction mesh operator node
    ::fwData::Node::wptr m_meshOpNode;

    /// associated structure traits
    ::fwData::StructureTraits::wptr m_structureTraits;

};

} // namespace fwData

#endif // __FWDATA_RECONSTRUCTIONTRAITS_HPP__
