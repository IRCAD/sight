/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <filesystem>
#include <functional>
#include <list>

namespace sight::ui::base::layoutManager
{

/**
 * @brief   Defines an interface for managing a frame.
 */
class UI_BASE_CLASS_API IFrameLayoutManager : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IFrameLayoutManager, ui::base::GuiBaseObject);

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

    enum class FrameState : std::uint8_t
    {
        UNKNOWN     = 0, ///< the unknown state
        ICONIZED    = 1, ///< the minimized state
        MAXIMIZED   = 2, ///< the maximied state
        FULL_SCREEN = 3  ///< the full screen state
    };

    class FrameInfo
    {
    public:

        FrameInfo() :

            m_minSize(std::make_pair(-1, -1)),
            m_size(std::make_pair(-1, -1)),
            m_position(std::make_pair(-1, -1))
        {
        }

        /// Frame name.
        std::string m_name;

        /// Optional version number, displayed in the title if specified.
        std::string m_version;

        /// Frame icon.
        std::filesystem::path m_iconPath;

        /// Frame minimum size (min width and min height)
        std::pair<int, int> m_minSize;

        /// Frame maximum size (max width and max height)
        std::pair<int, int> m_maxSize {-1, -1};

        /// Frame style
        Style m_style {DEFAULT};

        /// Frame size
        std::pair<int, int> m_size;

        /// Frame position
        std::pair<int, int> m_position;

        /// Frame state (maximize, minized, full screen)
        FrameState m_state {FrameState::UNKNOWN};

        /// Frame visibility
        bool m_visibility {true};
    };

    /// Constructor. Do nothing.
    UI_BASE_API IFrameLayoutManager();

    /// Destructor. Do nothing.
    UI_BASE_API ~IFrameLayoutManager() override = default;

    /**
     * @brief Configure the layout before creation.
     */
    UI_BASE_API virtual void initialize(const ui::base::config_t& configuration);

    UI_BASE_API static const RegistryKeyType REGISTRY_KEY;

    /**
     * @brief Instantiate frame.
     */
    UI_BASE_API virtual void createFrame() = 0;

    /**
     * @brief Destroy frame.
     */
    UI_BASE_API virtual void destroyFrame() = 0;

    /// Return the frame container
    virtual ui::base::container::fwContainer::sptr getFrame()
    {
        return m_frame;
    }

    /// Return the first container into the frame
    virtual ui::base::container::fwContainer::sptr getContainer()
    {
        return m_container;
    }

    typedef std::function<void ()> CloseCallback;
    UI_BASE_API virtual void setCloseCallback(CloseCallback fct);

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

    ui::base::container::fwContainer::sptr m_frame;
    ui::base::container::fwContainer::sptr m_container;
    CloseCallback m_closeCallback;

    UI_BASE_API void readConfig();

    UI_BASE_API void writeConfig() const;

private:

    static void defaultCloseCallback();

    /// Save frame configuration definition
    FrameInfo m_frameInfo;
};

} // namespace sight::ui::base::layoutManager
