/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiReconstructionQt/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/Material.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QObject>
#include <QPointer>

class QRadioButton;
class QCheckBox;
class QButtonGroup;
class QAbstractButton;

namespace uiReconstructionQt
{

/**
 * @brief Display a widget to change the reconstruction representation (surface, point, edge, ...).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiReconstructionQt::RepresentationEditor">
       <inout key="reconstruction" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b reconstruction [::fwData::Reconstruction]: reconstruction that will be updated
 */
class UIRECONSTRUCTIONQT_CLASS_API RepresentationEditor : public QObject,
                                                          public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (RepresentationEditor)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIRECONSTRUCTIONQT_API RepresentationEditor() noexcept;

    /// Destructor. Do nothing.
    UIRECONSTRUCTIONQT_API virtual ~RepresentationEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Initialize the UI
    virtual void starting() override;

    /// Clean the UI
    virtual void stopping() override;

    /// Update the UI according to the reconstruction
    virtual void updating() override;

    /// Do nothing.
    virtual void configuring() override;

    /// Notify the changes
    void notifyMaterial();

protected Q_SLOTS:

    void onChangeRepresentation( int id );
    void onChangeShading( int id );
    void onShowNormals(int state );

private:

    void refreshNormals();
    void refreshRepresentation();
    void refreshShading();

    QPointer<QButtonGroup> m_buttonGroup;
    QPointer<QButtonGroup> m_buttonGroupShading;
    QPointer<QButtonGroup> m_normalsRadioBox;

    ::fwData::Material::sptr m_material;

};

} // uiReconstructionQt
