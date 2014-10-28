/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITORQT_CLASSFACTORYREGISTRYINFO_HPP__
#define __MONITORQT_CLASSFACTORYREGISTRYINFO_HPP__

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

    fwCoreServiceClassDefinitionsMacro ( (ClassFactoryRegistryInfo)( ::fwGui::IActionSrv ) ) ;

    /// Does nothing
    MONITORQT_API ClassFactoryRegistryInfo() throw() ;

    /// Does nothing
    MONITORQT_API virtual ~ClassFactoryRegistryInfo() throw() ;

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
    QDialog* m_dialog;

    /// Widget used to show association ( service impl , data impl )
    QTreeWidget *m_tree;

};

} // namespace action
} // namespace monitor

#endif // __MONITORQT_CLASSFACTORYREGISTRYINFO_HPP__

