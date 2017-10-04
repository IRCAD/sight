/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_SMATERIALSELECTOR_HPP__
#define __UIVISUOGRE_SMATERIALSELECTOR_HPP__

#include "uiVisuOgre/config.hpp"

#include <fwData/Reconstruction.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QString>

namespace uiVisuOgre
{

/**
 * @brief   Allows to select an Ogre material template and apply it to the current reconstruction
 */
class UIVISUOGRE_CLASS_API SMaterialSelector : public QObject,
                                               public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SMaterialSelector)(::gui::editor::IEditor) );

    /// Constructor.
    UIVISUOGRE_API SMaterialSelector() noexcept;

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SMaterialSelector() noexcept;

    /**
     * @name Signals API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Signals::SignalKeyType s_SELECTED_SIG;
    typedef ::fwCom::Signal<void (std::string)> SelectedSignalType;
    /** @} */

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SMaterialSelectorInstance" type="::uiVisuOgre::SMaterialSelector">
        </service>
       @endcode
     */
    UIVISUOGRE_API virtual void configuring() override;

    /// Start the service. Create UI
    UIVISUOGRE_API virtual void starting() override;

    /// Stop the service. Delete UI
    UIVISUOGRE_API virtual void stopping() override;

    /// Update UI depending on current reconstruction
    UIVISUOGRE_API virtual void updating() override;

    UIVISUOGRE_API virtual void swapping() override;

protected Q_SLOTS:

    /// SLOT: Called when a material is selected
    void onSelectedModeItem(const QString& text);

    /// SLOT: Called when the reload button is pushed
    void onReloadMaterial();

private:

    void updateMaterial();

    QPointer<QComboBox> m_materialBox;
    QPointer<QPushButton> m_reloadButton;
};

} // uiVisuOgre

#endif // __UIVISUOGRE_SMATERIALSELECTOR_HPP__
