/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_MATERIAL_HPP__
#define __FWDATA_MATERIAL_HPP__

#include "fwData/Object.hpp"
#include "fwData/Color.hpp"
#include "fwData/Image.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Material), FWDATA_API);

namespace fwData
{

/**
 * @brief This class defines a material. A material is represented by an ambient color and a diffuse color.
 * @see ::fwData::Color
 */
class FWDATA_CLASS_API Material : public Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Material)(::fwData::Object), (()), ::fwData::factory::New< Material >);
    fwCampMakeFriendDataMacro((fwData)(Material));


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Material(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Material();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /**
     * @brief returns editable ambient color
     */
    FWDATA_API Color::sptr ambient() const;

    /**
     * @brief Setter for ambient color
     *
     * @param ambient color
     */
    FWDATA_API void setAmbient(const Color::sptr& ambient);

    /**
     * @brief returns editable diffuse color
     */
    FWDATA_API Color::sptr diffuse() const;

    /**
     * @brief Setter for diffuse color
     *
     * @param diffuse color
     */
    FWDATA_API void setDiffuse(const Color::sptr& diffuse);

    /**
     * @brief returns editable diffuse texture
     */
    FWDATA_API Image::sptr getDiffuseTexture() const;

    /**
     * @brief Setter for diffuse texture
     *
     * @param diffuseTexture texture
     */
    FWDATA_API void setDiffuseTexture(const Image::sptr& diffuseTexture);

    /**
     * @brief Options
     */
    typedef enum
    {
        MODE_STANDARD = 1,
        MODE_NORMALS  = 2,
    } OPTIONS_MODE;

    /**
     * @brief Shading models
     */
    typedef enum
    {
        MODE_FLAT    = 1,
        MODE_GOURAUD = 2, //MODE_FLAT<<1 -> 10
        MODE_PHONG   = 4, //MODE_FLAT<<2 -> 100
    } SHADING_MODE;

    /**
     * @brief Representation models
     */
    typedef enum
    {
        MODE_SURFACE   = 1, //1
        MODE_POINT     = 2, //MODE_SURFACE<<1 -> 10
        MODE_WIREFRAME = 4, //MODE_SURFACE<<2 -> 100
        MODE_EDGE      = 5, //MODE_SURFACE|MODE_WIREFRAME -> 101
    } REPRESENTATION_MODE;


    /**
     * @brief Texture filtering types
     */
    typedef enum
    {
        NEAREST,
        LINEAR,
    } FilteringType;

    /**
     * @brief Texture wrapping types
     */
    typedef enum
    {
        CLAMP,
        REPEAT,
    } WrappingType;

    /** @{
     *  @brief get/set the shading models(flat, gouraud, phong)
     */
    const SHADING_MODE getShadingMode() const;
    SHADING_MODE& getRefShadingMode();
    const SHADING_MODE & getCRefShadingMode() const;
    void setShadingMode(SHADING_MODE _shadingMode);
    /// @}

    /** @{
     *  @brief get/set the representation models (edge, point, wireframe, surface)
     */
    const REPRESENTATION_MODE getRepresentationMode() const;
    REPRESENTATION_MODE& getRefRepresentationMode();
    const REPRESENTATION_MODE& getCRefRepresentationMode() const;
    void setRepresentationMode(REPRESENTATION_MODE _representationMode);
    /// @}

    /** @{
     *  @brief get/set the option representation (Standard, normals)
     */
    const OPTIONS_MODE  getOptionsMode () const;
    OPTIONS_MODE& getRefOptionsMode ();
    const OPTIONS_MODE& getCRefOptionsMode () const;
    void setOptionsMode (OPTIONS_MODE _optionsMode);
    /// @}

    /** @{
     *  @brief get/set the texture filtering
     */
    const FilteringType getDiffuseTextureFiltering() const;
    FilteringType& getRefDiffuseTextureFiltering();
    const FilteringType& getCRefDiffuseTextureFiltering() const;
    void setDiffuseTextureFiltering(FilteringType _diffuseTextureFiltering);
    /// @}

    /** @{
     *  @brief get/set the texture wrapping
     */
    const WrappingType  getDiffuseTextureWrapping () const;
    WrappingType & getRefDiffuseTextureWrapping ();
    const WrappingType & getCRefDiffuseTextureWrapping () const;
    void setDiffuseTextureWrapping (WrappingType _diffuseTextureWrapping);
    /// @}

protected:

    /// Shading mode (flat, Gouraud, Phong)
    SHADING_MODE m_shadingMode;

    /// Fill mode (flat, wireframe, point)
    REPRESENTATION_MODE m_representationMode;

    /// Options mode (display normals or not)
    OPTIONS_MODE m_optionsMode;

    /// Ambient color
    Color::sptr m_ambient;

    /// Diffuse color
    Color::sptr m_diffuse;

    /// Diffuse texture
    Image::sptr m_diffuseTexture;

    /// Filtering for diffuse texture
    FilteringType m_diffuseTextureFiltering;

    /// Wrapping for diffuse texture
    WrappingType m_diffuseTextureWrapping;
};

//-----------------------------------------------------------------------------

inline const Material::SHADING_MODE Material::getShadingMode() const
{
    return m_shadingMode;
}

//-----------------------------------------------------------------------------

inline Material::SHADING_MODE& Material::getRefShadingMode()
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline const Material::SHADING_MODE & Material::getCRefShadingMode() const
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline void Material::setShadingMode(SHADING_MODE _shadingMode)
{
    this->m_shadingMode = _shadingMode;
}

//-----------------------------------------------------------------------------

inline const Material::REPRESENTATION_MODE Material::getRepresentationMode() const
{
    return m_representationMode;
}

//-----------------------------------------------------------------------------

inline Material::REPRESENTATION_MODE& Material::getRefRepresentationMode()
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline const Material::REPRESENTATION_MODE& Material::getCRefRepresentationMode() const
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline void Material::setRepresentationMode(REPRESENTATION_MODE _representationMode)
{
    this->m_representationMode = _representationMode;
}

//-----------------------------------------------------------------------------

inline const Material::OPTIONS_MODE Material::getOptionsMode () const
{
    return m_optionsMode;
}

//-----------------------------------------------------------------------------

inline Material::OPTIONS_MODE& Material::getRefOptionsMode ()
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline const Material::OPTIONS_MODE& Material::getCRefOptionsMode () const
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline void Material::setOptionsMode (OPTIONS_MODE _optionsMode)
{
    this->m_optionsMode = _optionsMode;
}

//-----------------------------------------------------------------------------

inline const Material::FilteringType Material::getDiffuseTextureFiltering() const
{
    return m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline Material::FilteringType& Material::getRefDiffuseTextureFiltering()
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline const Material::FilteringType& Material::getCRefDiffuseTextureFiltering() const
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline void Material::setDiffuseTextureFiltering(FilteringType _diffuseTextureFiltering)
{
    this->m_diffuseTextureFiltering = _diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline const Material::WrappingType Material::getDiffuseTextureWrapping () const
{
    return m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline Material::WrappingType & Material::getRefDiffuseTextureWrapping ()
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline const Material::WrappingType & Material::getCRefDiffuseTextureWrapping () const
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline void Material::setDiffuseTextureWrapping (WrappingType _diffuseTextureWrapping)
{
    this->m_diffuseTextureWrapping = _diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

} //namespace fwData

#endif /* __FWDATA_MATERIAL_HPP__ */

