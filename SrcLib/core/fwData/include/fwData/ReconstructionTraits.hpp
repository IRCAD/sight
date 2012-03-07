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
 * @brief   This class defines Reconstruction traits.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-20011.
 *
 * A ReconstructionTraits contains :
 *  - an identifier
 *  - a structure traits associated to the reconstruction
 *  - a mask operator node
 *  - a mesh operator node
 *
 * @see StructureTraits
 */
class FWDATA_CLASS_API ReconstructionTraits : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ReconstructionTraits)(::fwData::Object), (()), ::fwData::Factory::New< ReconstructionTraits >) ;

    fwGettersSettersDocMacro(Identifier, identifier, std::string, the ROIs identifier);

    /// Set the reconstruction operator mask node
    FWDATA_API void setMaskOpNode( ::fwData::Node::sptr maskOpNode );

    /// Get the reconstruction operator mask node. Return a null pointer if the mask node is not defined.
    FWDATA_API ::fwData::Node::sptr getMaskOpNode();

    /// Set the reconstruction operator mesh node.
    FWDATA_API void setMeshOpNode( ::fwData::Node::sptr meshOpNode );

    /// Get the reconstruction operator mesh node. Return a null pointer if the mesh node is not defined.
    FWDATA_API ::fwData::Node::sptr getMeshOpNode();

    /// Set the associated structure traits
    FWDATA_API void setStructureTraits( ::fwData::StructureTraits::sptr structureTraits );

    /// Get the associated structure traits
    FWDATA_API ::fwData::StructureTraits::sptr getStructureTraits();

protected :

    /**
     * @brief constructor
     */
    FWDATA_API ReconstructionTraits();

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~ReconstructionTraits();

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
