/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/com/slots.hpp>
#include <core/progress/monitor.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/widget/progress_bar.hpp>

namespace sight::module::ui::qt
{

/**
 * @brief Service displaying a progress bar.
 *
 * @section Slots Slots
 * - \b add_monitor(core::progress::monitor::sptr _monitor): visualize the progression of tasks.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::progress_bar">
        <config show_title="false" show_cancel="false" svg="path/to/svg" svg_size="48" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b show_title : Show the title of the current task if true (default: true)
 * - \b show_cancel : Show the cancel button of the current task if true (default: true)
 * - \b svg : If path is valid, will display an svg for pulse mode
 * - \b svg_size : The default size of the svg. If not set, the svg will be displayed at its original size
 */

class progress_bar : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(progress_bar, sight::ui::editor);

    /**
     * @brief Constructor. Do nothing.
     */
    progress_bar() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    ~progress_bar() noexcept override = default;

    struct slots final
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t ADD_MONITOR = "add_monitor";
    };

    struct signals final
    {
        using finished_t = core::com::signal<void ()>;
        using key_t      = sight::core::com::signals::key_t;
        static inline const key_t FINISHED = "finished";
    };

protected:

    /**
     * @brief Do nothing.
     */
    void updating() override;

    /**
     * @brief initialize the service.
     */
    void configuring() override;

    /**
     * @brief Create the related widgets for the progress bar.
     */
    void starting() override;

    /**
     * @brief Destroy the layout.
     */
    void stopping() override;

    /**
     * @brief add_monitor slot's method.
     */
    void add_monitor(core::progress::monitor::sptr _monitor);

private:

    /// The progress bar widget. Use a shared ptr to be able to pass it to a lambda function.
    std::shared_ptr<sight::ui::qt::widget::progress_bar> m_progress_bar_widget;
};

} // namespace sight::module::ui::qt
