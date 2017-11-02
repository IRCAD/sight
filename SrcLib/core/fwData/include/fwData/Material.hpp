/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_MATERIAL_HPP__
#define __FWDATA_MATERIAL_HPP__

#include "fwData/Color.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Image.hpp"
#include "fwData/Object.hpp"

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
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

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
        STANDARD      = 1,
        NORMALS       = 2,
        CELLS_NORMALS = 3,
    } OptionsType;

    /**
     * @brief Shading models
     */
    typedef enum
    {
        AMBIENT = 0,
        FLAT    = 1,
        GOURAUD = 2, //FLAT<<1 -> 10
        PHONG   = 4, //FLAT<<2 -> 100
    } ShadingType;

    /**
     * @brief Representation models
     */
    typedef enum
    {
        SURFACE   = 1, //1
        POINT     = 2, //SURFACE<<1 -> 10
        WIREFRAME = 4, //SURFACE<<2 -> 100
        EDGE      = 5, //SURFACE|WIREFRAME -> 101
    } RepresentationType;

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
    const ShadingType getShadingMode() const;
    ShadingType& getRefShadingMode();
    const ShadingType& getCRefShadingMode() const;
    void setShadingMode(ShadingType _shadingMode);
    /// @}

    /** @{
     *  @brief get/set the representation models (edge, point, wireframe, surface)
     */
    const RepresentationType getRepresentationMode() const;
    RepresentationType& getRefRepresentationMode();
    const RepresentationType& getCRefRepresentationMode() const;
    void setRepresentationMode(RepresentationType _representationMode);
    /// @}

    /** @{
     *  @brief get/set the option representation (Standard, normals)
     */
    const OptionsType  getOptionsMode () const;
    OptionsType& getRefOptionsMode ();
    const OptionsType& getCRefOptionsMode () const;
    void setOptionsMode (OptionsType _optionsMode);
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
    WrappingType& getRefDiffuseTextureWrapping ();
    const WrappingType& getCRefDiffuseTextureWrapping () const;
    void setDiffuseTextureWrapping (WrappingType _diffuseTextureWrapping);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a texture is added
    typedef ::fwCom::Signal< void (::fwData::Image::sptr) > AddedTextureSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_ADDED_TEXTURE_SIG;

    /// Type of signal when a texture is removed
    typedef ::fwCom::Signal< void (::fwData::Image::sptr) > RemovedTextureSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_REMOVED_TEXTURE_SIG;
    /**
     * @}
     */

protected:

    /// Shading mode (flat, Gouraud, Phong)
    ShadingType m_shadingMode;

    /// Fill mode (flat, wireframe, point)
    RepresentationType m_representationMode;

    /// Options mode (display normals or not)
    OptionsType m_optionsMode;

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

inline const Material::ShadingType Material::getShadingMode() const
{
    return m_shadingMode;
}

//-----------------------------------------------------------------------------

inline Material::ShadingType& Material::getRefShadingMode()
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline const Material::ShadingType& Material::getCRefShadingMode() const
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline void Material::setShadingMode(ShadingType _shadingMode)
{
    this->m_shadingMode = _shadingMode;
}

//-----------------------------------------------------------------------------

inline const Material::RepresentationType Material::getRepresentationMode() const
{
    return m_representationMode;
}

//-----------------------------------------------------------------------------

inline Material::RepresentationType& Material::getRefRepresentationMode()
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline const Material::RepresentationType& Material::getCRefRepresentationMode() const
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline void Material::setRepresentationMode(RepresentationType _representationMode)
{
    this->m_representationMode = _representationMode;
}

//-----------------------------------------------------------------------------

inline const Material::OptionsType Material::getOptionsMode () const
{
    return m_optionsMode;
}

//-----------------------------------------------------------------------------

inline Material::OptionsType& Material::getRefOptionsMode ()
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline const Material::OptionsType& Material::getCRefOptionsMode () const
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline void Material::setOptionsMode (OptionsType _optionsMode)
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

inline Material::WrappingType& Material::getRefDiffuseTextureWrapping ()
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline const Material::WrappingType& Material::getCRefDiffuseTextureWrapping () const
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

