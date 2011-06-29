/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_PROGRESSDIALOG_HPP
#define _FWGUIQT_PROGRESSDIALOG_HPP


#include <string>

// Defined QT_NO_KEYWORDS because of conflict with boost::signals namespace.
#ifndef QT_NO_KEYWORDS
#define QT_NO_KEYWORDS
#define QT_NO_KEYWORDS_FWDEF
#endif

#include <QApplication>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QString>

#include <fwGui/dialog/ProgressDialog.hpp>
#include "fwGuiQt/container/QtContainer.hpp"

#ifdef QT_NO_KEYWORDS_FWDEF
#undef QT_NO_KEYWORDS
#undef QT_NO_KEYWORDS_FWDEF
#endif

#include "fwGuiQt/config.hpp"


QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 * @class   ProgressDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 * @note Do not inherit from fwProgressDialog but embed a fwProgressDialog because this lasted is not copiable.
 * @todo ProgressDialog is not commented.
 */
class FWGUIQT_CLASS_API ProgressDialog : public QObject, public ::fwGui::dialog::IProgressDialog
{
    Q_OBJECT

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ProgressDialog)(::fwGui::dialog::IProgressDialog), (()), new ProgressDialog );

    FWGUIQT_API ProgressDialog( const std::string title="Progression", std::string message= std::string(86,' '));

    FWGUIQT_API virtual ~ProgressDialog();

    FWGUIQT_API void operator()(float percent,std::string msg);

    /// override
    FWGUIQT_API void setTitle(const std::string &title);

    /// override
    FWGUIQT_API void setMessage(const std::string &message);

    FWGUIQT_API void hideCancelButton();

protected Q_SLOTS:
    void cancelPressed();

protected :

    QString m_title;

    QPointer< QProgressDialog >  m_pdialog;
    QPointer< QProgressBar >     m_pprogressbar;
    QPointer< QPushButton >      m_pcancelButton;
    QPointer< QMainWindow >      m_pmainWindow;

};
} // namespace dialog
} // namespace fwGuiQt


#endif /* _FWGUIQT_PROGRESSDIALOG_HPP */
