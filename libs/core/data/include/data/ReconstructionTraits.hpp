/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "data/factory/new.hpp"
#include "data/Node.hpp"
#include "data/StructureTraits.hpp"

fwCampAutoDeclareDataMacro((sight)(data)(ReconstructionTraits));

namespace sight::data
{

/**
 * @brief This class defines Reconstruction traits.
 *
 * A ReconstructionTraits contains :
 *  - an identifier
 *  - a structure traits associated to the reconstruction
 *  - a mask operator node
 *  - a mesh operator node
 *
 * @see StructureTraits
 */
class DATA_CLASS_API ReconstructionTraits : public data::Object
{
public:
    fwCoreClassMacro(ReconstructionTraits, data::Object, data::factory::New< ReconstructionTraits >)
    fwCampMakeFriendDataMacro((sight)(data)(ReconstructionTraits));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API ReconstructionTraits(data::Object::Key key);

    /**
     * @brief destructor
     */
    DATA_API virtual ~ReconstructionTraits();

    /**
     * @{
     * @brief Get/Set value of the identifier.
     */
    std::string& getIdentifier ();
    const std::string& getIdentifier () const;
    void setIdentifier (const std::string& _identifier);
    /// @}

    /// Set the reconstruction operator mask node
    DATA_API void setMaskOpNode(const data::Node::sptr& maskOpNode );

    /// Get the reconstruction operator mask node. Return a null pointer if the mask node is not defined.
    DATA_API data::Node::sptr getMaskOpNode();

    /// Set the reconstruction operator mesh node.
    DATA_API void setMeshOpNode(const data::Node::sptr& meshOpNode );

    /// Get the reconstruction operator mesh node. Return a null pointer if the mesh node is not defined.
    DATA_API data::Node::sptr getMeshOpNode();

    /// Set the associated structure traits
    DATA_API void setStructureTraits(const data::StructureTraits::sptr& structureTraits );

    /// Get the associated structure traits
    DATA_API data::StructureTraits::sptr getStructureTraits();

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    /// ROI identifier
    std::string m_identifier;

    /// reconstruction mask operator node
    data::Node::sptr m_maskOpNode;

    /// reconstruction mesh operator node
    data::Node::sptr m_meshOpNode;

    /// associated structure traits
    data::StructureTraits::sptr m_structureTraits;

};

//-----------------------------------------------------------------------------

inline std::string& ReconstructionTraits::getIdentifier ()
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline const std::string& ReconstructionTraits::getIdentifier () const
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline void ReconstructionTraits::setIdentifier (const std::string& _identifier)
{
    m_identifier = _identifier;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
