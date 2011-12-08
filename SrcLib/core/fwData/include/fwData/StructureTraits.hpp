/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_STRUCTURETRAITS_HPP__
#define __FWDATA_STRUCTURETRAITS_HPP__

#include "fwData/Color.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class   StructureTraits
 * @brief   This class defines structure traits.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2011.
 *
 * A structure traits contains :
 *  - [required] a type (ie. Skin, Liver, ...)
 *  - [required] a category (or more) : Body, Head, Neck, Thorax, Abdomen, Pelvis, Arm, Leg, Liver_segments, Other
 *  - [required] a class : Tool, Environment, Vessel, Lesion, Organ, Functional, No_constraint
 *  - [required] a color
 *  - [optional] an attachment if class is LESION or FUNCTIONAL
 *  - [optional] a native ROI expression : ex. inter(world(type(Skin)),not(class(Organ)))
 *  - [optional] a native geometric ROI expression
 */
class FWDATA_CLASS_API StructureTraits : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (StructureTraits)(::fwData::Object), (()), ::fwTools::Factory::New< StructureTraits >) ;

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

    fwGettersSettersDocMacro(Type, type, std::string, the structure type);

    fwGettersSettersDocMacro(Categories, categories, CategoryContainer, the structure type);

    fwGettersSettersDocMacro(Class, class, StructureClass, the structure class);

    fwGettersSettersDocMacro(NativeExp, nativeExp, ROIExpression, the native ROI expression);

    fwGettersSettersDocMacro(NativeGeometricExp, nativeGeometricExp, ROIExpression, the native geometric ROI expression);

    fwGettersSettersDocMacro(AttachmentType, attachmentType, std::string, structure attachment type if class is LESION or FUNCTIONAL);

    fwGettersSettersDocMacro(Color, color, ::fwData::Color::sptr, structure default color);

protected :

    /// Constructor
    FWDATA_API StructureTraits();

    /// Destructor. Does nothing.
    FWDATA_API virtual ~StructureTraits();

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

};

} // namespace fwData

#endif // __FWDATA_STRUCTURETRAITS_HPP__
