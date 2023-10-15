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

#include "modules/ui/qt/config.hpp"

#include <core/tools/failed.hpp>

#include <data/image.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace sight::module::ui::qt::metrics
{

/**
 * @brief   Distance service is represented by a button. It allows to show distances in a generic scene.
 *
 * @note Sends a signal to request a distance. It should be connected to an ImageMultiDistance adaptor to create the
 * distance.
 *
 * @section Signals Signals
 *  \b distanceRequested() : signal to request a distance. It should be connected to an ImageMultiDistance
 * adaptor to create the distance.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::metrics::Distance" >
       <inout key="image" uid="..." />
   </service>
   @endcode

 * @subsection In-Out In-Out
 * -\b image [data::image]: image in which we calculate the distance.
 */
class MODULE_UI_QT_CLASS_API Distance : public QObject,
                                        public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(Distance, sight::ui::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API Distance() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~Distance() noexcept override;

    /**
     * @name Signal
     * @{
     */
    MODULE_UI_QT_API static const core::com::signals::key_t DISTANCE_REQUESTED_SIG;
    typedef core::com::signal<void ()> DistanceRequestedSignalType;
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
    void onDistanceButton();

private:

    QPointer<QPushButton> m_distButton;

    DistanceRequestedSignalType::sptr m_sigDistanceRequested; /// signal emitted when a distance is requested

    data::ptr<data::image, data::Access::inout> m_image {this, "image"};
};

} // namespace sight::module::ui::qt::metrics
