/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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

#include <fwServices/macros.hpp>

#include <QWidget>
#include <QString>
#include <QIcon>
#include <QPushButton>
#include <QHBoxLayout>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

namespace uiVisu
{

const ::fwCom::Signals::SignalKeyType SnapshotEditor::s_SNAPPED_SIG = "snapped";

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisu::SnapshotEditor, ::fwData::Object );


SnapshotEditor::SnapshotEditor() throw()
{
    m_sigSnapped = newSignal< SnappedSignalType >(s_SNAPPED_SIG);
}

//------------------------------------------------------------------------------

SnapshotEditor::~SnapshotEditor() throw()
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget *container = qtContainer->getQtContainer();

    ::boost::filesystem::path path(std::string(BUNDLE_PREFIX) + "/uiVisuQt_" + std::string(
                                       UIVISUQT_VER) + "/camera-photo.png");
    QIcon icon(QString::fromStdString(path.string()));
    m_snapButton = new QPushButton(icon, "", container);
    m_snapButton->setToolTip(QObject::tr("Snapshot"));

    QHBoxLayout *hlayout = new QHBoxLayout(container);
    hlayout->addWidget(m_snapButton);
    hlayout->setContentsMargins(0,0,0,0);

    container->setLayout(hlayout);

    QObject::connect(m_snapButton, SIGNAL(clicked()), this, SLOT(onSnapButton()));

}

//------------------------------------------------------------------------------

void SnapshotEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QObject::disconnect(m_snapButton, SIGNAL(clicked()), this, SLOT(onSnapButton()));

    qtContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SnapshotEditor::configuring() throw(fwTools::Failed)
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SnapshotEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::swapping() throw(::fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void SnapshotEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::onSnapButton()
{
    SLM_TRACE_FUNC();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget *container = qtContainer->getQtContainer();
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
    dialogFile.addFilter("Image file","*.jpg *.jpeg *.bmp *.png *.tiff");
    dialogFile.addFilter("jpeg","*.jpg *.jpeg");
    dialogFile.addFilter("bmp","*.bmp");
    dialogFile.addFilter("png","*.png");
    dialogFile.addFilter("tiff","*.tiff");
    dialogFile.addFilter("all","*.*");
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

}// namespace uiVisu
