/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#if defined(QT_WEBKIT)
#include <QWebView>
#include <QWebPage>
#else
#include <QTextBrowser>
#endif
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QDesktopServices>

#include <boost/filesystem/operations.hpp>
#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/macros.hpp>

#include "uiGenericQt/action/ShowAcknowledgments.hpp"

namespace uiGeneric
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiGeneric::action::ShowAcknowledgments , ::fwTools::Object ) ;

//------------------------------------------------------------------------------

ShowAcknowledgments::ShowAcknowledgments( ) throw():
m_bServiceIsConfigured(false),
m_fsAckPath("")
{}

//------------------------------------------------------------------------------

ShowAcknowledgments::~ShowAcknowledgments() throw()
{}

//------------------------------------------------------------------------------

void ShowAcknowledgments::info(std::ostream &_sstream )
{
    _sstream << "ShowAcknowledgments" << std::endl;
}

//------------------------------------------------------------------------------
void ShowAcknowledgments::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        m_fsAckPath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsAckPath);
        OSLM_WARN_IF("Acknowledgments file " <<filename<< " doesn't exist", !m_bServiceIsConfigured);
        OSLM_TRACE("Filename found " << filename ) ;
    }
}

//------------------------------------------------------------------------------
void ShowAcknowledgments::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("The Acknowledgments service isn't configured properly.", m_bServiceIsConfigured);

    QDialog* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Acknowledgments"));
    QUrl url(QString::fromStdString(m_fsAckPath.string()));
#if defined(QT_WEBKIT)
    QWebView* htmlView = new QWebView(dialog);
    htmlView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    htmlView->load( url );
    QObject::connect( htmlView, SIGNAL(linkClicked(const QUrl&)),this, SLOT(onUrlClicked(const QUrl&)));
#else
    QTextBrowser * htmlView = new QTextBrowser (dialog);
    htmlView->setSource(url);
    htmlView->setOpenExternalLinks(true);
    htmlView->setMinimumSize(500, 400);
#endif
    QPushButton* okButton = new QPushButton(QObject::tr("Ok"));
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(okButton);
    hLayout->setContentsMargins(5,5,5,5);

    QFrame* line = new QFrame(dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(htmlView, 0);
    layout->addWidget(line, 0);
    layout->addLayout(hLayout, 0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    dialog->setLayout( layout );

    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    dialog->exec();
}

//------------------------------------------------------------------------------

void ShowAcknowledgments::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowAcknowledgments::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void ShowAcknowledgments::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void ShowAcknowledgments::onUrlClicked(const QUrl & url )
{
    QDesktopServices::openUrl(url);
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiGeneric
