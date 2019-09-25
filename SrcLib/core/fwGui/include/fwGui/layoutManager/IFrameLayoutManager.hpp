/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwData/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <boost/filesystem/path.hpp>

#include <functional>
#include <list>

namespace fwGui
{
namespace layoutManager
{

/**
 * @brief   Defines an interface for managing a frame.
 */
class FWGUI_CLASS_API IFrameLayoutManager : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassMacro(IFrameLayoutManager, ::fwGui::GuiBaseObject);

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Defines all possible style for a frame
    typedef enum
    {
        DEFAULT,
        STAY_ON_TOP,
        MODAL
    } Style;

    typedef std::string RegistryKeyType;

    static const std::string SOFTWARE_UI;
    static const std::string FRAME_STATE_UI;
    static const std::string FRAME_SIZE_W_UI;
    static const std::string FRAME_SIZE_H_UI;
    static const std::string FRAME_POSITION_X_UI;
    static const std::string FRAME_POSITION_Y_UI;

    typedef enum
    {
        UNKNOWN,     ///< the unknown state
        ICONIZED,    ///< the minimized state
        MAXIMIZED,   ///< the maximied state
        FULL_SCREEN  ///< the full screen state
    } FrameState;

    class FrameInfo
    {
    public:

        FrameInfo() :
            m_name(""),
            m_minSize(std::make_pair(-1, -1)),
            m_style(DEFAULT),
            m_size(std::make_pair(-1, -1)),
            m_position(std::make_pair(-1, -1)),
            m_state(UNKNOWN),
            m_visibility(true)
        {
        }

        /// Frame name.
        std::string m_name;
        /// Frame icon.
        ::boost::filesystem::path m_iconPath;
        /// Frame minimum size (min width and min height)
        std::pair< int, int >            m_minSize;
        /// Frame style
        Style m_style;
        /// Frame size
        std::pair< int, int >            m_size;
        /// Frame position
        std::pair< int, int >            m_position;
        /// Frame state (maximize, minized, full screen)
        FrameState m_state;
        /// Frame visibility
        bool m_visibility;
    };

    /// Constructor. Do nothing.
    FWGUI_API IFrameLayoutManager();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IFrameLayoutManager();

    /**
     * @brief Configure the layout before creation.
     */
    FWGUI_API virtual void initialize( ConfigurationType configuration);

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

    /**
     * @brief Instantiate frame.
     */
    FWGUI_API virtual void createFrame() = 0;

    /**
     * @brief Destroy frame.
     */
    FWGUI_API virtual void destroyFrame() = 0;

    /// Return the frame container
    virtual ::fwGui::container::fwContainer::sptr getFrame()
    {
        return m_frame;
    }

    /// Return the first container into the frame
    virtual ::fwGui::container::fwContainer::sptr getContainer()
    {
        return m_container;
    }

    typedef std::function< void () > CloseCallback;
    FWGUI_API virtual void setCloseCallback(CloseCallback fct);

protected:

    /**
     * @brief Configuration definition.
     * @{ */
    const FrameInfo& getFrameInfo() const
    {
        return m_frameInfo;
    }

    //------------------------------------------------------------------------------

    FrameInfo& getFrameInfo()
    {
        return m_frameInfo;
    }

    //------------------------------------------------------------------------------

    void setFrameInfo(const FrameInfo& frameInfo)
    {
        m_frameInfo = frameInfo;
    }
    /**  @} */

    ::fwGui::container::fwContainer::sptr m_frame;
    ::fwGui::container::fwContainer::sptr m_container;
    CloseCallback m_closeCallback;

    FWGUI_API void readConfig();

    FWGUI_API void writeConfig();

    FWGUI_API ::fwData::Composite::sptr getPreferenceUI();

private:

    void defaultCloseCallback();

    /// Save frame configuration definition
    FrameInfo m_frameInfo;

};

} // namespace layoutManager
} // namespace fwGui
