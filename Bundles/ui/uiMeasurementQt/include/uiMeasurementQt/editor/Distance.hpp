/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiMeasurementQt/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace uiMeasurementQt
{
namespace editor
{
/**
 * @brief   Distance service is represented by a button. It allows to show distances in a generic scene.
 *
 * @note Sends a signal to request a distance. It should be conected to an ImageMultiDistance adaptor to create the
 * distance.
 *
 * @section Signals Signals
 *  \b distanceRequested() : signal to request a distance. It should be conected to an ImageMultiDistance
 * adaptor to create the distance.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::uiMeasurementQt::editor::Distance" >
       <inout key="image" uid="..." />
   </service>
   @endcode

 * @subsection In-Out In-Out
 * -\b image [::fwData::Image]: Image in which we calculate the distance.
 */
class UIMEASUREMENTQT_CLASS_API Distance : public QObject,
                                           public ::fwGui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (Distance)(::fwGui::editor::IEditor) )

    /// Constructor. Do nothing.
    UIMEASUREMENTQT_API Distance() noexcept;

    /// Destructor. Do nothing.
    UIMEASUREMENTQT_API virtual ~Distance() noexcept override;

    /**
     * @name Signal
     * @{
     */
    UIMEASUREMENTQT_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_REQUESTED_SIG;
    typedef ::fwCom::Signal< void () > DistanceRequestedSignalType;
    /**
     * @}
     */
protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     *
     * This method launches the IEditor::starting method.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     *
     * This method launches the IEditor::stopping method.
     */
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

    /// Initialize the widget
    virtual void configuring() override;

protected Q_SLOTS:
    /**
     * @brief Notify the scene that a distance is added.
     */
    void onDistanceButton();

private:
    QPointer< QPushButton > m_distButton;

    DistanceRequestedSignalType::sptr m_sigDistanceRequested; /// signal emitted when a distance is requested

};
} // namespace editor
} // uiMeasurementQt
