/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "uiVisuOgre/config.hpp"

#include <fwData/Reconstruction.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace uiVisuOgre
{

/**
 * @brief   Allows to select a ::fwData::Image and apply it to the current reconstruction as an Ogre texture
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::uiVisuOgre::STextureSelector">
            <inout key="reconstruction" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b reconstruction [::fwData::Reconstruction]: reconstruction where the texture should be applied.
 */
class UIVISUOGRE_CLASS_API STextureSelector : public QObject,
                                              public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(STextureSelector, ::fwGui::editor::IEditor);

    UIVISUOGRE_API STextureSelector() noexcept;
    UIVISUOGRE_API virtual ~STextureSelector() noexcept;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="STextureSelectorInstance" type="::uiVisuOgre::STextureSelector">
        </service>
       @endcode
     */
    UIVISUOGRE_API virtual void configuring() override;

    /// Start the service. Create UI
    UIVISUOGRE_API virtual void starting() override;

    /// Stop the service. Delete UI
    UIVISUOGRE_API virtual void stopping() override;

    /// Do nothing
    UIVISUOGRE_API virtual void updating() override;

protected Q_SLOTS:

    void onLoadButton();
    void onDeleteButton();

private:

    QPointer<QPushButton> m_loadButton;
    QPointer<QPushButton> m_deleteButton;
};

} // uiVisuOgre
