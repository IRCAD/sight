/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    fwCoreServiceClassDefinitionsMacro( (STextureSelector)(::fwGui::editor::IEditor) )

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
