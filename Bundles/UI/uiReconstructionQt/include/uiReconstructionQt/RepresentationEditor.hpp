/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIRECONSTRUCTIONQT_REPRESENTATIONEDITOR_HPP__
#define __UIRECONSTRUCTIONQT_REPRESENTATIONEDITOR_HPP__

#include "uiReconstructionQt/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/Material.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QRadioButton;
class QCheckBox;
class QButtonGroup;
class QAbstractButton;

namespace uiReconstructionQt
{

/**
 * @brief   RepresentationEditor service.
 */
class UIRECONSTRUCTIONQT_CLASS_API RepresentationEditor : public QObject,
                                                          public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (RepresentationEditor)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIRECONSTRUCTIONQT_API RepresentationEditor() noexcept;

    /// Destructor. Do nothing.
    UIRECONSTRUCTIONQT_API virtual ~RepresentationEditor() noexcept;

    /**
     * @name Signals API
     * @{
     */
    /// normals mode (0: none, 1: point, 2: cell), reconstruction fwID
    typedef ::fwCom::Signal< void ( std::uint8_t, std::string ) > NormalsModeModifiedSignalType;
    UIRECONSTRUCTIONQT_API static const ::fwCom::Signals::SignalKeyType s_NORMALS_MODE_MODIFIED_SIG;
    /** @} */

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() override;

    ///This method launches the IEditor::stopping method.
    virtual void stopping() override;

    virtual void updating() override;

    virtual void swapping() override;

    virtual void configuring() override;

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

#endif /*__UIRECONSTRUCTIONQT_REPRESENTATIONEDITOR_HPP__*/

