/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include "core/com/has_signals.hpp"

#include "viz/scene3d/factory/new.hpp"

#include <core/base_object.hpp>

#include <viz/scene3d/render.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreMovableObject.h>

namespace sight::viz::scene3d
{

/**
 * @brief Class used to display overlay text.
 *
 * This class inherits from 'Ogre::MovableObject' so it can be attached to 3D object and follow them around.
 * It can also be displayed in 2D if not attached to anything and it's position can be set and updated through
 * the 'setPosition' method.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API text : public sight::core::base_object,
                                         public core::com::has_signals
{
public:

    SIGHT_DECLARE_CLASS(text, sight::core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Class used to register a class factory in factory registry.
    template<typename T>
    class registry
    {
    public:

        registry(std::string _functor_key)
        {
            auto fact = [](const sight::viz::scene3d::layer::sptr& _layer) -> std::shared_ptr<T>
                        {
                            // Capture the factory inside a lambda to distinguish it from overloaded methods.
                            return viz::scene3d::text_factory::make<T>(_layer);
                        };
            viz::scene3d::registry::get_text_registry()->add_factory(_functor_key, fact);
        }
    };

    //------------------------------------------------------------------------------

    static viz::scene3d::text::sptr make(const sight::viz::scene3d::layer::sptr& _layer)
    {
        return viz::scene3d::text_factory::make(viz::scene3d::text::REGISTRY_KEY, _layer);
    }

    /// Defines the key used for the factory.
    SIGHT_VIZ_SCENE3D_API static inline const std::string REGISTRY_KEY {"sight::viz::scene3d::text::REGISTRY_KEY"};

    static const inline std::string TEXT_EDITED_SIGNAL = "textEdited";
    using text_edited_signal_t = core::com::signal<void (std::string)>;

    static const inline std::string EDITING_FINISHED_SIGNAL = "editingFinished";
    using editing_finished_signal_t = core::com::signal<void ()>;

    text()
    {
        new_signal<text_edited_signal_t>(TEXT_EDITED_SIGNAL);
        new_signal<editing_finished_signal_t>(EDITING_FINISHED_SIGNAL);
    }

    /// Attach to a scene node.
    SIGHT_VIZ_SCENE3D_API virtual void attach_to_node(Ogre::SceneNode* _node, Ogre::Camera* _camera) = 0;

    /// Detach the text from a scene node
    SIGHT_VIZ_SCENE3D_API virtual void detach_from_node() = 0;

    /// Displayed text.
    SIGHT_VIZ_SCENE3D_API virtual void set_text(const std::string& _text) = 0;

    /// Position in screen coordinates.
    SIGHT_VIZ_SCENE3D_API virtual void set_position(float _x, float _y) = 0;

    /// Text color, white by default.
    SIGHT_VIZ_SCENE3D_API virtual void set_text_color(const Ogre::ColourValue& _color) = 0;

    /// Text color, white by default.
    SIGHT_VIZ_SCENE3D_API virtual void set_text_color(const std::string& _color) = 0;

    /// Set the visibility of the text.
    SIGHT_VIZ_SCENE3D_API virtual void set_visible(bool _visible) = 0;

    /// Aligns the text by setting the x and y coordinates' origin.
    SIGHT_VIZ_SCENE3D_API virtual void set_text_alignment(const std::string& _h_align, const std::string& _valign) = 0;

    /// Sets the font to be used for rendering.
    SIGHT_VIZ_SCENE3D_API virtual void set_font(const std::string& _font) = 0;

    /// Sets the font size to be used for rendering.
    SIGHT_VIZ_SCENE3D_API virtual void set_font_size(std::size_t _size) = 0;

    /// Sets the edit mode. If true, the label can be modified, else it is read-only.
    SIGHT_VIZ_SCENE3D_API virtual void set_edit_mode(bool _edit_mode) = 0;
};

} // namespace sight::viz::scene3d
