/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_STEXTURESELECTOR_HPP__
#define __UIVISUOGRE_STEXTURESELECTOR_HPP__

#include "uiVisuOgre/config.hpp"

#include <fwData/Reconstruction.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace uiVisuOgre
{

/**
 * @brief   Allows to select a ::fwData::Image and apply it to the current reconstruction as an Ogre texture
 */
class UIVISUOGRE_CLASS_API STextureSelector : public QObject,
                                              public ::gui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (STextureSelector)(::gui::editor::IEditor) );

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
    UIVISUOGRE_API virtual void configuring();

    /// Start the service. Create UI
    UIVISUOGRE_API virtual void starting();

    /// Stop the service. Delete UI
    UIVISUOGRE_API virtual void stopping();

    /// Do nothing
    UIVISUOGRE_API virtual void updating();

protected Q_SLOTS:

    void onLoadButton();
    void onDeleteButton();

private:

    QPointer<QPushButton> m_loadButton;
    QPointer<QPushButton> m_deleteButton;
};

} // uiVisuOgre

#endif // __UIVISUOGRE_STEXTURESELECTOR_HPP__
