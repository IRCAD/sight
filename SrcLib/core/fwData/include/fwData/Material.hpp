/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DATA_MATERIAL_HPP_
#define _DATA_MATERIAL_HPP_


#include "fwData/Object.hpp"
#include "fwData/Color.hpp"

namespace fwData
{
/**
 * @class   Material
 * @brief   This class defines a material. A material is represented by an ambient color and a diffuse color
 * @see     ::fwData::Color
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API Material : public Object {

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Material)(::fwData::Object), (()), ::fwTools::Factory::New< Material >) ;

    /// Constructor
    FWDATA_API Material();

    /// Destructor
    FWDATA_API virtual ~Material();


    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Material::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Material::csptr _source );

    /**
    * @brief returns editable ambient color
    */
    FWDATA_API Color::sptr ambient() const;

    /**
    * @brief returns editable diffuse color
    */
    FWDATA_API Color::sptr diffuse() const;

    /*
     * @brief Options
     */
    typedef enum
    {
        MODE_STANDARD = 1,
        MODE_NORMALS  = 2,
    } OPTIONS_MODE;

    /*
     * @brief Shading models
     */
    typedef enum
    {
        MODE_FLAT    = 1,
        MODE_GOURAUD = 2,//MODE_FLAT<<1 -> 10
        MODE_PHONG   = 4,//MODE_FLAT<<2 -> 100
    } SHADING_MODE;

    /*
     * @brief Representation models
     */
    typedef enum
    {
        MODE_SURFACE   = 1, //1
        MODE_POINT     = 2, //MODE_SURFACE<<1 -> 10
        MODE_WIREFRAME = 4, //MODE_SURFACE<<2 -> 100
        MODE_EDGE      = 5, //MODE_SURFACE|MODE_WIREFRAME -> 101
    } REPRESENTATION_MODE;

    fwGettersSettersDocMacro(ShadingMode, shadingMode, SHADING_MODE, the shading models(flat, gouraud, phong).);
    fwGettersSettersDocMacro(RepresentationMode, representationMode, REPRESENTATION_MODE, the representation models(edge, point, wireframe, surface).);
    fwGettersSettersDocMacro(OptionsMode, optionsMode, OPTIONS_MODE, the option representation (Standard, normals).);

protected :

    SHADING_MODE m_shadingMode;
    REPRESENTATION_MODE m_representationMode;
    OPTIONS_MODE m_optionsMode;

    Color::sptr m_ambient;
    Color::sptr m_diffuse;
};

}

#endif /* _DATA_MATERIAL_HPP_ */
