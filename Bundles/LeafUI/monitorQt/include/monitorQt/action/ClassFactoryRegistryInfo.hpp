/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITORQT_ACTION_CLASSFACTORYREGISTRYINFO_HPP__
#define __MONITORQT_ACTION_CLASSFACTORYREGISTRYINFO_HPP__

#include <QApplication>
#include <QDialog>
#include <QTreeWidget>

#include <fwGui/IActionSrv.hpp>

#include "monitorQt/config.hpp"

namespace monitor
{
namespace action
{
/**
 * @brief   Implements an action to show services registered in factory
 */
class MONITORQT_CLASS_API ClassFactoryRegistryInfo : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ClassFactoryRegistryInfo)( ::fwGui::IActionSrv ) );

    /// Does nothing
    MONITORQT_API ClassFactoryRegistryInfo() noexcept;

    /// Does nothing
    MONITORQT_API virtual ~ClassFactoryRegistryInfo() noexcept;

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
    QDialog* m_dialog;

    /// Widget used to show association ( service impl , data impl )
    QTreeWidget *m_tree;

};

} // namespace action
} // namespace monitor

#endif // __MONITORQT_ACTION_CLASSFACTORYREGISTRYINFO_HPP__

