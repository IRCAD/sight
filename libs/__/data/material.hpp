/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/color.hpp"
#include "data/factory/new.hpp"
#include "data/image.hpp"
#include "data/object.hpp"

namespace sight::data
{

/**
 * @brief This class defines a material. A material is represented by an ambient color and a diffuse color.
 * @see Color
 */
class DATA_CLASS_API material final : public object
{
public:

    SIGHT_DECLARE_CLASS(material, object);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API material();

    /// Destructor
    DATA_API ~material() noexcept override = default;

    /**
     * @brief returns editable ambient color
     */
    DATA_API color::sptr ambient() const;

    /**
     * @brief Setter for ambient color
     *
     * @param ambient color
     */
    DATA_API void setAmbient(const color::sptr& ambient);

    /**
     * @brief returns editable diffuse color
     */
    DATA_API color::sptr diffuse() const;

    /**
     * @brief Setter for diffuse color
     *
     * @param diffuse color
     */
    DATA_API void setDiffuse(const color::sptr& diffuse);

    /**
     * @brief returns editable diffuse texture
     * @{
     */
    DATA_API image::sptr getDiffuseTexture();
    DATA_API image::csptr getDiffuseTexture() const;
    ///@}

    /**
     * @brief Setter for diffuse texture
     *
     * @param diffuseTexture texture
     */
    DATA_API void setDiffuseTexture(const image::sptr& diffuseTexture);

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
    typedef core::com::signal<void (image::sptr)> AddedTextureSignalType;
    DATA_API static const core::com::signals::key_t ADDED_TEXTURE_SIG;

    /// Type of signal when a texture is removed
    typedef core::com::signal<void (image::sptr)> RemovedTextureSignalType;
    DATA_API static const core::com::signals::key_t REMOVED_TEXTURE_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const material& other) const noexcept;
    DATA_API bool operator!=(const material& other) const noexcept;
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

protected:

    /// Shading mode (flat, Phong)
    ShadingType m_shadingMode {PHONG};

    /// Fill mode (flat, wireframe, point)
    RepresentationType m_representationMode {SURFACE};

    /// Options mode (display normals or not)
    OptionsType m_optionsMode {STANDARD};

    /// Ambient color
    color::sptr m_ambient;

    /// Diffuse color
    color::sptr m_diffuse;

    /// Diffuse texture
    image::sptr m_diffuseTexture;

    /// Filtering for diffuse texture
    FilteringType m_diffuseTextureFiltering {NEAREST};

    /// Wrapping for diffuse texture
    WrappingType m_diffuseTextureWrapping {REPEAT};
};

//-----------------------------------------------------------------------------

inline material::ShadingType& material::getShadingMode()
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline const material::ShadingType& material::getShadingMode() const
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline void material::setShadingMode(ShadingType _shadingMode)
{
    this->m_shadingMode = _shadingMode;
}

//-----------------------------------------------------------------------------

inline material::RepresentationType& material::getRepresentationMode()
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline const material::RepresentationType& material::getRepresentationMode() const
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline void material::setRepresentationMode(RepresentationType _representationMode)
{
    this->m_representationMode = _representationMode;
}

//-----------------------------------------------------------------------------

inline material::OptionsType& material::getOptionsMode()
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline const material::OptionsType& material::getOptionsMode() const
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline void material::setOptionsMode(OptionsType _optionsMode)
{
    this->m_optionsMode = _optionsMode;
}

//-----------------------------------------------------------------------------

inline material::FilteringType& material::getDiffuseTextureFiltering()
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline const material::FilteringType& material::getDiffuseTextureFiltering() const
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline void material::setDiffuseTextureFiltering(FilteringType _diffuseTextureFiltering)
{
    this->m_diffuseTextureFiltering = _diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline material::WrappingType& material::getDiffuseTextureWrapping()
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline const material::WrappingType& material::getDiffuseTextureWrapping() const
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline void material::setDiffuseTextureWrapping(WrappingType _diffuseTextureWrapping)
{
    this->m_diffuseTextureWrapping = _diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

} //namespace sight::data
