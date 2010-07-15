/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISUQT_SNAPSHOTEDITOR_HPP_
#define _UIVISUQT_SNAPSHOTEDITOR_HPP_

#include <QObject>
#include <QPointer>

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiVisuQt/config.hpp"

class QPushButton;

namespace uiVisu
{

/**
 * @brief   SnapshotEditor service.
 * @class   SnapshotEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIVISUQT_CLASS_API SnapshotEditor : public QObject, public ::gui::editor::IEditor
{

    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (SnapshotEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIVISUQT_API SnapshotEditor() throw() ;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~SnapshotEditor() throw() ;

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

protected slots:
    void onSnapButton();

private:
    std::string requestFileName();

    std::vector< std::string > m_scenesUID;

    QPointer< QPushButton > m_snapButton;

};

} // uiVisu

#endif /*_UIVISUQT_SNAPSHOTEDITOR_HPP_*/


