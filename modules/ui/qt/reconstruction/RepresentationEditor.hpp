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

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/tools/Failed.hpp>

#include <data/Material.hpp>
#include <data/Reconstruction.hpp>

#include <ui/base/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QRadioButton;
class QCheckBox;
class QButtonGroup;
class QAbstractButton;

namespace sight::module::ui::qt::reconstruction
{

/**
 * @brief Display a widget to change the reconstruction representation (surface, point, edge, ...).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::reconstruction::RepresentationEditor">
       <inout key="reconstruction" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b reconstruction [sight::data::Reconstruction]: reconstruction that will be updated
 */
class MODULE_UI_QT_CLASS_API RepresentationEditor : public QObject,
                                                    public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(RepresentationEditor, sight::ui::base::IEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API RepresentationEditor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~RepresentationEditor() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Initialize the UI
    void starting() override;

    /// Clean the UI
    void stopping() override;

    /// Update the UI according to the reconstruction
    void updating() override;

    /// Do nothing.
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Notify the changes
    void notifyMaterial();

protected Q_SLOTS:

    void onChangeRepresentation(int id);
    void onChangeShading(int id);
    void onShowNormals(int state);

private:

    void refreshNormals();
    void refreshRepresentation();
    void refreshShading();

    QPointer<QButtonGroup> m_buttonGroup;
    QPointer<QButtonGroup> m_buttonGroupShading;
    QPointer<QButtonGroup> m_normalsRadioBox;

    data::Material::sptr m_material;

    data::ptr<data::Reconstruction, data::Access::inout> m_rec {this, "reconstruction", true};
};

} // uiReconstructionQt
