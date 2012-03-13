/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QWidget>
#include <QString>
#include <QIcon>
#include <QPushButton>
#include <QHBoxLayout>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/String.hpp>
#include <fwData/Composite.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiVisuQt/SnapshotEditor.hpp"

namespace uiVisu
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiVisu::SnapshotEditor , ::fwTools::Object ) ;


SnapshotEditor::SnapshotEditor() throw()
{

}

//------------------------------------------------------------------------------

SnapshotEditor::~SnapshotEditor() throw()
{}

//------------------------------------------------------------------------------

void SnapshotEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget *container = qtContainer->getQtContainer();

    ::boost::filesystem::path path("Bundles/uiVisuQt_" + std::string(UIVISUQT_VER) + "/camera-photo.png");
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
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QObject::disconnect(m_snapButton, SIGNAL(clicked()), this, SLOT(onSnapButton()));

    qtContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SnapshotEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->initialize();

    std::vector < Configuration > snapConfig = m_configuration->find("snap");
    if(!snapConfig.empty())
    {
        std::vector < Configuration > sceneConfig = snapConfig.at(0)->find("scene");
        for(unsigned int i =0; i < sceneConfig.size(); i++)
        {
            OSLM_ASSERT("Wrong tag name: " << sceneConfig.at(i)->getName(), sceneConfig.at(i)->getName() == "scene");
            SLM_ASSERT("UID attribute is missing", sceneConfig.at(i)->hasAttribute("uid"));

            std::string value(sceneConfig.at(i)->getAttributeValue("uid"));
            m_scenesUID.push_back(value);
        }
    }
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

void SnapshotEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
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
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget *container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);
    if( container->isVisible() )
    {
        for(unsigned int i=0; i < m_scenesUID.size(); i++)
        {
            ::fwServices::IService::sptr service = ::fwServices::get(m_scenesUID.at(i));
            ::fwData::Composite::sptr composite = service->getObject< ::fwData::Composite >();
            SLM_ASSERT("SnapshotEditor sceneUID " << m_scenesUID.at(i) <<" isn't a GenericScene?" , composite);

            ::fwData::Object::NewSptr dataInfo;

            ::fwData::String::NewSptr sceneID;
            sceneID->value() = m_scenesUID.at(i);
            ::fwData::String::NewSptr filename;

            filename->value() = this->requestFileName();
            if(!filename->value().empty())
            {
                dataInfo->setField_NEWAPI("sceneID", sceneID);
                dataInfo->setField_NEWAPI("filename", filename);
                ::fwComEd::CompositeMsg::NewSptr compositeMsg;
                compositeMsg->addEvent( "SNAP", dataInfo );
                ::fwServices::IEditionService::notify(this->getSptr(), composite, compositeMsg);
            }
        }
    }
    else
    {
        std::string msgInfo("Sorry, it is not possible to snapshot the negato view. This view is not shown on screen.");
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

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        fileName = result->getPath().string();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(result->getPath().parent_path()) );
    }

    return fileName;
}
//------------------------------------------------------------------------------

}// namespace uiVisu
