/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QIcon>

#include <fwCore/base.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/Object.hpp>
#include <fwData/String.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiMeasurementQt/editor/Distance.hpp"


namespace uiMeasurement
{
namespace editor
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiMeasurement::editor::Distance , ::fwData::Image ) ;


Distance::Distance() throw()
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

Distance::~Distance() throw()
{}

//------------------------------------------------------------------------------

void Distance::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    namespace fs = ::boost::filesystem;
    fs::path pathImageDist ("Bundles/uiMeasurementQt_0-1/distance.png");
    OSLM_ASSERT("Image "<< pathImageDist << "is missing", fs::exists(pathImageDist));

    QIcon imageDist(QString::fromStdString(pathImageDist.string()));

    m_distButton = new QPushButton( imageDist, tr(""), container) ;
    m_distButton->setToolTip(tr("Distance"));

    QVBoxLayout* layout  = new QVBoxLayout();
    layout->addWidget( m_distButton, 1 );
    layout->setContentsMargins(0, 0, 0, 0);
    QObject::connect(m_distButton, SIGNAL(clicked()), this, SLOT(onDistanceButton()));

    container->setLayout( layout );
}

//------------------------------------------------------------------------------

void Distance::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_distButton, SIGNAL(clicked()), this, SLOT(onDistanceButton()));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void Distance::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();

    std::vector < Configuration > placeInSceneConfig = m_configuration->find("placeInScene");
    if(!placeInSceneConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", placeInSceneConfig.at(0)->hasAttribute("uid"));
        m_scenesUID = placeInSceneConfig.at(0)->getAttributeValue("uid");
    }
}

//------------------------------------------------------------------------------

void Distance::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Distance::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void Distance::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Distance::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void Distance::onDistanceButton()
{
    SLM_ASSERT("No scene UID!", !m_scenesUID.empty());
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    // force distance to be shown
    image->setFieldSingleElement("ShowDistances",  ::fwData::Boolean::NewSptr(true));

    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::NEW_DISTANCE, ::fwData::String::NewSptr(m_scenesUID) );
    ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
}

//------------------------------------------------------------------------------
} // namespace editor
} // namespace uiMeasurement

