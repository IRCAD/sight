/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "videoQt/action/SVideoConfiguration.hpp"

#include <arUtils/preferences.hpp>

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Composite.hpp>
#include <fwData/location/Folder.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <boost/lexical_cast.hpp>

#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace videoQt
{

namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::videoQt::action::SVideoConfiguration, ::fwData::Object );

//-----------------------------------------------------------------------------

SVideoConfiguration::SVideoConfiguration() throw() : m_videoDirEdit(new QLineEdit())
{
}

//------------------------------------------------------------------------------

SVideoConfiguration::~SVideoConfiguration() throw()
{
}

//------------------------------------------------------------------------------

void SVideoConfiguration::starting() throw(::fwTools::Failed)
{
    this->actionServiceStarting();

    // Check preferences
    auto preferencesServicesList = ::fwServices::OSR::getServices("::preferences::IPreferencesService");
    if(!preferencesServicesList.empty())
    {
        ::fwServices::IService::sptr prefService = *preferencesServicesList.begin();
        ::fwData::Composite::sptr prefs          = prefService->getObject< ::fwData::Composite >();

        ::fwData::Composite::IteratorType iterPref = prefs->find( ::arUtils::s_VIDEORENDER_PREF );
        if ( iterPref != prefs->end() )
        {
            m_videoPref = ::fwData::Composite::dynamicCast(iterPref->second);
        }
        else
        {
            m_videoPref                             = ::fwData::Composite::New();
            (*prefs)[::arUtils::s_VIDEORENDER_PREF] = m_videoPref;
        }
    }
}

//------------------------------------------------------------------------------

void SVideoConfiguration::stopping() throw(::fwTools::Failed)
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SVideoConfiguration::configuring() throw(fwTools::Failed)
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SVideoConfiguration::updating() throw(::fwTools::Failed)
{
    if (m_videoPref)
    {
        ::fwData::String::sptr videoDir;
        ::fwData::Composite::IteratorType iterVideoDir = m_videoPref->find( ::arUtils::s_VIDEO_DIR_PREF );
        bool videoFound = (iterVideoDir  != m_videoPref->end());
        if (videoFound)
        {
            videoDir = ::fwData::String::dynamicCast(iterVideoDir->second);
            SLM_ERROR_IF("Wrong type of preference : '" + ::arUtils::s_VIDEO_DIR_PREF + "' parameter must be a string",
                         !videoDir);
        }
        else
        {
            videoDir = ::fwData::String::New();
        }

        m_videoDirEdit->setText(QString::fromStdString(videoDir->value()));

        QPointer<QDialog> dialog     = new QDialog();
        QPointer<QGridLayout> layout = new QGridLayout();

        QPointer<QLabel> videoDirLabel       = new QLabel(QObject::tr("video dir: "));
        QPointer<QPushButton> videoDirButton = new QPushButton("...");

        layout->addWidget(videoDirLabel, 0, 0);
        layout->addWidget(m_videoDirEdit, 0, 1);
        layout->addWidget(videoDirButton, 0, 2);

        QObject::connect(videoDirButton, SIGNAL(clicked()), this, SLOT(onSelectVideoDir()));

        QPointer<QPushButton> cancelButton = new QPushButton("Cancel");
        QPointer<QPushButton> okButton     = new QPushButton("OK");
        okButton->setDefault(true);

        QPointer<QHBoxLayout> buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(okButton);
        layout->addLayout(buttonLayout, 2, 1, 2, 2 );

        QObject::connect(cancelButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::reject);
        QObject::connect(okButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::accept);

        dialog->setLayout(layout);


        if (dialog->exec() == QDialog::Accepted)
        {
            //save video directory
            if(!m_videoDirEdit->text().isEmpty())
            {
                videoDir->value() = m_videoDirEdit->text().toStdString();
                if (!videoFound)
                {
                    (*m_videoPref)[::arUtils::s_VIDEO_DIR_PREF] = videoDir;
                }
            }
        }

    }
}

//------------------------------------------------------------------------------

void SVideoConfiguration::swapping() throw(::fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void SVideoConfiguration::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SVideoConfiguration::onSelectVideoDir()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Select video directory");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        m_videoDirEdit->setText( QString::fromStdString(result->getFolder().string()) );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
}


} // namespace action
} // namespace videoQt
