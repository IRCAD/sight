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

#include "modules/ui/viz/config.hpp"

#include <data/Reconstruction.hpp>

#include <ui/base/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace sight::module::ui::viz
{

/**
 * @brief   Allows to select a data::Image and apply it to the current reconstruction as an Ogre texture
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::sight::module::ui::viz::STextureSelector">
            <inout key="reconstruction" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b reconstruction [sight::data::Reconstruction]: reconstruction where the texture should be applied.
 */
class MODULE_UI_VIZ_CLASS_API STextureSelector : public QObject,
                                                 public sight::ui::base::IEditor
{

Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(STextureSelector, sight::ui::base::IEditor)

    MODULE_UI_VIZ_API STextureSelector() noexcept;
    MODULE_UI_VIZ_API virtual ~STextureSelector() noexcept;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="STextureSelectorInstance" type="::sight::module::ui::viz::STextureSelector">
        </service>
       @endcode
     */
    MODULE_UI_VIZ_API virtual void configuring() override;

    /// Start the service. Create UI
    MODULE_UI_VIZ_API virtual void starting() override;

    /// Stop the service. Delete UI
    MODULE_UI_VIZ_API virtual void stopping() override;

    /// Do nothing
    MODULE_UI_VIZ_API virtual void updating() override;

protected Q_SLOTS:

    void onLoadButton();
    void onDeleteButton();

private:

    QPointer<QPushButton> m_loadButton;
    QPointer<QPushButton> m_deleteButton;
};

} // uiVisuOgre
