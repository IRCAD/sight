/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QObject>
#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHelpEngine>
#include <QHelpContentWidget>
#include <QTextBrowser>

#include <boost/filesystem/operations.hpp>

#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include "uiGenericQt/action/ShowHelpContents.hpp"

namespace uiGeneric
{
namespace action
{

//------------------------------------------------------------------------------
/** @brief Help browser */
class HelpBrowser : public QTextBrowser
{
public:
    HelpBrowser(QHelpEngine *helpEngine, QWidget *parent = 0)
        : QTextBrowser(parent), helpEngine(helpEngine)
    {
    }

    QVariant loadResource(int type, const QUrl &url)
    {
        if (url.scheme() == "qthelp")
            return QVariant(helpEngine->fileData(url));
        else
            return QTextBrowser::loadResource(type, url);
    }

private:
    QHelpEngine *helpEngine;
};
//------------------------------------------------------------------------------


REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiGeneric::action::ShowHelpContents , ::fwTools::Object ) ;

//------------------------------------------------------------------------------

ShowHelpContents::ShowHelpContents( ) throw():
    m_bServiceIsConfigured(false),
    m_fsHelpPath("")
{}

//------------------------------------------------------------------------------

ShowHelpContents::~ShowHelpContents() throw()
{}

//------------------------------------------------------------------------------

void ShowHelpContents::info(std::ostream &_sstream )
{
    _sstream << "Action for show help contents" << std::endl;
}

//------------------------------------------------------------------------------

void ShowHelpContents::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        m_fsHelpPath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsHelpPath);
        OSLM_WARN_IF("Help file " <<filename<< " doesn't exist", !m_bServiceIsConfigured);
        OSLM_TRACE("Filename found " << filename ) ;
    }
}

//------------------------------------------------------------------------------

void ShowHelpContents::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("The Help service isn't configured properly.", m_bServiceIsConfigured);

    QDialog* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Help"));
    QHelpEngine* helpEngine = new QHelpEngine(QString::fromStdString(m_fsHelpPath.string()), dialog);
    if (!helpEngine->setupData())
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( "Help file is missing or not correct." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
       // Setup help engine information failed.
       // qhc (Qt Help Collection) or qch (Qt Compressed Help) file is not corect.
    }
    else
    {
        QSplitter *helpPanel = new QSplitter(Qt::Horizontal);
        HelpBrowser *helpBrowser = new HelpBrowser(helpEngine, dialog);
        helpPanel->insertWidget(0, helpEngine->contentWidget());
        helpPanel->insertWidget(1, helpBrowser);
        helpPanel->setStretchFactor(1, 1);

        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->addWidget(helpPanel);
        dialog->setLayout( hLayout );
        QObject::connect(helpEngine->contentWidget(), SIGNAL(linkActivated(const QUrl &)), helpBrowser, SLOT(setSource(const QUrl &)));

        dialog->exec();
    }
}

//------------------------------------------------------------------------------

void ShowHelpContents::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowHelpContents::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void ShowHelpContents::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------


} // namespace action
} // namespace uiGeneric
