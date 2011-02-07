/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISUQT_POINT_EDITOR_HPP
#define _UIVISUQT_POINT_EDITOR_HPP


#include <QObject>
#include <QLineEdit>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <fwData/Point.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiVisuQt/config.hpp"

namespace uiVisu
{

/**
 * @brief   PointEditor service allows to display point information.
 * @class   PointEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIVISUQT_CLASS_API PointEditor : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (PointEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIVISUQT_API PointEditor() throw() ;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~PointEditor() throw() ;

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overides )
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    void updating() throw(::fwTools::Failed);

    void swapping() throw(::fwTools::Failed);

    void configuring() throw( ::fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;


private:

    QPointer< QLineEdit >   m_textCtrl_x;
    QPointer< QLineEdit >   m_textCtrl_y;
    QPointer< QLineEdit >   m_textCtrl_z;

};

} // uiData

#endif /*_UIVISUQT_POINT_EDITOR_HPP_*/
