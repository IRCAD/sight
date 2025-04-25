/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/tools/failed.hpp>

#include <data/image.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace sight::module::ui::qt::metrics
{

/**
 * @brief   distance service is represented by a button. It allows to show distances in a generic scene.
 *
 * @note Sends a signal to request a distance. It should be connected to an ImageMultidistance adaptor to create the
 * distance.
 *
 * @section Signals Signals
 *  \b distance_requested() : signal to request a distance. It should be connected to an ImageMultidistance
 * adaptor to create the distance.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::metrics::distance" >
       <inout key="image" uid="..." />
   </service>
   @endcode

 * @subsection In-Out In-Out
 * -\b image [data::image]: image in which we calculate the distance.
 */
class distance : public QObject,
                 public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(distance, sight::ui::editor);

    /// Constructor. Do nothing.
    distance() noexcept;

    /// Destructor. Do nothing.
    ~distance() noexcept override;

    /**
     * @name Signal
     * @{
     */
    static const core::com::signals::key_t DISTANCE_REQUESTED_SIG;
    using distance_requested_signal_t = core::com::signal<void ()>;
/**
 * @}
 */

protected:

    /**
     * @brief Install the layout.
     *
     * This method launches the editor::starting method.
     */
    void starting() override;

    /**
     * @brief Destroy the layout.
     *
     * This method launches the editor::stopping method.
     */
    void stopping() override;

    /// Do nothing
    void updating() override;

    /// Initialize the widget
    void configuring() override;

protected Q_SLOTS:

    /**
     * @brief Notify the scene that a distance is added.
     */
    void on_distance_button();

private:

    QPointer<QPushButton> m_dist_button;

    distance_requested_signal_t::sptr m_sig_distance_requested; /// signal emitted when a distance is requested

    data::ptr<data::image, data::access::inout> m_image {this, "image"};
};

} // namespace sight::module::ui::qt::metrics
