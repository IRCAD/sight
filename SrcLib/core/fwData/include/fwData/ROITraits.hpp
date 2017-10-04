/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_ROITRAITS_HPP__
#define __FWDATA_ROITRAITS_HPP__

#include "fwData/factory/new.hpp"
#include "fwData/Node.hpp"
#include "fwData/StructureTraits.hpp"

fwCampAutoDeclareDataMacro((fwData)(ROITraits), FWDATA_API);

namespace fwData
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
class FWDATA_CLASS_API ROITraits : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ROITraits)(::fwData::Object), (()), ::fwData::factory::New< ROITraits >);
    fwCampMakeFriendDataMacro((fwData)(ROITraits));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API ROITraits(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~ROITraits();

    /**
     * @{
     * @brief Get/Set value of the ROIs identifier.
     */
    const std::string  getIdentifier() const;
    std::string& getRefIdentifier();
    const std::string& getCRefIdentifier() const;
    void setIdentifier(const std::string& _identifier);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the ROI evaluated expression.
     */
    const StructureTraits::ROIExpression  getEvaluatedExp () const;
    StructureTraits::ROIExpression& getRefEvaluatedExp ();
    const StructureTraits::ROIExpression& getCRefEvaluatedExp () const;
    void setEvaluatedExp (const StructureTraits::ROIExpression _evaluatedExp);
    void setCRefEvaluatedExp (const StructureTraits::ROIExpression& _evaluatedExp);
    /// @}

    /// Set the ROI mask node used for ROI
    FWDATA_API void setMaskOpNode(const ::fwData::Node::sptr& maskOpNode );

    /// Get the ROI mask node used for ROI, the only case where sptr can be null is when m_evaluatedExp correspond to
    // "W" ( W <=> World <=> no roi constraint ).
    FWDATA_API ::fwData::Node::sptr getMaskOpNode();

    /// Set the associated structure traits
    FWDATA_API void setStructureTraits(const ::fwData::StructureTraits::sptr& structureTraits );

    /// Get the associated structure traits
    FWDATA_API ::fwData::StructureTraits::sptr getStructureTraits();

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    /// ROI identifier
    std::string m_identifier;

    /// mask node used for ROITraits
    ::fwData::Node::sptr m_maskOpNode;

    /// associated structure traits
    ::fwData::StructureTraits::sptr m_structureTraits;

    /// roi evaluated expression
    StructureTraits::ROIExpression m_evaluatedExp;
};

//-----------------------------------------------------------------------------

inline const std::string ROITraits::getIdentifier() const
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline std::string& ROITraits::getRefIdentifier()
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline const std::string& ROITraits::getCRefIdentifier() const
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline void ROITraits::setIdentifier(const std::string& _identifier)
{
    m_identifier = _identifier;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression ROITraits::getEvaluatedExp () const
{
    return m_evaluatedExp;
}

//-----------------------------------------------------------------------------

inline StructureTraits::ROIExpression& ROITraits::getRefEvaluatedExp ()
{
    return m_evaluatedExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression& ROITraits::getCRefEvaluatedExp () const
{
    return m_evaluatedExp;
}

//-----------------------------------------------------------------------------

inline void ROITraits::setEvaluatedExp (const StructureTraits::ROIExpression _evaluatedExp)
{
    m_evaluatedExp = _evaluatedExp;
}

//-----------------------------------------------------------------------------

inline void ROITraits::setCRefEvaluatedExp (const StructureTraits::ROIExpression& _evaluatedExp)
{
    m_evaluatedExp = _evaluatedExp;
}

//-----------------------------------------------------------------------------

} // namespace fwData

#endif // __FWDATA_ROITRAITS_HPP__

