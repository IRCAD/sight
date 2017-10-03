/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_STRUCTURETRAITS_HPP__
#define __FWDATA_STRUCTURETRAITS_HPP__

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
    const std::string  getType () const;
    std::string& getRefType ();
    const std::string& getCRefType () const;
    void setType (const std::string& _type);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the categories.
     */
    const CategoryContainer getCategories () const;
    CategoryContainer& getRefCategories ();
    const CategoryContainer& getCRefCategories () const;
    void setCategories (const CategoryContainer _categories);
    void setCRefCategories (const CategoryContainer& _categories);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the class.
     */
    const StructureClass getClass () const;
    StructureClass& getRefClass ();
    const StructureClass& getCRefClass () const;
    void setClass (const StructureClass _class);
    void setCRefClass (const StructureClass& _class);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeExp.
     */
    const ROIExpression  getNativeExp () const;
    ROIExpression& getRefNativeExp ();
    const ROIExpression& getCRefNativeExp () const;
    void setNativeExp (const ROIExpression _nativeExp);
    void setCRefNativeExp (const ROIExpression& _nativeExp);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the nativeGeometricExp.
     */
    const ROIExpression  getNativeGeometricExp () const;
    ROIExpression& getRefNativeGeometricExp ();
    const ROIExpression& getCRefNativeGeometricExp () const;
    void setNativeGeometricExp (const ROIExpression _nativeGeometricExp);
    void setCRefNativeGeometricExp (const ROIExpression& _nativeGeometricExp);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the attachmentType.
     */
    const std::string  getAttachmentType () const;
    std::string& getRefAttachmentType ();
    const std::string& getCRefAttachmentType () const;
    void setAttachmentType (const std::string _attachmentType);
    void setCRefAttachmentType (const std::string& _attachmentType);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the color.
     */
    const ::fwData::Color::sptr  getColor () const;
    ::fwData::Color::sptr & getRefColor();
    const ::fwData::Color::sptr& getCRefColor() const;
    void setColor (const ::fwData::Color::sptr _color);
    void setCRefColor (const ::fwData::Color::sptr& _color);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the anatomicRegion.
     */

    const std::string  getAnatomicRegion () const;
    std::string& getRefAnatomicRegion ();
    const std::string& getCRefAnatomicRegion () const;
    void setAnatomicRegion (const std::string _anatomicRegion);
    void setCRefAnatomicRegion (const std::string& _anatomicRegion);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyCategory.
     */
    const std::string  getPropertyCategory () const;
    std::string& getRefPropertyCategory ();
    const std::string& getCRefPropertyCategory () const;
    void setPropertyCategory (const std::string _propertyCategory);
    void setCRefPropertyCategory (const std::string& _propertyCategory);

    /// @}

    /**
     * @{
     * @brief Get/Set value of the propertyType.
     */
    const std::string  getPropertyType () const;
    std::string& getRefPropertyType ();
    const std::string& getCRefPropertyType () const;
    void setPropertyType (const std::string _propertyType);
    void setCRefPropertyType (const std::string& _propertyType);
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

