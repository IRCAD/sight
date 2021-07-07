/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include <ui/base/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QLabel;

namespace sight::module::ui::qt
{

/**
 * @brief Adds an opacity editor widget (Slider) applied on a data::Material.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::SMaterialOpacityEditor">
       <inout key="material" uid="..." />
   </service>
   @endcode
 *
 *@subsection In-Out In-Out
 * - \b material [sight::data::Material]: material object to update.
 */

class MODULE_UI_QT_CLASS_API SMaterialOpacityEditor : public QObject,
                                                      public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SMaterialOpacityEditor, sight::ui::base::IEditor);

    /// Creates the service.
    MODULE_UI_QT_API SMaterialOpacityEditor() noexcept;

    /// Destroys the service.
    MODULE_UI_QT_API ~SMaterialOpacityEditor() noexcept override;

private:

    /// Configures the service.
    void configuring() final;

    /// Sets the connections and the UI elements.
    void starting() final;

    /// Does nothing.
    void updating() final;

    /// Destroys the connections and cleans the container.
    void stopping() final;

    QPointer<QSlider> m_opacitySlider;
    QPointer<QLabel> m_opacityValue;

private Q_SLOTS:

    /**
     * @brief Slot: called when the opacity slider changed.
     * @param _value The new opacity value.
     */
    void onOpacitySlider(int _value);
};

} // namespace sight::module::ui::qt
