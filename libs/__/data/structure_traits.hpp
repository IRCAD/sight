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
    enum category
    {
        body,
        head,
        neck,
        thorax,
        abdomen,
        pelvis,
        arm,
        leg,
        liver_segments,
        other
    };

    using category_container_t = std::vector<category>;
    using roi_expression_t     = std::string;

    /// Defines structure class
    enum structure_class
    {
        tool,
        environment,
        vessel,
        lesion,
        organ,
        functional,
        no_constraint
    };

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
    std::string type();
    const std::string& type() const;
    void set_type(const std::string& _type);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the categories.
     */
    category_container_t get_categories();
    const category_container_t& get_categories() const;
    void set_categories(const category_container_t& _categories);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the class.
     */
    structure_class get_class();
    const structure_class& get_class() const;
    void set_class(const structure_class& _class);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeExp.
     */
    roi_expression_t get_native_exp();
    const roi_expression_t& get_native_exp() const;
    void set_native_exp(const roi_expression_t& _native_exp);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeGeometricExp.
     */
    roi_expression_t get_native_geometric_exp();
    const roi_expression_t& get_native_geometric_exp() const;
    void set_native_geometric_exp(const roi_expression_t& _native_geometric_exp);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the attachmentType.
     */
    std::string get_attachment_type();
    const std::string& get_attachment_type() const;
    void set_attachment_type(const std::string& _attachment_type);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the color.
     */
    color::sptr get_color();
    color::csptr get_color() const;
    void set_color(const color::sptr& _color);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the anatomicRegion.
     */
    std::string get_anatomic_region();
    const std::string& get_anatomic_region() const;
    void set_anatomic_region(const std::string& _anatomic_region);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyCategory.
     */
    std::string get_property_category();
    const std::string& get_property_category() const;
    void set_property_category(const std::string& _property_category);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyType.
     */
    std::string get_property_type();
    const std::string& get_property_type() const;
    void set_property_type(const std::string& _property_type);
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const structure_traits& _other) const noexcept;
    DATA_API bool operator!=(const structure_traits& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    /// structure type (ie. Liver, Skin, ...)
    std::string m_type;

    /// structure categories (ie. abdomen, body, ...)
    category_container_t m_categories;

    /// structure default color
    color::sptr m_color;

    /// structure class (TOOL, ENVIRONMENT, VESSEL, LESION, ORGAN, FUNCTIONAL, NO_CONSTRAINT)
    structure_class m_class {no_constraint};

    /// native ROI expression (can be empty)
    roi_expression_t m_native_exp;

    /// native geometric ROI expression
    roi_expression_t m_native_geometric_exp;

    /// structure attachment type if class is LESION or FUNCTIONAL (can be empty)
    std::string m_attachment_type;

    /// Dicom designation of Anatomic region if unknown it must be set to "ToDefine"
    std::string m_anatomic_region;

    /// Dicom property category of organ if unknown it must be set to "ToDefine"
    std::string m_property_category;

    /// Dicom property type of organ if unknown it must be set to "ToDefine"
    std::string m_property_type;
};

//-----------------------------------------------------------------------------

inline std::string structure_traits::type()
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::type() const
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_type(const std::string& _type)
{
    m_type = _type;
}

//-----------------------------------------------------------------------------

inline structure_traits::category_container_t structure_traits::get_categories()
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline const structure_traits::category_container_t& structure_traits::get_categories() const
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_categories(const structure_traits::category_container_t& _categories)
{
    m_categories = _categories;
}

//-----------------------------------------------------------------------------

inline structure_traits::structure_class structure_traits::get_class()
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline const structure_traits::structure_class& structure_traits::get_class() const
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_class(const structure_traits::structure_class& _class)
{
    m_class = _class;
}

//-----------------------------------------------------------------------------

inline structure_traits::roi_expression_t structure_traits::get_native_exp()
{
    return m_native_exp;
}

//-----------------------------------------------------------------------------

inline const structure_traits::roi_expression_t& structure_traits::get_native_exp() const
{
    return m_native_exp;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_native_exp(const structure_traits::roi_expression_t& _native_exp)
{
    m_native_exp = _native_exp;
}

//-----------------------------------------------------------------------------

inline structure_traits::roi_expression_t structure_traits::get_native_geometric_exp()
{
    return m_native_geometric_exp;
}

//-----------------------------------------------------------------------------

inline const structure_traits::roi_expression_t& structure_traits::get_native_geometric_exp() const
{
    return m_native_geometric_exp;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_native_geometric_exp(const structure_traits::roi_expression_t& _native_geometric_exp)
{
    m_native_geometric_exp = _native_geometric_exp;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::get_attachment_type()
{
    return m_attachment_type;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::get_attachment_type() const
{
    return m_attachment_type;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_attachment_type(const std::string& _attachment_type)
{
    m_attachment_type = _attachment_type;
}

//-----------------------------------------------------------------------------

inline color::sptr structure_traits::get_color()
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline color::csptr structure_traits::get_color() const
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_color(const color::sptr& _color)
{
    m_color = _color;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::get_anatomic_region()
{
    return m_anatomic_region;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::get_anatomic_region() const
{
    return m_anatomic_region;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_anatomic_region(const std::string& _anatomic_region)
{
    m_anatomic_region = _anatomic_region;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::get_property_category()
{
    return m_property_category;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::get_property_category() const
{
    return m_property_category;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_property_category(const std::string& _property_category)
{
    m_property_category = _property_category;
}

//-----------------------------------------------------------------------------

inline std::string structure_traits::get_property_type()
{
    return m_property_type;
}

//-----------------------------------------------------------------------------

inline const std::string& structure_traits::get_property_type() const
{
    return m_property_type;
}

//-----------------------------------------------------------------------------

inline void structure_traits::set_property_type(const std::string& _property_type)
{
    m_property_type = _property_type;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
