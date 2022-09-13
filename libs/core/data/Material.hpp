/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/Color.hpp"
#include "data/factory/new.hpp"
#include "data/Image.hpp"
#include "data/Object.hpp"

namespace sight::data
{

/**
 * @brief This class defines a material. A material is represented by an ambient color and a diffuse color.
 * @see Color
 */
class DATA_CLASS_API Material final : public Object
{
public:

    SIGHT_DECLARE_CLASS(Material, Object, factory::New<Material>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Material(Object::Key key);

    /// Destructor
    DATA_API ~Material() noexcept override = default;

    /**
     * @brief returns editable ambient color
     */
    DATA_API Color::sptr ambient() const;

    /**
     * @brief Setter for ambient color
     *
     * @param ambient color
     */
    DATA_API void setAmbient(const Color::sptr& ambient);

    /**
     * @brief returns editable diffuse color
     */
    DATA_API Color::sptr diffuse() const;

    /**
     * @brief Setter for diffuse color
     *
     * @param diffuse color
     */
    DATA_API void setDiffuse(const Color::sptr& diffuse);

    /**
     * @brief returns editable diffuse texture
     * @{
     */
    DATA_API Image::sptr getDiffuseTexture();
    DATA_API Image::csptr getDiffuseTexture() const;
    ///@}

    /**
     * @brief Setter for diffuse texture
     *
     * @param diffuseTexture texture
     */
    DATA_API void setDiffuseTexture(const Image::sptr& diffuseTexture);

    /**
     * @brief Options
     */
    typedef enum
    {
        STANDARD      = 1,
        NORMALS       = 2,
        CELLS_NORMALS = 3
    } OptionsType;

    /**
     * @brief Shading models
     */
    typedef enum
    {
        AMBIENT = 0,
        FLAT    = 1,
        PHONG   = 2
    } ShadingType;

    /**
     * @brief Representation models
     */
    typedef enum
    {
        SURFACE   = 1, //1
        POINT     = 2, //SURFACE<<1 -> 10
        WIREFRAME = 4, //SURFACE<<2 -> 100
        EDGE      = 5  //SURFACE|WIREFRAME -> 101
    } RepresentationType;

    /**
     * @brief Texture filtering types
     */
    typedef enum
    {
        NEAREST,
        LINEAR
    } FilteringType;

    /**
     * @brief Texture wrapping types
     */
    typedef enum
    {
        CLAMP,
        REPEAT
    } WrappingType;

    /** @{
     *  @brief get/set the shading models(flat, phong)
     */
    ShadingType& getShadingMode();
    const ShadingType& getShadingMode() const;
    void setShadingMode(ShadingType _shadingMode);
    /// @}

    /** @{
     *  @brief get/set the representation models (edge, point, wireframe, surface)
     */
    RepresentationType& getRepresentationMode();
    const RepresentationType& getRepresentationMode() const;
    void setRepresentationMode(RepresentationType _representationMode);
    /// @}

    /** @{
     *  @brief get/set the option representation (Standard, normals)
     */
    OptionsType& getOptionsMode();
    const OptionsType& getOptionsMode() const;
    void setOptionsMode(OptionsType _optionsMode);
    /// @}

    /** @{
     *  @brief get/set the texture filtering
     */
    FilteringType& getDiffuseTextureFiltering();
    const FilteringType& getDiffuseTextureFiltering() const;
    void setDiffuseTextureFiltering(FilteringType _diffuseTextureFiltering);
    /// @}

    /** @{
     *  @brief get/set the texture wrapping
     */
    WrappingType& getDiffuseTextureWrapping();
    const WrappingType& getDiffuseTextureWrapping() const;
    void setDiffuseTextureWrapping(WrappingType _diffuseTextureWrapping);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a texture is added
    typedef core::com::Signal<void (Image::sptr)> AddedTextureSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_ADDED_TEXTURE_SIG;

    /// Type of signal when a texture is removed
    typedef core::com::Signal<void (Image::sptr)> RemovedTextureSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_REMOVED_TEXTURE_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Material& other) const noexcept;
    DATA_API bool operator!=(const Material& other) const noexcept;
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

protected:

    /// Shading mode (flat, Phong)
    ShadingType m_shadingMode {PHONG};

    /// Fill mode (flat, wireframe, point)
    RepresentationType m_representationMode {SURFACE};

    /// Options mode (display normals or not)
    OptionsType m_optionsMode {STANDARD};

    /// Ambient color
    Color::sptr m_ambient;

    /// Diffuse color
    Color::sptr m_diffuse;

    /// Diffuse texture
    Image::sptr m_diffuseTexture;

    /// Filtering for diffuse texture
    FilteringType m_diffuseTextureFiltering {NEAREST};

    /// Wrapping for diffuse texture
    WrappingType m_diffuseTextureWrapping {REPEAT};
};

//-----------------------------------------------------------------------------

inline Material::ShadingType& Material::getShadingMode()
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline const Material::ShadingType& Material::getShadingMode() const
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline void Material::setShadingMode(ShadingType _shadingMode)
{
    this->m_shadingMode = _shadingMode;
}

//-----------------------------------------------------------------------------

inline Material::RepresentationType& Material::getRepresentationMode()
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline const Material::RepresentationType& Material::getRepresentationMode() const
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline void Material::setRepresentationMode(RepresentationType _representationMode)
{
    this->m_representationMode = _representationMode;
}

//-----------------------------------------------------------------------------

inline Material::OptionsType& Material::getOptionsMode()
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline const Material::OptionsType& Material::getOptionsMode() const
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline void Material::setOptionsMode(OptionsType _optionsMode)
{
    this->m_optionsMode = _optionsMode;
}

//-----------------------------------------------------------------------------

inline Material::FilteringType& Material::getDiffuseTextureFiltering()
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline const Material::FilteringType& Material::getDiffuseTextureFiltering() const
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline void Material::setDiffuseTextureFiltering(FilteringType _diffuseTextureFiltering)
{
    this->m_diffuseTextureFiltering = _diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline Material::WrappingType& Material::getDiffuseTextureWrapping()
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline const Material::WrappingType& Material::getDiffuseTextureWrapping() const
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline void Material::setDiffuseTextureWrapping(WrappingType _diffuseTextureWrapping)
{
    this->m_diffuseTextureWrapping = _diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

} //namespace sight::data
