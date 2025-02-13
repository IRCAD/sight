/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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
#include "data/image.hpp"

namespace sight::data
{

/**
 * @brief This class defines a material. A material is represented by an ambient color and a diffuse color.
 * @see Color
 */
class SIGHT_DATA_CLASS_API material final : public object
{
public:

    SIGHT_DECLARE_CLASS(material, object);

    struct defaults
    {
        static inline const sight::data::color AMBIENT = {0.8F, 0.8F, 0.8F, 1.0F};
        static inline const sight::data::color DIFFUSE = {1.0F, 1.0F, 1.0F, 1.0F};
    };

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API material();

    /// Destructor
    SIGHT_DATA_API ~material() noexcept override = default;

    /**
     * @brief returns editable ambient color
     */
    SIGHT_DATA_API color::sptr ambient() const;

    /**
     * @brief Setter for ambient color
     *
     * @param _ambient color
     */
    SIGHT_DATA_API void set_ambient(const color::sptr& _ambient);

    /**
     * @brief returns editable diffuse color
     */
    SIGHT_DATA_API color::sptr diffuse() const;

    /**
     * @brief Setter for diffuse color
     *
     * @param _diffuse color
     */
    SIGHT_DATA_API void set_diffuse(const color::sptr& _diffuse);

    /**
     * @brief returns editable diffuse texture
     * @{
     */
    SIGHT_DATA_API image::sptr get_diffuse_texture();
    SIGHT_DATA_API image::csptr get_diffuse_texture() const;
    ///@}

    /**
     * @brief Setter for diffuse texture
     *
     * @param _diffuse_texture texture
     */
    SIGHT_DATA_API void set_diffuse_texture(const image::sptr& _diffuse_texture);

    /**
     * @brief Options
     */
    enum options_t
    {
        standard      = 1,
        normals       = 2,
        cells_normals = 3
    };

    /**
     * @brief shading models
     */
    enum class shading_t : int
    {
        ambient = 0,
        flat    = 1,
        phong   = 2
    };

    /**
     * @brief Representation models
     */
    enum representation_t
    {
        surface   = 1, //1
        point     = 2, //SURFACE<<1 -> 10
        wireframe = 4, //SURFACE<<2 -> 100
        edge      = 5  //SURFACE|WIREFRAME -> 101
    };

    /**
     * @brief texture filtering types
     */
    enum filtering_t
    {
        nearest,
        linear
    };

    /**
     * @brief texture wrapping types
     */
    enum wrapping_t
    {
        clamp,
        repeat
    };

    /** @{
     *  @brief get/set the shading models(flat, phong)
     */
    shading_t& get_shading_mode();
    const shading_t& get_shading_mode() const;
    void set_shading_mode(shading_t _shading_mode);
    /// @}

    /** @{
     *  @brief get/set the representation models (edge, point, wireframe, surface)
     */
    representation_t& get_representation_mode();
    const representation_t& get_representation_mode() const;
    void set_representation_mode(representation_t _representation_mode);
    /// @}

    /** @{
     *  @brief get/set the option representation (Standard, normals)
     */
    options_t& get_options_mode();
    const options_t& get_options_mode() const;
    void set_options_mode(options_t _options_mode);
    /// @}

    /** @{
     *  @brief get/set the texture filtering
     */
    filtering_t& get_diffuse_texture_filtering();
    const filtering_t& get_diffuse_texture_filtering() const;
    void set_diffuse_texture_filtering(filtering_t _diffuse_texture_filtering);
    /// @}

    /** @{
     *  @brief get/set the texture wrapping
     */
    wrapping_t& get_diffuse_texture_wrapping();
    const wrapping_t& get_diffuse_texture_wrapping() const;
    void set_diffuse_texture_wrapping(wrapping_t _diffuse_texture_wrapping);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when a texture is added
    using added_texture_signal_t = core::com::signal<void (image::sptr)>;
    SIGHT_DATA_API static const core::com::signals::key_t ADDED_TEXTURE_SIG;

    /// Type of signal when a texture is removed
    using removed_texture_signal_t = core::com::signal<void (image::sptr)>;
    SIGHT_DATA_API static const core::com::signals::key_t REMOVED_TEXTURE_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const material& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const material& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    // Unified ways to convert representations from string for service definitions
    SIGHT_DATA_API static representation_t string_to_representation_mode(std::string _str);

protected:

    /// shading mode (flat, Phong)
    shading_t m_shading_mode {shading_t::phong};

    /// Fill mode (flat, wireframe, point)
    representation_t m_representation_mode {surface};

    /// Options mode (display normals or not)
    options_t m_options_mode {standard};

    /// Ambient color
    color::sptr m_ambient {std::make_shared<data::color>(defaults::AMBIENT)};

    /// Diffuse color
    color::sptr m_diffuse {std::make_shared<data::color>(defaults::DIFFUSE)};

    /// Diffuse texture
    image::sptr m_diffuse_texture;

    /// Filtering for diffuse texture
    filtering_t m_diffuse_texture_filtering {nearest};

    /// Wrapping for diffuse texture
    wrapping_t m_diffuse_texture_wrapping {repeat};
};

//-----------------------------------------------------------------------------

inline material::shading_t& material::get_shading_mode()
{
    return this->m_shading_mode;
}

//-----------------------------------------------------------------------------

inline const material::shading_t& material::get_shading_mode() const
{
    return this->m_shading_mode;
}

//-----------------------------------------------------------------------------

inline void material::set_shading_mode(shading_t _shading_mode)
{
    this->m_shading_mode = _shading_mode;
}

//-----------------------------------------------------------------------------

inline material::representation_t& material::get_representation_mode()
{
    return this->m_representation_mode;
}

//-----------------------------------------------------------------------------

inline const material::representation_t& material::get_representation_mode() const
{
    return this->m_representation_mode;
}

//-----------------------------------------------------------------------------

inline void material::set_representation_mode(representation_t _representation_mode)
{
    this->m_representation_mode = _representation_mode;
}

//-----------------------------------------------------------------------------

inline material::options_t& material::get_options_mode()
{
    return this->m_options_mode;
}

//-----------------------------------------------------------------------------

inline const material::options_t& material::get_options_mode() const
{
    return this->m_options_mode;
}

//-----------------------------------------------------------------------------

inline void material::set_options_mode(options_t _options_mode)
{
    this->m_options_mode = _options_mode;
}

//-----------------------------------------------------------------------------

inline material::filtering_t& material::get_diffuse_texture_filtering()
{
    return this->m_diffuse_texture_filtering;
}

//-----------------------------------------------------------------------------

inline const material::filtering_t& material::get_diffuse_texture_filtering() const
{
    return this->m_diffuse_texture_filtering;
}

//-----------------------------------------------------------------------------

inline void material::set_diffuse_texture_filtering(filtering_t _diffuse_texture_filtering)
{
    this->m_diffuse_texture_filtering = _diffuse_texture_filtering;
}

//-----------------------------------------------------------------------------

inline material::wrapping_t& material::get_diffuse_texture_wrapping()
{
    return this->m_diffuse_texture_wrapping;
}

//-----------------------------------------------------------------------------

inline const material::wrapping_t& material::get_diffuse_texture_wrapping() const
{
    return this->m_diffuse_texture_wrapping;
}

//-----------------------------------------------------------------------------

inline void material::set_diffuse_texture_wrapping(wrapping_t _diffuse_texture_wrapping)
{
    this->m_diffuse_texture_wrapping = _diffuse_texture_wrapping;
}

//-----------------------------------------------------------------------------

} //namespace sight::data
