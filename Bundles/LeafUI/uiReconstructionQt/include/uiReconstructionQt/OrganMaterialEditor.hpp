/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIRECONSTRUCTIONQT_ORGAN_MATERIAL_EDITOR_HPP_
#define _UIRECONSTRUCTIONQT_ORGAN_MATERIAL_EDITOR_HPP_

#include <QObject>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiReconstructionQt/config.hpp"

class QSlider;
class QPushButton;

namespace uiReconstruction
{
/**
 * @brief   OrganMaterialEditor service.
 * @class   OrganMaterialEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIRECONSTRUCTIONQT_CLASS_API OrganMaterialEditor : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (OrganMaterialEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIRECONSTRUCTIONQT_API OrganMaterialEditor() throw() ;

    /// Destructor. Do nothing.
    UIRECONSTRUCTIONQT_API virtual ~OrganMaterialEditor() throw() ;

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overides )
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    void refreshMaterial( );
    void materialNotification( );

protected slots:

    void onOpacitySlider( int value);
    void onColorButton();

private:

    QPointer<QPushButton> m_colourButton ;
    QPointer<QSlider> m_opacitySlider ;

};

} // uiReconstruction

#endif /*_UIRECONSTRUCTIONQT_ORGAN_MATERIAL_EDITOR_HPP_*/


