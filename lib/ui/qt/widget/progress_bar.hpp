/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <sight/ui/qt/config.hpp>

#include <core/progress/monitor.hpp>

#include <QLabel>
#include <QLayout>
#include <QPointer>
#include <QProgressBar>
#include <QSvgWidget>
#include <QToolButton>

namespace sight::ui::qt::widget
{

/**
 * @brief Progress bar widget for displaying the progress of a task using monitors.
 * This widget can display multiple tasks at once, showing the title of the current task,
 * a progress bar (or a pulsing SVG animation), and a cancel button if the task supports cancellation.
 *
 * When all tasks are finished, an optional callback can be invoked.
 */
class SIGHT_UI_QT_CLASS_API_QT progress_bar final : public std::enable_shared_from_this<progress_bar>
{
public:

    using finished_callback_t = std::function<void ()>;

    /**
     * @brief progress_bar widget constructor.
     *
     * @param _parent : the parent widget that will contain tickmarks widget.
     * @param _show_title : if true, show the title of the current task.
     * @param _show_cancel : if true, show the cancel button of the current task.
     * @param _pulse : if true, the progress bar will be in pulse mode.
     * @param _expanding : if true, the progress bar will expand to fill available space.
     * @param _svg_path : if valid, will display an svg for pulse mode.
     * @param _svg_size : the default size of the svg.
     * @param _finished_callback : callback called when all tasks are finished.
     */
    SIGHT_UI_QT_API_QT explicit progress_bar(
        QWidget* _parent,
        const std::optional<bool>& _show_title                = std::nullopt,
        const std::optional<bool>& _show_cancel               = std::nullopt,
        const std::optional<bool>& _pulse                     = std::nullopt,
        const std::optional<bool>& _expanding                 = std::nullopt,
        const std::optional<std::filesystem::path>& _svg_path = std::nullopt,
        const std::optional<int>& _svg_size                   = std::nullopt,
        finished_callback_t _finished_callback                = nullptr
    );

    /**
     * @brief Destructor for the progress_bar widget.
     *
     * Everything is cleared and all widgets destroyed. The _monitors themselves are not cancelled or stopped.
     */
    SIGHT_UI_QT_API_QT virtual ~progress_bar();

    /**
     * @brief Add a _monitor to the progress bar.
     *
     * @param __monitor : the _monitor to add.
     */
    SIGHT_UI_QT_API_QT void add_monitor(core::progress::monitor::sptr __monitor);

    /**
     * @brief Get the underlying QWidget that contains the progress bar.
     *
     * @return QWidget* : the underlying QWidget.
     */
    inline QWidget* widget() const;

    /**
     * @brief Set the callback to be called when all _monitors are deleted.
     *
     * @param _callback : the callback function.
     */
    inline void set_finished_callback(finished_callback_t _callback);

    /**
     * @brief Check if all _monitors are finished.
     *
     * @return true if all _monitors are finished, false otherwise.
     */
    inline bool is_finished() const;

private:

    void update_widgets();

    /// Show the title of the current _monitor if true
    std::optional<bool> m_show_title {true};

    /// Show the cancel button of the current _monitor if true
    std::optional<bool> m_show_cancel {true};

    /// True for pulse mode
    std::optional<bool> m_pulse {false};

    /// True for pulse mode
    std::optional<bool> m_expanding {false};

    /// If path is valid, will display an svg for pulse mode
    std::optional<std::filesystem::path> m_svg_path;

    /// The default size of the svg
    std::optional<int> m_svg_size;

    /// UI elements
    QPointer<QWidget> m_container;
    QPointer<QProgressBar> m_progress_bar;
    QPointer<QSvgWidget> m_svg_widget;
    QPointer<QToolButton> m_cancel_button;

    /// Protect the _monitors list
    mutable std::recursive_mutex m_mutex;

    /// List of current _monitors being displayed
    std::vector<core::progress::monitor::wptr> m_progress_monitors;

    /// Callback called when all _monitors are finished
    finished_callback_t m_finished_callback;
};

//------------------------------------------------------------------------------

inline QWidget* progress_bar::widget() const
{
    return m_container;
}

//------------------------------------------------------------------------------

inline void progress_bar::set_finished_callback(finished_callback_t _callback)
{
    std::lock_guard lock(m_mutex);
    m_finished_callback = _callback;
}

//------------------------------------------------------------------------------

inline bool progress_bar::is_finished() const
{
    std::lock_guard lock(m_mutex);

    return m_progress_monitors.empty() || std::ranges::all_of(
        m_progress_monitors,
        [](const auto& _monitor)
        {
            return _monitor.expired() || _monitor.lock()->get_state() >= core::progress::monitor::state::canceled;
        });
}

} //namespace sight::ui::qt::widget
