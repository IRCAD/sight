/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuQt/SnapshotEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace uiVisuQt
{

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SnapshotEditor::s_SNAPPED_SIG = "snapped";

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuQt::SnapshotEditor );

//------------------------------------------------------------------------------

SnapshotEditor::SnapshotEditor() noexcept
{
    m_sigSnapped = newSignal< SnappedSignalType >(s_SNAPPED_SIG);
}

//------------------------------------------------------------------------------

SnapshotEditor::~SnapshotEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer() );

    ::boost::filesystem::path path = ::fwRuntime::getBundleResourceFilePath("uiVisuQt", "camera-photo.png");
    QIcon icon(QString::fromStdString(path.string()));
    m_snapButton = new QPushButton(icon, "");
    m_snapButton->setToolTip(QObject::tr("Snapshot"));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(m_snapButton);
    hlayout->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(hlayout);

    QObject::connect(m_snapButton, SIGNAL(clicked()), this, SLOT(onSnapButton()));

}

//------------------------------------------------------------------------------

void SnapshotEditor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SnapshotEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SnapshotEditor::updating()
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::swapping()
{

}

//------------------------------------------------------------------------------

void SnapshotEditor::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::onSnapButton()
{
    SLM_TRACE_FUNC();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);
    if( container->isVisible() )
    {
        std::string filename = this->requestFileName();

        if(!filename.empty())
        {
            m_sigSnapped->asyncEmit(filename);
        }
    }
    else
    {
        std::string msgInfo("It is not possible to snapshot the negato view. This view is not shown on screen.");
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Negato view snapshot");
        messageBox.setMessage( msgInfo );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

std::string SnapshotEditor::requestFileName()
{
    std::string fileName = "";

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Save snapshot as");
    dialogFile.addFilter("Image file", "*.jpg *.jpeg *.bmp *.png *.tiff");
    dialogFile.addFilter("jpeg", "*.jpg *.jpeg");
    dialogFile.addFilter("bmp", "*.bmp");
    dialogFile.addFilter("png", "*.png");
    dialogFile.addFilter("tiff", "*.tiff");
    dialogFile.addFilter("all", "*.*");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        fileName = result->getPath().string();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(result->getPath().parent_path()) );
    }

    return fileName;
}
//------------------------------------------------------------------------------

} // namespace uiVisuQt
