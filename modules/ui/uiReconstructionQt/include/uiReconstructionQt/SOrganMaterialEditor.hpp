/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "uiReconstructionQt/config.hpp"

#include <core/tools/Failed.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QPushButton;
class QLabel;

namespace uiReconstructionQt
{

/**
 * @brief Display a widget to change the reconstruction material (color and transparency).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiReconstructionQt::SOrganMaterialEditor">
       <inout key="reconstruction" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b reconstruction [::fwData::Reconstruction]: reconstruction containing the material to update.
 */
class UIRECONSTRUCTIONQT_CLASS_API SOrganMaterialEditor : public QObject,
                                                          public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(SOrganMaterialEditor, ::fwGui::editor::IEditor)

    /// Creates the service.
    UIRECONSTRUCTIONQT_API SOrganMaterialEditor() noexcept;

    /// Destroys the service.
    UIRECONSTRUCTIONQT_API virtual ~SOrganMaterialEditor() noexcept;

private:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Object::s_MODIFIED_SIG of s_RECONSTRUCTION_INOUT to s_UPDATE_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Configures the service.
    virtual void configuring() override final;

    /// Sets the connections and the UI elements.
    virtual void starting() override final;

    /// Updates the UI according to the material (color and transparency widgets).
    virtual void updating() override final;

    /// Destroys the connections and cleans the container.
    virtual void stopping() override final;

    /// Updates the UI according to the material (color and transparency widgets)
    void refreshMaterial( );

    /// Notifies the material changes.
    void materialNotification( );

    QPointer<QPushButton> m_diffuseColourButton;
    QPointer<QPushButton> m_ambientColourButton;
    QPointer<QSlider> m_opacitySlider;
    QPointer<QLabel> m_transparencyValue;

private Q_SLOTS:

    /**
     * @brief Slot: called when the opacity slider changed.
     * @param _value The new opacity value.
     */
    void onOpacitySlider(int _value);

    /// Slot: called when the diffuse color button is clicked.
    void onDiffuseColorButton();

    /// Slot: called when the ambient color button is clicked.
    void onAmbientColorButton();

};

} // uiReconstructionQt