inline const std::string StructureTraits::getType () const
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getRefType ()
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getCRefType () const
{
    return m_type;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setType (const std::string& _type)
{
    m_type = _type;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::CategoryContainer StructureTraits::getCategories () const
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline StructureTraits::CategoryContainer& StructureTraits::getRefCategories ()
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::CategoryContainer& StructureTraits::getCRefCategories () const
{
    return m_categories;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCategories (const StructureTraits::CategoryContainer _categories)
{
    m_categories = _categories;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefCategories (const StructureTraits::CategoryContainer& _categories)
{
    m_categories = _categories;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::StructureClass StructureTraits::getClass () const
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline StructureTraits::StructureClass& StructureTraits::getRefClass ()
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::StructureClass& StructureTraits::getCRefClass () const
{
    return m_class;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setClass (const StructureTraits::StructureClass _class)
{
    m_class = _class;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefClass (const StructureTraits::StructureClass& _class)
{
    m_class = _class;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression StructureTraits::getNativeExp () const
{
    return m_nativeExp;
}

//-----------------------------------------------------------------------------

inline StructureTraits::ROIExpression& StructureTraits::getRefNativeExp ()
{
    return m_nativeExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression& StructureTraits::getCRefNativeExp () const
{
    return m_nativeExp;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setNativeExp (const StructureTraits::ROIExpression _nativeExp)
{
    m_nativeExp = _nativeExp;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefNativeExp (const StructureTraits::ROIExpression& _nativeExp)
{
    m_nativeExp = _nativeExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression StructureTraits::getNativeGeometricExp () const
{
    return m_nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline StructureTraits::ROIExpression& StructureTraits::getRefNativeGeometricExp ()
{
    return m_nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline const StructureTraits::ROIExpression& StructureTraits::getCRefNativeGeometricExp () const
{
    return m_nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setNativeGeometricExp (const StructureTraits::ROIExpression _nativeGeometricExp)
{
    m_nativeGeometricExp = _nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefNativeGeometricExp (const StructureTraits::ROIExpression& _nativeGeometricExp)
{
    m_nativeGeometricExp = _nativeGeometricExp;
}

//-----------------------------------------------------------------------------

inline const std::string StructureTraits::getAttachmentType () const
{
    return m_attachmentType;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getRefAttachmentType ()
{
    return m_attachmentType;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getCRefAttachmentType () const
{
    return m_attachmentType;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setAttachmentType (const std::string _attachmentType)
{
    m_attachmentType = _attachmentType;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefAttachmentType (const std::string& _attachmentType)
{
    m_attachmentType = _attachmentType;
}

//-----------------------------------------------------------------------------

inline const ::fwData::Color::sptr StructureTraits::getColor () const
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline ::fwData::Color::sptr& StructureTraits::getRefColor()
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline const ::fwData::Color::sptr& StructureTraits::getCRefColor() const
{
    return m_color;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setColor (const ::fwData::Color::sptr _color)
{
    m_color = _color;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefColor (const ::fwData::Color::sptr& _color)
{
    m_color = _color;
}

//-----------------------------------------------------------------------------

inline const std::string StructureTraits::getAnatomicRegion () const
{
    return m_anatomicRegion;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getRefAnatomicRegion ()
{
    return m_anatomicRegion;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getCRefAnatomicRegion () const
{
    return m_anatomicRegion;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setAnatomicRegion (const std::string _anatomicRegion)
{
    m_anatomicRegion = _anatomicRegion;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefAnatomicRegion (const std::string& _anatomicRegion)
{
    m_anatomicRegion = _anatomicRegion;
}

//-----------------------------------------------------------------------------

inline const std::string StructureTraits::getPropertyCategory () const
{
    return m_propertyCategory;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getRefPropertyCategory ()
{
    return m_propertyCategory;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getCRefPropertyCategory () const
{
    return m_propertyCategory;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setPropertyCategory (const std::string _propertyCategory)
{
    m_propertyCategory = _propertyCategory;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefPropertyCategory (const std::string& _propertyCategory)
{
    m_propertyCategory = _propertyCategory;
}

//-----------------------------------------------------------------------------

inline const std::string StructureTraits::getPropertyType () const
{
    return m_propertyType;
}

//-----------------------------------------------------------------------------

inline std::string& StructureTraits::getRefPropertyType ()
{
    return m_propertyType;
}

//-----------------------------------------------------------------------------

inline const std::string& StructureTraits::getCRefPropertyType () const
{
    return m_propertyType;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setPropertyType (const std::string _propertyType)
{
    m_propertyType = _propertyType;
}

//-----------------------------------------------------------------------------

inline void StructureTraits::setCRefPropertyType (const std::string& _propertyType)
{
    m_propertyType = _propertyType;
}

//-----------------------------------------------------------------------------

} // namespace fwData

#endif // __FWDATA_STRUCTURETRAITS_HPP__

