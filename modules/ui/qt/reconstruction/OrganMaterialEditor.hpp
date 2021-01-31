/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/tools/Failed.hpp>

#include <QObject>
#include <QPointer>

#include <ui/base/editor/IEditor.hpp>

class QSlider;
class QPushButton;
class QLabel;

namespace sight::modules::ui::qt::reconstruction
{

/**
 * @brief Display a widget to change the reconstruction material (color and transparency).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::ui::qt::reconstruction::OrganMaterialEditor">
       <inout key="reconstruction" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b reconstruction [data::Reconstruction]: reconstruction containing the material to update.
 */
class MODULE_UI_QT_CLASS_API OrganMaterialEditor : public QObject,
                                                   public sight::ui::base::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(OrganMaterialEditor, sight::ui::base::editor::IEditor)

    /// Constructor. Do nothing.
    MODULE_UI_QT_API OrganMaterialEditor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~OrganMaterialEditor() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Initialize the UI
    virtual void starting() override;

    /// Clean the UI
    virtual void stopping() override;

    /// Update the UI according to the material (color and transparency widgets)
    virtual void updating() override;

    /// Do nothing
    virtual void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Update the UI according to the material (color and transparency widgets)
    void refreshMaterial( );

    /// Notify the material changes
    void materialNotification( );

protected Q_SLOTS:

    void onOpacitySlider( int value);
    void onColorButton();

private:

    QPointer<QPushButton> m_colourButton;
    QPointer<QSlider> m_opacitySlider;
    QPointer<QLabel> m_transparencyValue;
};

} // uiReconstructionQt
