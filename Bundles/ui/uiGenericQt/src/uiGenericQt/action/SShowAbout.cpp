/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <QApplication>
#if defined(QT_WEBKIT)
#include <QWebView>
#include <QWebPage>
#else
#include <QTextBrowser>
#endif
#include <QDesktopServices>
#include <QDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include <boost/filesystem/operations.hpp>
#include <fwCore/base.hpp>
#include <fwServices/macros.hpp>
#include <fwRuntime/operations.hpp>

#include "uiGenericQt/action/SShowAbout.hpp"

namespace uiGenericQt
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiGenericQt::action::SShowAbout, ::fwData::Object );

//------------------------------------------------------------------------------

SShowAbout::SShowAbout( ) noexcept :
    m_bServiceIsConfigured(false),
    m_fsAboutPath(""),
    m_title("About"),
    m_size(500, 300)
{
}

//------------------------------------------------------------------------------

SShowAbout::~SShowAbout() noexcept
{
}

//------------------------------------------------------------------------------

void SShowAbout::info(std::ostream& _sstream )
{
    _sstream << "SShowAbout" << std::endl;
}

//------------------------------------------------------------------------------

void SShowAbout::configuring()
{
    this->::fwGui::IActionSrv::initialize();

    typedef SPTR (::fwRuntime::ConfigurationElement) ConfigurationElement;

    ConfigurationElement cfgFilename = m_configuration->findConfigurationElement("filename");
    ConfigurationElement cfgTitle    = m_configuration->findConfigurationElement("title");
    ConfigurationElement cfgSize     = m_configuration->findConfigurationElement("size");

    if(cfgFilename)
    {
        const std::string& filename = cfgFilename->getExistingAttributeValue("id");
        // Convert the path from a bundle location
        m_fsAboutPath = ::fwRuntime::getBundleResourceFilePath(filename);

        m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsAboutPath);
        SLM_WARN_IF("About file " + filename + " doesn't exist", !m_bServiceIsConfigured);
        SLM_TRACE("Filename found '" + filename + "'");
    }

    if(cfgTitle)
    {
        m_title = cfgTitle->getValue();
        SLM_TRACE("Set title to '" + m_title + "'");
    }

    if(cfgSize)
    {
        const std::string& w = cfgSize->getExistingAttributeValue("width");
        const std::string& h = cfgSize->getExistingAttributeValue("height");

        m_size.setWidth(std::stoi(w));
        m_size.setHeight(std::stoi(h));

        OSLM_TRACE("Set frame size to (" << m_size.width() << ", " << m_size.height() << ")");
    }
}

//------------------------------------------------------------------------------

void SShowAbout::updating( )
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("The service 'SShowAbout' isn't configured properly.", m_bServiceIsConfigured );

    QDialog* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString::fromStdString(m_title));
    QUrl url(QString::fromStdString(m_fsAboutPath.string()));
#if defined(QT_WEBKIT)
    QWebView* htmlView = new QWebView(dialog);
    htmlView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    htmlView->load( url );
    QObject::connect( htmlView, SIGNAL(linkClicked(const QUrl &)), this, SLOT(onUrlClicked(const QUrl &)));
#else
    QTextBrowser* htmlView = new QTextBrowser(dialog);
    htmlView->setSource(url);
    htmlView->setOpenExternalLinks(true);
    htmlView->setMinimumSize(m_size);
    QStringList searchPaths;
    searchPaths.append(QString::fromStdString(m_fsAboutPath.parent_path().string()));
    htmlView->setSearchPaths(searchPaths);
#endif
    QPushButton* okButton = new QPushButton(QObject::tr("Ok"));
    QHBoxLayout* hLayout  = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(okButton);
    hLayout->setContentsMargins(5, 5, 5, 5);

    QFrame* line = new QFrame(dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(htmlView, 0);
    layout->addWidget(line, 0);
    layout->addLayout(hLayout, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    dialog->setLayout( layout );

    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(dialog, SIGNAL(accepted()), dialog, SLOT(deleteLater()));
    dialog->setModal(true);
    dialog->show();
}

//------------------------------------------------------------------------------

void SShowAbout::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SShowAbout::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SShowAbout::onUrlClicked(const QUrl& url )
{
    QDesktopServices::openUrl(url);
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiGenericQt
