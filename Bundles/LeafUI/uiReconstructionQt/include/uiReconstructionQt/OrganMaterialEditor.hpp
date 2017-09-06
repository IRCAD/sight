/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIRECONSTRUCTIONQT_ORGANMATERIALEDITOR_HPP__
#define __UIRECONSTRUCTIONQT_ORGANMATERIALEDITOR_HPP__

#include "uiReconstructionQt/config.hpp"

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QPushButton;
class QLabel;

namespace uiReconstructionQt
{
/**
 * @brief   OrganMaterialEditor service.
 */
class UIRECONSTRUCTIONQT_CLASS_API OrganMaterialEditor : public QObject,
                                                         public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (OrganMaterialEditor)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIRECONSTRUCTIONQT_API OrganMaterialEditor() noexcept;

    /// Destructor. Do nothing.
    UIRECONSTRUCTIONQT_API virtual ~OrganMaterialEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting();

    ///This method launches the IEditor::stopping method.
    virtual void stopping();

    virtual void updating();

    virtual void swapping();

    virtual void configuring();

    /// Overrides
    virtual void info( std::ostream& _sstream );

    void refreshMaterial( );
    void materialNotification( );

protected Q_SLOTS:

    void onOpacitySlider( int value);
    void onColorButton();

private:

    QPointer<QPushButton> m_colourButton;
    QPointer<QSlider> m_opacitySlider;
    QPointer<QLabel> m_transparencyValue;
};

} // uiReconstructionQt

#endif /*__UIRECONSTRUCTIONQT_ORGANMATERIALEDITOR_HPP__*/

