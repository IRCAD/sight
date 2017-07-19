/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITORQT_ACTION_COMPONENTSTREE_HPP__
#define __MONITORQT_ACTION_COMPONENTSTREE_HPP__

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
 * @brief  Implements an action that show bundle information.
 */
class MONITORQT_CLASS_API ComponentsTree : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ComponentsTree)(::fwGui::IActionSrv) );

    /// Does nothing
    MONITORQT_API ComponentsTree() noexcept;

    /// Does nothing
    MONITORQT_API virtual ~ComponentsTree() noexcept;

protected:

    /// Calls classic IAction methods to configure
    void configuring();

    /// Prepare dialog box
    void starting();

    /// Fill factory information in m_tree and show the dialog
    void updating();

    /// Delete dialog box
    void stopping();

private:

    /// Dialog shown on update method
    QPointer< QDialog > m_dialog;

    /// Widget used to show association ( service impl , data impl )
    QPointer< QTreeWidget > m_treeContainer;
};


} // namespace action
} // namespace monitor

#endif //__MONITORQT_ACTION_COMPONENTSTREE_HPP__

