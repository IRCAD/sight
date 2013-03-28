/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MONITORQT_UI_ACTION_COMPONENTSTREE_HPP_
#define _MONITORQT_UI_ACTION_COMPONENTSTREE_HPP_

#include <QDialog>
#include <QPointer>
#include <QObject>
#include <QTreeWidget>

#include <fwGui/IActionSrv.hpp>

#include "monitorQt/config.hpp"

namespace monitor
{
namespace action
{

/**
 * @brief   Implements an action that show bundle information
 * @author IRCAD (Research and Development Team).
 */
class MONITORQT_CLASS_API ComponentsTree : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ComponentsTree)(::fwGui::IActionSrv) ) ;

    /// Does nothing
    MONITORQT_API ComponentsTree() throw() ;

    /// Does nothing
    MONITORQT_API virtual ~ComponentsTree() throw() ;

protected :

    /// Calls classic IAction methods to configure
    void configuring() throw (::fwTools::Failed);

    /// Prepare dialog box
    void starting() throw (::fwTools::Failed);

    /// Fill factory information in m_tree and show the dialog
    void updating() throw (::fwTools::Failed);

    /// Delete dialog box
    void stopping() throw (::fwTools::Failed);

private:

    /// Dialog shown on update method
    QPointer< QDialog > m_dialog;

    /// Widget used to show association ( service impl , data impl )
    QPointer< QTreeWidget > m_treeContainer;
};


} // namespace action
} // namespace monitor


#endif //_MONITORQT_UI_ACTION_COMPONENTSTREE_HPP_
