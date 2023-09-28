/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "core/com/has_signals.hpp"

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/factory/new.hpp"

#include <core/base_object.hpp>

#include <viz/scene3d/SRender.hpp>

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
class VIZ_SCENE3D_CLASS_API IText : public sight::core::base_object,
                                    public core::com::has_signals
{
public:

    SIGHT_DECLARE_CLASS(IText, sight::core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Class used to register a class factory in factory registry.
    template<typename T>
    class Registry
    {
    public:

        Registry(std::string functorKey)
        {
            auto fact = [](const sight::viz::scene3d::Layer::sptr& _layer) -> std::shared_ptr<T>
                        {
                            // Capture the factory inside a lambda to distinguish it from overloaded methods.
                            return viz::scene3d::textFactory::make<T>(_layer);
                        };
            viz::scene3d::registry::getTextRegistry()->add_factory(functorKey, fact);
        }
    };

    //------------------------------------------------------------------------------

    static viz::scene3d::IText::sptr make(const sight::viz::scene3d::Layer::sptr& _layer)
    {
        return viz::scene3d::textFactory::make(viz::scene3d::IText::REGISTRY_KEY, _layer);
    }

    /// Defines the key used for the factory.
    VIZ_SCENE3D_API static inline const std::string REGISTRY_KEY {"sight::viz::scene3d::IText::REGISTRY_KEY"};

    static const inline std::string TEXT_EDITED_SIGNAL = "textEdited";
    using TextEditedSignal = core::com::signal<void (std::string)>;

    static const inline std::string EDITING_FINISHED_SIGNAL = "editingFinished";
    using EditingFinishedSignal = core::com::signal<void ()>;

    IText()
    {
        new_signal<TextEditedSignal>(TEXT_EDITED_SIGNAL);
        new_signal<EditingFinishedSignal>(EDITING_FINISHED_SIGNAL);
    }

    /// Attach to a scene node.
    VIZ_SCENE3D_API virtual void attachToNode(Ogre::SceneNode* _node, Ogre::Camera* _camera) = 0;

    /// Detach the text from a scene node
    VIZ_SCENE3D_API virtual void detachFromNode() = 0;

    /// Displayed text.
    VIZ_SCENE3D_API virtual void setText(const std::string& _text) = 0;

    /// Position in screen coordinates.
    VIZ_SCENE3D_API virtual void setPosition(float _x, float _y) = 0;

    /// Text color, white by default.
    VIZ_SCENE3D_API virtual void setTextColor(const Ogre::ColourValue& _color) = 0;

    /// Text color, white by default.
    VIZ_SCENE3D_API virtual void setTextColor(const std::string& _color) = 0;

    /// Set the visibility of the text.
    VIZ_SCENE3D_API virtual void setVisible(bool _visible) = 0;

    /// Aligns the text by setting the x and y coordinates' origin.
    VIZ_SCENE3D_API virtual void setTextAlignment(const std::string& hAlign, const std::string& valign) = 0;

    /// Sets the font to be used for rendering.
    VIZ_SCENE3D_API virtual void setFont(const std::string& font) = 0;

    /// Sets the font size to be used for rendering.
    VIZ_SCENE3D_API virtual void setFontSize(std::size_t size) = 0;

    /// Sets the edit mode. If true, the label can be modified, else it is read-only.
    VIZ_SCENE3D_API virtual void setEditMode(bool editMode) = 0;
};

} // namespace sight::viz::scene3d
