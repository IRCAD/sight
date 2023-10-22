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
    DATA_API void setAmbient(const color::sptr& _ambient);

    /**
     * @brief returns editable diffuse color
     */
    DATA_API color::sptr diffuse() const;

    /**
     * @brief Setter for diffuse color
     *
     * @param diffuse color
     */
    DATA_API void setDiffuse(const color::sptr& _diffuse);

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
    DATA_API void setDiffuseTexture(const image::sptr& _diffuse_texture);

    /**
     * @brief Options
     */
    typedef enum
    {
        STANDARD      = 1,
        NORMALS       = 2,
        CELLS_NORMALS = 3
    } options_t;

    /**
     * @brief shading models
     */
    typedef enum
    {
        AMBIENT = 0,
        FLAT    = 1,
        PHONG   = 2
    } shading_t;

    /**
     * @brief Representation models
     */
    typedef enum
    {
        SURFACE   = 1, //1
        POINT     = 2, //SURFACE<<1 -> 10
        WIREFRAME = 4, //SURFACE<<2 -> 100
        EDGE      = 5  //SURFACE|WIREFRAME -> 101
    } representation_t;

    /**
     * @brief texture filtering types
     */
    typedef enum
    {
        NEAREST,
        LINEAR
    } filtering_t;

    /**
     * @brief texture wrapping types
     */
    typedef enum
    {
        CLAMP,
        REPEAT
    } wrapping_t;

    /** @{
     *  @brief get/set the shading models(flat, phong)
     */
    shading_t& getShadingMode();
    const shading_t& getShadingMode() const;
    void setShadingMode(shading_t _shading_mode);
    /// @}

    /** @{
     *  @brief get/set the representation models (edge, point, wireframe, surface)
     */
    representation_t& getRepresentationMode();
    const representation_t& getRepresentationMode() const;
    void setRepresentationMode(representation_t _representation_mode);
    /// @}

    /** @{
     *  @brief get/set the option representation (Standard, normals)
     */
    options_t& getOptionsMode();
    const options_t& getOptionsMode() const;
    void setOptionsMode(options_t _options_mode);
    /// @}

    /** @{
     *  @brief get/set the texture filtering
     */
    filtering_t& getDiffuseTextureFiltering();
    const filtering_t& getDiffuseTextureFiltering() const;
    void setDiffuseTextureFiltering(filtering_t _diffuse_texture_filtering);
    /// @}

    /** @{
     *  @brief get/set the texture wrapping
     */
    wrapping_t& getDiffuseTextureWrapping();
    const wrapping_t& getDiffuseTextureWrapping() const;
    void setDiffuseTextureWrapping(wrapping_t _diffuse_texture_wrapping);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a texture is added
    typedef core::com::signal<void (image::sptr)> added_texture_signal_t;
    DATA_API static const core::com::signals::key_t ADDED_TEXTURE_SIG;

    /// Type of signal when a texture is removed
    typedef core::com::signal<void (image::sptr)> removed_texture_signal_t;
    DATA_API static const core::com::signals::key_t REMOVED_TEXTURE_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const material& _other) const noexcept;
    DATA_API bool operator!=(const material& _other) const noexcept;
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

protected:

    /// shading mode (flat, Phong)
    shading_t m_shadingMode {PHONG};

    /// Fill mode (flat, wireframe, point)
    representation_t m_representationMode {SURFACE};

    /// Options mode (display normals or not)
    options_t m_optionsMode {STANDARD};

    /// Ambient color
    color::sptr m_ambient;

    /// Diffuse color
    color::sptr m_diffuse;

    /// Diffuse texture
    image::sptr m_diffuseTexture;

    /// Filtering for diffuse texture
    filtering_t m_diffuseTextureFiltering {NEAREST};

    /// Wrapping for diffuse texture
    wrapping_t m_diffuseTextureWrapping {REPEAT};
};

//-----------------------------------------------------------------------------

inline material::shading_t& material::getShadingMode()
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline const material::shading_t& material::getShadingMode() const
{
    return this->m_shadingMode;
}

//-----------------------------------------------------------------------------

inline void material::setShadingMode(shading_t _shading_mode)
{
    this->m_shadingMode = _shading_mode;
}

//-----------------------------------------------------------------------------

inline material::representation_t& material::getRepresentationMode()
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline const material::representation_t& material::getRepresentationMode() const
{
    return this->m_representationMode;
}

//-----------------------------------------------------------------------------

inline void material::setRepresentationMode(representation_t _representation_mode)
{
    this->m_representationMode = _representation_mode;
}

//-----------------------------------------------------------------------------

inline material::options_t& material::getOptionsMode()
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline const material::options_t& material::getOptionsMode() const
{
    return this->m_optionsMode;
}

//-----------------------------------------------------------------------------

inline void material::setOptionsMode(options_t _options_mode)
{
    this->m_optionsMode = _options_mode;
}

//-----------------------------------------------------------------------------

inline material::filtering_t& material::getDiffuseTextureFiltering()
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline const material::filtering_t& material::getDiffuseTextureFiltering() const
{
    return this->m_diffuseTextureFiltering;
}

//-----------------------------------------------------------------------------

inline void material::setDiffuseTextureFiltering(filtering_t _diffuse_texture_filtering)
{
    this->m_diffuseTextureFiltering = _diffuse_texture_filtering;
}

//-----------------------------------------------------------------------------

inline material::wrapping_t& material::getDiffuseTextureWrapping()
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline const material::wrapping_t& material::getDiffuseTextureWrapping() const
{
    return this->m_diffuseTextureWrapping;
}

//-----------------------------------------------------------------------------

inline void material::setDiffuseTextureWrapping(wrapping_t _diffuse_texture_wrapping)
{
    this->m_diffuseTextureWrapping = _diffuse_texture_wrapping;
}

//-----------------------------------------------------------------------------

} //namespace sight::data
