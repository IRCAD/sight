/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_SMATERIALSELECTOR_HPP__
#define __UIVISUOGRE_SMATERIALSELECTOR_HPP__

#include <fwData/Reconstruction.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QComboBox>
#include <QString>
#include <QLabel>

#include "uiVisuOgre/config.hpp"

namespace uiVisuOgre
{

/**
 * @brief   Allows to select an Ogre material template and apply it to the current reconstruction
 * @class   SMaterialSelector
 */
class UIVISUOGRE_CLASS_API SMaterialSelector : public QObject,
                                               public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SMaterialSelector)(::gui::editor::IEditor) );

    /// Constructor.
    UIVISUOGRE_API SMaterialSelector() throw();

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SMaterialSelector() throw();

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SMaterialSelectorInstance" impl="::uiVisuOgre::SMaterialSelector" type="::gui::editor::IEditor">
        </service>
       @endcode
     */
    UIVISUOGRE_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Start the service. Create UI
    UIVISUOGRE_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Stop the service. Delete UI
    UIVISUOGRE_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Update UI depending on current reconstruction
    UIVISUOGRE_API virtual void updating() throw ( ::fwTools::Failed );

    UIVISUOGRE_API virtual void swapping() throw(::fwTools::Failed);

protected Q_SLOTS:

    void onSelectedModeItem(const QString& text);

private:

    void updateMaterial();

    QPointer<QLabel> m_materialText;
    QPointer<QComboBox> m_materialBox;
};

} // uiVisuOgre

#endif // __UIVISUOGRE_SMATERIALSELECTOR_HPP__
