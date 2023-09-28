/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <filesystem>
#include <core/base.hpp>
#include <service/macros.hpp>
#include <core/runtime/path.hpp>

#include "SShowAbout.hpp"

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

SShowAbout::SShowAbout() noexcept :

    m_fsAboutPath(""),
    m_title("About"),
    m_size(500, 300)
{
}

//------------------------------------------------------------------------------

SShowAbout::~SShowAbout() noexcept =
    default;

//------------------------------------------------------------------------------

void SShowAbout::info(std::ostream& _sstream)
{
    _sstream << "SShowAbout" << std::endl;
}

//------------------------------------------------------------------------------

void SShowAbout::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->getConfiguration();

    const auto filename = config.get<std::string>("filename.<xmlattr>.id");
    // Convert the path from a module location
    m_fsAboutPath = core::runtime::get_module_resource_file_path(filename);

    m_bServiceIsConfigured = std::filesystem::exists(m_fsAboutPath);
    SIGHT_WARN_IF("About file " + filename + " doesn't exist", !m_bServiceIsConfigured);

    m_title = config.get<std::string>("title", m_title);

    m_size.setWidth(config.get<int>("size.<xmlattr>.width", m_size.width()));
    m_size.setHeight(config.get<int>("size.<xmlattr>.height", m_size.height()));
}

//------------------------------------------------------------------------------

void SShowAbout::updating()
{
    SIGHT_ASSERT("The service 'SShowAbout' isn't configured properly.", m_bServiceIsConfigured);

    auto* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString::fromStdString(m_title));
    QUrl url = QUrl::fromLocalFile(QString::fromStdString(m_fsAboutPath.string()));
#if defined(QT_WEBKIT)
    QWebView* htmlView = new QWebView(dialog);
    htmlView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    htmlView->load(url);
    QObject::connect(htmlView, SIGNAL(linkClicked(const QUrl&)), this, SLOT(onUrlClicked(const QUrl&)));
#else
    auto* htmlView = new QTextBrowser(dialog);
    htmlView->setSource(url);
    htmlView->setOpenExternalLinks(true);
    htmlView->setMinimumSize(m_size);
    QStringList searchPaths;
    searchPaths.append(QString::fromStdString(m_fsAboutPath.parent_path().string()));
    htmlView->setSearchPaths(searchPaths);
#endif
    auto* okButton = new QPushButton(QObject::tr("Ok"));
    auto* h_layout = new QHBoxLayout();
    h_layout->addStretch();
    h_layout->addWidget(okButton);
    h_layout->setContentsMargins(5, 5, 5, 5);

    auto* line = new QFrame(dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    auto* layout = new QVBoxLayout();
    layout->addWidget(htmlView, 0);
    layout->addWidget(line, 0);
    layout->addLayout(h_layout, 0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    dialog->setLayout(layout);

    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(dialog, SIGNAL(accepted()), dialog, SLOT(deleteLater()));
    dialog->setModal(true);
    dialog->show();
}

//------------------------------------------------------------------------------

void SShowAbout::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SShowAbout::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SShowAbout::onUrlClicked(const QUrl& url)
{
    QDesktopServices::openUrl(url);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
