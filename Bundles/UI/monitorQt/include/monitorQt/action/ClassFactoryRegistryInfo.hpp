/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITORQT_ACTION_CLASSFACTORYREGISTRYINFO_HPP__
#define __MONITORQT_ACTION_CLASSFACTORYREGISTRYINFO_HPP__

#include "monitorQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <QApplication>
#include <QDialog>
#include <QTreeWidget>

namespace monitorQt
{
namespace action
{
/**
 * @brief   Implements an action to show services registered in factory
 */
class MONITORQT_CLASS_API ClassFactoryRegistryInfo : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (ClassFactoryRegistryInfo)( ::fwGui::IActionSrv ) );

    /// Does nothing
    MONITORQT_API ClassFactoryRegistryInfo() noexcept;

    /// Does nothing
    MONITORQT_API virtual ~ClassFactoryRegistryInfo() noexcept;

protected:

    /// Calls classic IAction methods to configure
    void configuring() override;

    /// Prepare dialog box
    void starting() override;

    /// Fill factory information in m_tree and show the dialog
    void updating() override;

    /// Delete dialog box
    void stopping() override;

private:

    /// Dialog shown on update method
    QDialog* m_dialog;

    /// Widget used to show association ( service impl , data impl )
    QTreeWidget* m_tree;

};

} // namespace action
} // namespace monitorQt

#endif // __MONITORQT_ACTION_CLASSFACTORYREGISTRYINFO_HPP__

