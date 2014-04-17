/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIRECONSTRUCTIONQT_REPRESENTATION_EDITOR_HPP_
#define _UIRECONSTRUCTIONQT_REPRESENTATION_EDITOR_HPP_

#include <QObject>
#include <QPointer>

#include <fwTools/Failed.hpp>
#include <fwData/Material.hpp>


#include <gui/editor/IEditor.hpp>

#include "uiReconstructionQt/config.hpp"

class QRadioButton;
class QCheckBox;
class QButtonGroup;
class QAbstractButton;

namespace uiReconstruction
{

/**
 * @brief   RepresentationEditor service.
 * @class   RepresentationEditor
 * 
 * @date    2010.
 */
class UIRECONSTRUCTIONQT_CLASS_API RepresentationEditor : public  QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (RepresentationEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIRECONSTRUCTIONQT_API RepresentationEditor() throw() ;

    /// Destructor. Do nothing.
    UIRECONSTRUCTIONQT_API virtual ~RepresentationEditor() throw() ;

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Management of observations ( overrides )
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    void notifyMaterial();
    void notifyMesh();

protected Q_SLOTS:

    void onChangeRepresentation( int id );
    void onChangeShading( int id );
    void onShowNormals(int state );


private:

    void refreshNormals();
    void refreshRepresentation() ;
    void refreshShading() ;

    QPointer<QButtonGroup> m_buttonGroup;
    QPointer<QButtonGroup> m_buttonGroupShading;
    QPointer<QButtonGroup> m_normalsRadioBox ;

    ::fwData::Material::sptr m_material ;

};

} // uiReconstruction

#endif /*_UIRECONSTRUCTIONQT_REPRESENTATION_EDITOR_HPP_*/


