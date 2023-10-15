/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/color.hpp"
#include "data/factory/new.hpp"
#include "data/object.hpp"

namespace sight::data
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
class DATA_CLASS_API structure_traits final : public object
{
public:

    SIGHT_DECLARE_CLASS(structure_traits, object);

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
    } Category;

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
    } StructureClass;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API structure_traits();

    /// Destructor. Does nothing.
    DATA_API ~structure_traits() noexcept override = default;

    /**
     * @{
     * @brief Get/Set value of the structure type.
     */
    std::string getType();
    const std::string& getType() const;
    void setType(const std::string& _type);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the categories.
     */
    CategoryContainer getCategories();
    const CategoryContainer& getCategories() const;
    void setCategories(const CategoryContainer& _categories);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the class.
     */
    StructureClass getClass();
    const StructureClass& getClass() const;
    void setClass(const StructureClass& _class);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeExp.
     */
    ROIExpression getNativeExp();
    const ROIExpression& getNativeExp() const;
    void setNativeExp(const ROIExpression& _nativeExp);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeGeometricExp.
     */
    ROIExpression getNativeGeometricExp();
    const ROIExpression& getNativeGeometricExp() const;
    void setNativeGeometricExp(const ROIExpression& _nativeGeometricExp);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the attachmentType.
     */
    std::string getAttachmentType();
    const std::string& getAttachmentType() const;
    void setAttachmentType(const std::string& _attachmentType);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the color.
     */
    color::sptr getColor();
    color::csptr getColor() const;
    void setColor(const color::sptr& _color);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the anatomicRegion.
     */
    std::string getAnatomicRegion();
    const std::string& getAnatomicRegion() const;
    void setAnatomicRegion(const std::string& _anatomicRegion);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyCategory.
     */
    std::string getPropertyCategory();
    const std::string& getPropertyCategory() const;
    void setPropertyCategory(const std::string& _propertyCategory);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyType.
     */
    std::string getPropertyType();
    const std::string& getPropertyType() const;
    void setPropertyType(const std::string& _propertyType);
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const structure_traits& other) const noexcept;
    DATA_API bool operator!=(const structure_traits& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& source,
        const std::unique_ptr<deep_copy_cache_t>& cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    /// structure type (ie. Liver, Skin, ...)
    std::string m_type;

    /// structure categories (ie. abdomen, body, ...)
    CategoryContainer m_categories;

    /// structure default color
    color::sptr m_color;

    /// structure class (TOOL, ENVIRONMENT, VESSEL, LESION, ORGAN, FUNCTIONAL, NO_CONSTRAINT)
    StructureClass m_class {NO_CONSTRAINT};

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

inline std::string structure_traits::getType()
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::getType() const
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setType(const std::string& _type)
{
    m_type = _type;
}

//-----------------------------------------------------------------------------

inline structure_traits::CategoryContainer structure_traits::getCategories()
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline const structure_traits::CategoryContainer& structure_traits::getCategories() const
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setCategories(const structure_traits::CategoryContainer& _categories)
{
    m_categories = _categories;
}

//-----------------------------------------------------------------------------

inline structure_traits::StructureClass structure_traits::getClass()
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline const structure_traits::StructureClass& structure_traits::getClass() const
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setClass(const structure_traits::StructureClass& _class)
{
    m_class = _class;
}

//-----------------------------------------------------------------------------

inline structure_traits::ROIExpression structure_traits::getNativeExp()
{
    return m_nativeExp;
}

//-----------------------------------------------------------------------------

inline const structure_traits::ROIExpression& structure_traits::getNativeExp() const
{
    return m_nativeExp;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setNativeExp(const structure_traits::ROIExpression& _nativeExp)
{
    m_nativeExp = _nativeExp;
}

//-----------------------------------------------------------------------------

inline structure_traits::ROIExpression structure_traits::getNativeGeometricExp()
{
    return m_nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline const structure_traits::ROIExpression& structure_traits::getNativeGeometricExp() const
{
    return m_nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setNativeGeometricExp(const structure_traits::ROIExpression& _nativeGeometricExp)
{
    m_nativeGeometricExp = _nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::getAttachmentType()
{
    return m_attachmentType;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::getAttachmentType() const
{
    return m_attachmentType;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setAttachmentType(const std::string& _attachmentType)
{
    m_attachmentType = _attachmentType;
}

//-----------------------------------------------------------------------------

inline color::sptr structure_traits::getColor()
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline color::csptr structure_traits::getColor() const
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setColor(const color::sptr& _color)
{
    m_color = _color;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::getAnatomicRegion()
{
    return m_anatomicRegion;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::getAnatomicRegion() const
{
    return m_anatomicRegion;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setAnatomicRegion(const std::string& _anatomicRegion)
{
    m_anatomicRegion = _anatomicRegion;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::getPropertyCategory()
{
    return m_propertyCategory;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::getPropertyCategory() const
{
    return m_propertyCategory;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setPropertyCategory(const std::string& _propertyCategory)
{
    m_propertyCategory = _propertyCategory;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::getPropertyType()
{
    return m_propertyType;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::getPropertyType() const
{
    return m_propertyType;
}

//-----------------------------------------------------------------------------

inline void structure_traits::setPropertyType(const std::string& _propertyType)
{
    m_propertyType = _propertyType;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
