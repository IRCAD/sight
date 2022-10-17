/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
class DATA_CLASS_API ROITraits : public Object
{
public:

    SIGHT_DECLARE_CLASS(ROITraits, Object, factory::New<ROITraits>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API ROITraits(Object::Key key);

    /**
     * @brief destructor
     */
    DATA_API ~ROITraits() noexcept override = default;

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
    StructureTraits::ROIExpression& getEvaluatedExp();
    const StructureTraits::ROIExpression& getEvaluatedExp() const;
    void setEvaluatedExp(const StructureTraits::ROIExpression& _evaluatedExp);
    /// @}

    /// Set the ROI mask node used for ROI
    DATA_API void setMaskOpNode(const Node::sptr& maskOpNode);

    /// Get the ROI mask node used for ROI, the only case where sptr can be null is when m_evaluatedExp correspond to
    // "W" ( W <=> World <=> no roi constraint ).
    DATA_API Node::sptr getMaskOpNode();
    DATA_API Node::csptr getMaskOpNode() const;

    /// Set the associated structure traits
    DATA_API void setStructureTraits(const StructureTraits::sptr& structureTraits);

    /// Get the associated structure traits
    DATA_API StructureTraits::sptr getStructureTraits();
    DATA_API StructureTraits::csptr getStructureTraits() const;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const ROITraits& other) const noexcept;
    DATA_API bool operator!=(const ROITraits& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

private:

    /// ROI identifier
    std::string m_identifier;

    /// mask node used for ROITraits
    Node::sptr m_maskOpNode;

    /// associated structure traits
    StructureTraits::sptr m_structureTraits;

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

inline StructureTraits::ROIExpression& ROITraits::getEvaluatedExp()
{
    return m_evaluatedExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression& ROITraits::getEvaluatedExp() const
{
    return m_evaluatedExp;
}

//-----------------------------------------------------------------------------

inline void ROITraits::setEvaluatedExp(const StructureTraits::ROIExpression& _evaluatedExp)
{
    m_evaluatedExp = _evaluatedExp;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
