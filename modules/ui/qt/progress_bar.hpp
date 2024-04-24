/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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
#include <core/jobs/base.hpp>

#include <ui/__/editor.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QObject>
#include <QPointer>
#include <QProgressBar>
#include <QSvgWidget>
#include <QToolButton>

namespace sight::module::ui::qt
{

/**
 * @brief Service displaying a progress bar.
 *
 * @section Signals Signals
 * - \b started(core::jobs::base::wptr _job) : Emitted when a job is started
 * - \b ended(core::jobs::base::wptr _job) : Emitted when a job is canceled or finished
 *
 * @section Slots Slots
 * - \b show_job(core::jobs::base::sptr _job): visualize the progression of jobs.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::progress_bar">
        <config show_title="false" show_cancel="false" svg="path/to/svg" svg_size="48" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b show_title : Show the title of the current job if true (default: true)
 * - \b show_cancel : Show the cancel button of the current job if true (default: true)
 * - \b svg : If path is valid, will display an svg for pulse mode
 * - \b svg_size : The default size of the svg. If not set, the svg will be displayed at its original size
 */

class progress_bar : public QObject,
                     public sight::ui::editor
{
Q_OBJECT

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

    struct signals final
    {
        using key_t = sight::core::com::signals::key_t;
        static inline const key_t STARTED = "started";
        static inline const key_t ENDED   = "ended";

        using void_job_signal_t = sight::core::com::signal<void (core::jobs::base::wptr _job)>;
    };

    struct slots final
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t SHOW_JOB = "show_job";
    };

    /**
     * @brief Update widgets visibility. This method is called by the job hooks.
     *
     * @note It need to be public because we hold a weak pointer.
     * @warning This method is not thread safe and must be called on main thread.
     */
    void update_widgets(core::jobs::base::wptr _job_to_remove = core::jobs::base::wptr());

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
     * @brief show_job slot's method.
     */
    void show_job(core::jobs::base::sptr _job);

private:

    /// Show the title of the current job if true
    bool m_show_title {true};

    /// Show the cancel button of the current job if true
    bool m_show_cancel {true};

    /// True for pulse mode
    bool m_pulse {false};

    /// If path is valid, will display an svg for pulse mode
    std::filesystem::path m_svg_path;

    /// The default size of the svg
    std::optional<int> m_svg_size;

    QPointer<QHBoxLayout> m_layout;
    QPointer<QLabel> m_title;
    QPointer<QProgressBar> m_progress_bar;
    QPointer<QSvgWidget> m_svg_widget;
    QPointer<QToolButton> m_cancel_button;

    std::vector<core::jobs::base::wptr> m_jobs;
};

} // namespace sight::module::ui::qt
