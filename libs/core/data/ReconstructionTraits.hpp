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

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (ReconstructionTraits));

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

    SIGHT_DECLARE_CLASS(ReconstructionTraits, data::Object, data::factory::New<ReconstructionTraits>);
    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (ReconstructionTraits));

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
    std::string& getIdentifier();
    const std::string& getIdentifier() const;
    void setIdentifier(const std::string& _identifier);
    /// @}

    /// Set the reconstruction operator mask node
    DATA_API void setMaskOpNode(const data::Node::sptr& maskOpNode);

    /// Get the reconstruction operator mask node. Return a null pointer if the mask node is not defined.
    DATA_API data::Node::sptr getMaskOpNode();
    //------------------------------------------------------------------------------

    DATA_API data::Node::csptr getMaskOpNode() const
    {
        return m_maskOpNode;
    }

    /// Set the reconstruction operator mesh node.
    DATA_API void setMeshOpNode(const data::Node::sptr& meshOpNode);

    /// Get the reconstruction operator mesh node. Return a null pointer if the mesh node is not defined.
    DATA_API data::Node::sptr getMeshOpNode();
    //------------------------------------------------------------------------------

    DATA_API data::Node::csptr getMeshOpNode() const
    {
        return m_meshOpNode;
    }

    /// Set the associated structure traits
    DATA_API void setStructureTraits(const data::StructureTraits::sptr& structureTraits);

    /// Get the associated structure traits
    DATA_API data::StructureTraits::sptr getStructureTraits();
    //------------------------------------------------------------------------------

    DATA_API data::StructureTraits::csptr getStructureTraits() const
    {
        return m_structureTraits;
    }

    /**
     * @brief Defines shallow copy.
     * @param _source the source object where find data.
     */
    DATA_API void shallowCopy(const data::Object::csptr& _source) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object where find data.
     * @param _cache contains all copied objects to avoid duplication.
     */
    DATA_API void cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& _cache) override;

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

inline std::string& ReconstructionTraits::getIdentifier()
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline const std::string& ReconstructionTraits::getIdentifier() const
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline void ReconstructionTraits::setIdentifier(const std::string& _identifier)
{
    m_identifier = _identifier;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
