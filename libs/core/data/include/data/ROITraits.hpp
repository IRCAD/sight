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

fwCampAutoDeclareDataMacro((sight)(data)(ROITraits));

namespace sight::data
{
/**
 * @brief   This class defines ROI traits.
 *
 * A ROI traits contains :
 *  - an identifier
 *  - an evaluated expression : ex. inter(skin,liver)
 *  - a ROI mask node used for ROI, the only case where sptr can be null is when evaluated expression correspond to "W"
 *( W <=> World <=> no roi constraint ).
 *  - a structure traits associated to the ROI
 *
 * @note Several ROITraits can be associated to the same structure.
 * @see StructureTraits
 */
class DATA_CLASS_API ROITraits : public data::Object
{
public:
    fwCoreClassMacro(ROITraits, data::Object, data::factory::New< ROITraits >)
    fwCampMakeFriendDataMacro((sight)(data)(ROITraits));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API ROITraits(data::Object::Key key);

    /**
     * @brief destructor
     */
    DATA_API virtual ~ROITraits();

    /**
     * @{
     * @brief Get/Set value of the ROIs identifier.
     */
    std::string& getIdentifier();
    const std::string& getIdentifier() const;
    void setIdentifier(const std::string& _identifier);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the ROI evaluated expression.
     */
    StructureTraits::ROIExpression& getEvaluatedExp ();
    const StructureTraits::ROIExpression& getEvaluatedExp () const;
    void setEvaluatedExp (const StructureTraits::ROIExpression& _evaluatedExp);
    /// @}

    /// Set the ROI mask node used for ROI
    DATA_API void setMaskOpNode(const data::Node::sptr& maskOpNode );

    /// Get the ROI mask node used for ROI, the only case where sptr can be null is when m_evaluatedExp correspond to
    // "W" ( W <=> World <=> no roi constraint ).
    DATA_API data::Node::sptr getMaskOpNode();

    /// Set the associated structure traits
    DATA_API void setStructureTraits(const data::StructureTraits::sptr& structureTraits );

    /// Get the associated structure traits
    DATA_API data::StructureTraits::sptr getStructureTraits();

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    /// ROI identifier
    std::string m_identifier;

    /// mask node used for ROITraits
    data::Node::sptr m_maskOpNode;

    /// associated structure traits
    data::StructureTraits::sptr m_structureTraits;

    /// roi evaluated expression
    StructureTraits::ROIExpression m_evaluatedExp;
};

//-----------------------------------------------------------------------------

inline std::string& ROITraits::getIdentifier()
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline const std::string& ROITraits::getIdentifier() const
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline void ROITraits::setIdentifier(const std::string& _identifier)
{
    m_identifier = _identifier;
}

//-----------------------------------------------------------------------------

inline StructureTraits::ROIExpression& ROITraits::getEvaluatedExp ()
{
    return m_evaluatedExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression& ROITraits::getEvaluatedExp () const
{
    return m_evaluatedExp;
}

//-----------------------------------------------------------------------------

inline void ROITraits::setEvaluatedExp (const StructureTraits::ROIExpression& _evaluatedExp)
{
    m_evaluatedExp = _evaluatedExp;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
