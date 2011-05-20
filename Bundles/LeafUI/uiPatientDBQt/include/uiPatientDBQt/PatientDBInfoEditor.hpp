/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDBQT_PATIENTDBINFOEDITOR_HPP_
#define _UIPATIENTDBQT_PATIENTDBINFOEDITOR_HPP_

#include <QPointer>
#include <QObject>
#include <QTreeWidget>

#include <gui/editor/IEditor.hpp>

#include "uiPatientDBQt/config.hpp"


namespace uiPatientDB
{
/**
 * @brief   This editor show information about the selected patient, study and acquisition.
 * @class   PatientDBInfoEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIPATIENTDBQT_CLASS_API PatientDBInfoEditor : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT
public :
    fwCoreServiceClassDefinitionsMacro ( (PatientDBInfoEditor)(::gui::editor::IEditor) ) ;

    /// Constructor
    UIPATIENTDBQT_API PatientDBInfoEditor();

    /// Destructor
    UIPATIENTDBQT_API virtual ~PatientDBInfoEditor() throw() ;

protected:

    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    virtual void configuring() throw (::fwTools::Failed);

    /// Updates patient information
    virtual void updating() throw (::fwTools::Failed);

    /// Receives message and call updating()
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void info( std::ostream &_sstream ) ;

private :

    // The selector panel
    QPointer< QTreeWidget > m_editorPanel;

};

} // namespace uiPatientDB

#endif // _UIPATIENTDBQT_PATIENTDBINFOEDITOR_HPP_

