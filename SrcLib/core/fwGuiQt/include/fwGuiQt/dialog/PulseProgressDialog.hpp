/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_DIALOG_PULSEPROGRESSDIALOG_HPP__
#define __FWGUIQT_DIALOG_PULSEPROGRESSDIALOG_HPP__

#include "fwGuiQt/config.hpp"

#include <fwGui/dialog/IPulseProgressDialog.hpp>

#include <QPointer>
#include <QProgressDialog>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   This class allows us to show a pulse progress bar.
 */
class FWGUIQT_CLASS_API PulseProgressDialog : public ::fwGui::dialog::IPulseProgressDialog
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (PulseProgressDialog)(::fwGui::dialog::IPulseProgressDialog),
                                            (()),
                                            ::fwGui::factory::New< PulseProgressDialog > );

    FWGUIQT_API PulseProgressDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~PulseProgressDialog();

    /// override
    FWGUIQT_API void setTitle(const std::string& title) override;

    /// override
    FWGUIQT_API void setMessage(const std::string& message) override;

    FWGUIQT_API void show() override;

protected:

    QPointer< QProgressDialog >  m_pdialog;

};
} // namespace dialog
} // namespace fwGuiQt

#endif /* __FWGUIQT_DIALOG_PULSEPROGRESSDIALOG_HPP__ */
