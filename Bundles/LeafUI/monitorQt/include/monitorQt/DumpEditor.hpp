/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITORQT_DUMPEDITOR_HPP__
#define __MONITORQT_DUMPEDITOR_HPP__

#include <vector>

#include <boost/signals/connection.hpp>

#include <QPointer>
#include <QPushButton>
#include <QSignalMapper>
#include <QTableView>
#include <QTableWidget>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "monitorQt/config.hpp"

class QTimer;

namespace monitor
{

/**
 * @class   DumpEditor
 * @brief   Editor to dump or restore selected buffer.
 * @date    2010-2013
 */
class MONITORQT_CLASS_API DumpEditor : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (DumpEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Does nothing.
    MONITORQT_API DumpEditor() throw() ;

    /// Destructor. Does nothing.
    MONITORQT_API virtual ~DumpEditor() throw() ;

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Install the layout and call updating() method
    virtual void starting() throw(::fwTools::Failed);

    /// Stop the layout.
    virtual void stopping() throw(::fwTools::Failed);

    /// Update the choice selection
    virtual void updating() throw(::fwTools::Failed);

    /// Call updating() method
    virtual void swapping() throw(::fwTools::Failed);

    /// Calls classic IAction methods to configure
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides. Does nothing.
    virtual void info( std::ostream &_sstream ) ;

    /// Start m_updateTimer, call on buffManager signal emit ( see m_refreshSignal )
    void onUpdate();

protected Q_SLOTS:

    /// This method is called when an item is pressed.
    void changeStatus(int);

    /// Slot called when user clic on button m_refresh, call updating() method
    void onRefreshButton();

private:

    // Managed buffers
    std::vector< const void * const * > m_objectsUID;

    /// Widget to print some information on managed buffer by system
    QTableWidget* m_list;

    /// Button to force refresh
    QPushButton*  m_refresh;

    /// Mapper use in list widget to map each dump button to an action
    QSignalMapper* m_mapper;

    /// Editor to manage dump policy
    QTableView* m_policyEditor;

    /// Editor to show few memory information
    QTableView* m_infoEditor;

    /// Manage connection between buffManager updated signal and onUpdate method
    ::boost::signals::scoped_connection m_refreshSignal;

    /// Timer use to call each 300 ms onRefreshButton() slot
    QPointer<QTimer> m_updateTimer;
};

} // monitor

#endif //__MONITORQT_DUMPEDITOR_HPP__

