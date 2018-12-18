/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwData/Color.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

fwCampAutoDeclareDataMacro((fwData)(StructureTraits), FWDATA_API);

namespace fwData
{
/**
 * @brief This class defines structure traits.
 *
 * A structure traits contains :
 *  - [required] a type (ie. Skin, Liver, ...)
 *  - [required] a category (or more) : Body, Head, Neck, Thorax, Abdomen, Pelvis, Arm, Leg, Liver_segments, Other
 *  - [required] a class : Tool, Environment, Vessel, Lesion, Organ, Functional, No_constraint
 *  - [required] a color
 *  - [optional] an attachment only if class is LESION or FUNCTIONAL
 *  - [optional] a native ROI expression : ex. inter(world(type(Skin)),not(class(Organ)))
 *  - [optional] a native geometric ROI expression
 */
class FWDATA_CLASS_API StructureTraits : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (StructureTraits)(::fwData::Object), (()),
                                            ::fwData::factory::New< StructureTraits >);
    fwCampMakeFriendDataMacro((fwData)(StructureTraits));

    /// Defines structure categories
    typedef enum
    {
        BODY,
        HEAD,
        NECK,
        THORAX,
        ABDOMEN,
        PELVIS,
        ARM,
        LEG,
        LIVER_SEGMENTS,
        OTHER
    }Category;

    typedef std::vector<Category> CategoryContainer;
    typedef std::string ROIExpression;

    /// Defines structure class
    typedef enum
    {
        TOOL,
        ENVIRONMENT,
        VESSEL,
        LESION,
        ORGAN,
        FUNCTIONAL,
        NO_CONSTRAINT
    }StructureClass;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API StructureTraits(::fwData::Object::Key key);

    /// Destructor. Does nothing.
    FWDATA_API virtual ~StructureTraits();

    /**
     * @{
     * @brief Get/Set value of the structure type.
     */
    std::string& getType ();
    const std::string& getType () const;
    void setType (const std::string& _type);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the categories.
     */
    CategoryContainer& getCategories ();
    const CategoryContainer& getCategories () const;
    void setCategories (const CategoryContainer& _categories);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the class.
     */
    StructureClass& getClass ();
    const StructureClass& getClass () const;
    void setClass (const StructureClass& _class);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeExp.
     */
    ROIExpression& getNativeExp ();
    const ROIExpression& getNativeExp () const;
    void setNativeExp (const ROIExpression& _nativeExp);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeGeometricExp.
     */
    ROIExpression& getNativeGeometricExp ();
    const ROIExpression& getNativeGeometricExp () const;
    void setNativeGeometricExp (const ROIExpression& _nativeGeometricExp);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the attachmentType.
     */
    std::string& getAttachmentType ();
    const std::string& getAttachmentType () const;
    void setAttachmentType (const std::string& _attachmentType);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the color.
     */
    ::fwData::Color::sptr& getColor();
    const ::fwData::Color::sptr& getColor() const;
    void setColor (const ::fwData::Color::sptr& _color);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the anatomicRegion.
     */
    std::string& getAnatomicRegion ();
    const std::string& getAnatomicRegion () const;
    void setAnatomicRegion (const std::string& _anatomicRegion);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyCategory.
     */
    std::string& getPropertyCategory ();
    const std::string& getPropertyCategory () const;
    void setPropertyCategory (const std::string& _propertyCategory);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyType.
     */
    std::string& getPropertyType ();
    const std::string& getPropertyType () const;
    void setPropertyType (const std::string& _propertyType);
    /// @}

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    /// structure type (ie. Liver, Skin, ...)
    std::string m_type;

    /// structure categories (ie. abdomen, body, ...)
    CategoryContainer m_categories;

    /// structure default color
    ::fwData::Color::sptr m_color;

    /// structure class (TOOL, ENVIRONMENT, VESSEL, LESION, ORGAN, FUNCTIONAL, NO_CONSTRAINT)
    StructureClass m_class;

    /// native ROI expression (can be empty)
    ROIExpression m_nativeExp;

    /// native geometric ROI expression
    ROIExpression m_nativeGeometricExp;

    /// structure attachment type if class is LESION or FUNCTIONAL (can be empty)
    std::string m_attachmentType;

    /// Dicom designation of Anatomic region if unknown it must be set to "ToDefine"
    std::string m_anatomicRegion;

    /// Dicom property category of organ if unknown it must be set to "ToDefine"
    std::string m_propertyCategory;

    /// Dicom property type of organ if unknown it must be set to "ToDefine"
    std::string m_propertyType;
};

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getType ()
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getType () const
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setType (const std::string& _type)
{
    m_type = _type;
}

//-----------------------------------------------------------------------------

inline StructureTraits::CategoryContainer& StructureTraits::getCategories ()
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::CategoryContainer& StructureTraits::getCategories () const
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCategories (const StructureTraits::CategoryContainer& _categories)
{
    m_categories = _categories;
}

//-----------------------------------------------------------------------------

inline StructureTraits::StructureClass& StructureTraits::getClass ()
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::StructureClass& StructureTraits::getClass () const
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setClass (const StructureTraits::StructureClass& _class)
{
    m_class = _class;
}

//-----------------------------------------------------------------------------

inline StructureTraits::ROIExpression& StructureTraits::getNativeExp ()
{
    return m_nativeExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression& StructureTraits::getNativeExp () const
{
    return m_nativeExp;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setNativeExp (const StructureTraits::ROIExpression& _nativeExp)
{
    m_nativeExp = _nativeExp;
}

//-----------------------------------------------------------------------------

inline StructureTraits::ROIExpression& StructureTraits::getNativeGeometricExp ()
{
    return m_nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression& StructureTraits::getNativeGeometricExp () const
{
    return m_nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setNativeGeometricExp (const StructureTraits::ROIExpression& _nativeGeometricExp)
{
    m_nativeGeometricExp = _nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getAttachmentType ()
{
    return m_attachmentType;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getAttachmentType () const
{
    return m_attachmentType;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setAttachmentType (const std::string& _attachmentType)
{
    m_attachmentType = _attachmentType;
}

//-----------------------------------------------------------------------------

inline ::fwData::Color::sptr& StructureTraits::getColor()
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline const ::fwData::Color::sptr& StructureTraits::getColor() const
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setColor (const ::fwData::Color::sptr& _color)
{
    m_color = _color;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getAnatomicRegion ()
{
    return m_anatomicRegion;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getAnatomicRegion () const
{
    return m_anatomicRegion;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setAnatomicRegion (const std::string& _anatomicRegion)
{
    m_anatomicRegion = _anatomicRegion;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getPropertyCategory ()
{
    return m_propertyCategory;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getPropertyCategory () const
{
    return m_propertyCategory;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setPropertyCategory (const std::string& _propertyCategory)
{
    m_propertyCategory = _propertyCategory;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getPropertyType ()
{
    return m_propertyType;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getPropertyType () const
{
    return m_propertyType;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setPropertyType (const std::string& _propertyType)
{
    m_propertyType = _propertyType;
}

//-----------------------------------------------------------------------------

} // namespace fwData
