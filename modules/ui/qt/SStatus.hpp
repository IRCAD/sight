/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <core/tools/Failed.hpp>

#include <ui/base/IEditor.hpp>

#include <QLabel>
#include <QPointer>

class QPushButton;

namespace sight::module::ui::qt
{

/**
 * @brief   SStatus service shows a colored square (red, orange, green) representing a status.
 *
 * @note To change the status color, you should call the slots 'changeToGreen', * 'changeToOrange', 'changeToRed'
 * or 'toggleGreenRed'.
 *
 * @brief Configures the status tooltip
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::SStatus">
       <form>square|circle</form>
       <count>2</count>
       <layout>horizontal|vertical</layout>
       <size>
           <width>20</width>
           <height>20</height>
       </size>
       <labels>
            <labelStatus>SCP Server</labelStatus>
            <labelStatus>TCP Server</labelStatus>
       </labels>
       <red>Stopped</red>
       <green>Tracking</green>
       <orange>Started</orange>
   </service>
   @endcode
 * - \b form (optional, 'square' by default): the form of the indicator
 * - \b count (optional, '1' by default): the number of status
 * - \b layout(optional, 'horizontal' by default): orientation of the layout
 * - \b size (optional): the size of the indicator
 *   - \b width (optional, 20 by default): the width of the indicator
 *   - \b height (optional, 20 by default): the height of the indicator
 * - \b labels (optional): the description associated to the indicators when count > 1
 *   - \b labelStatus (optional): the description associated to each indicator
 * - \b labelStatus (optional): the description associated to the indicator when count = 1 or is missing
 * - \b red (optional): the description associated to the red status
 * - \b green (optional): the description associated to the green status
 * - \b orange (optional): the description associated to the orange status
 *
 * @section Slots Slots
 * - \b changeToGreen(): This slot allows to change the indicator color to green. If there is more than one status, it
 * changes them all.
 * - \b changeToRed(): This slot allows to change the indicator color to red. If there is more than one status, it
 * changes them all.
 * - \b changeToOrange(): This slot allows to change the indicator color to orange. If there is more than one status, it
 * changes them all.
 * - \b toggleGreenRed(bool): This slot allows to change the indicator color to green or red. If there is more than one,
 * it toggle them all.
 * - \b changeNthToGreen(int): This slot allows to change the indicator color to green for the ith status.
 * - \b changeNthToRed(int): This slot allows to change the indicator color to red for the ith status.
 * - \b changeNthToOrange(int): This slot allows to change the indicator color to orange for the ith status.
 * - \b toggleNthGreenRed(int,bool): This slot allows to change the indicator color to green or red for the ith
 * status.
 */
class MODULE_UI_QT_CLASS_API SStatus : public QObject,
                                       public sight::ui::base::IEditor
{
public:

    SIGHT_DECLARE_SERVICE(SStatus, sight::ui::base::IEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SStatus() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SStatus() noexcept;

    /**
     * @name Slots API
     *@{
     */
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_CHANGE_TO_GREEN_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_CHANGE_TO_RED_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_CHANGE_TO_ORANGE_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_TOGGLE_GREEN_RED_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_CHANGE_NTH_TO_GREEN_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_CHANGE_NTH_TO_RED_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_CHANGE_NTH_TO_ORANGE_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_TOGGLE_NTH_GREEN_RED_SLOT;
/** @} */

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    void starting() override;

    /**
     * @brief Destroy the layout.
     */
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Configures the service
    void configuring() override;

    /// Overrides
    void info(std::ostream& _sstream) override;

    /// SLOT : change label color
    void changeToGreen();

    /// SLOT : change label color
    void changeToRed();

    /// SLOT : change label color
    void changeToOrange();

    /// SLOT : change label color (true = green, false = red)
    void toggleGreenRed(const bool green);

    /// SLOT : change nth label color
    void changeNthToGreen(const int index);

    /// SLOT : change nth label color
    void changeNthToRed(const int index);

    /// SLOT : change nth label color
    void changeNthToOrange(const int index);

    /// SLOT : change nth label color (true = green, false = red)
    void toggleNthGreenRed(const int index, const bool green);

private:

    /// Number of status
    size_t m_count;

    QVector<QPointer<QLabel> > m_indicator;
    QVector<QPointer<QLabel> > m_labelStatus;

    std::string m_greenTooltip;  ///< Tooltip for green status
    std::string m_redTooltip;    ///< Tooltip for red status
    std::string m_orangeTooltip; ///< Tooltip for orange status
    std::string m_layout;        ///< Layout orientation

    bool m_isCircular; ///< label is a circle if true (else it's a square)

    size_t m_width;  ///< width of idicator
    size_t m_height; ///< height of idicator
};

} // namespace sight::module::ui::qt
