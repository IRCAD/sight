/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurementQt/editor/Distance.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <QIcon>
#include <QVBoxLayout>

namespace uiMeasurementQt
{
namespace editor
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiMeasurementQt::editor::Distance, ::fwData::Image );

const ::fwCom::Signals::SignalKeyType Distance::s_DISTANCE_REQUESTED_SIG = "distanceRequested";

//------------------------------------------------------------------------------

Distance::Distance() noexcept
{
    m_sigDistanceRequested = newSignal< DistanceRequestedSignalType >(s_DISTANCE_REQUESTED_SIG);
}

//------------------------------------------------------------------------------

Distance::~Distance() noexcept
{
}

//------------------------------------------------------------------------------

void Distance::starting()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    namespace fs = ::boost::filesystem;
    fs::path pathImageDist = ::fwRuntime::getBundleResourceFilePath("uiMeasurementQt", "distance.png");
    OSLM_ASSERT("Image "<< pathImageDist << "is missing", fs::exists(pathImageDist));

    QIcon imageDist(QString::fromStdString(pathImageDist.string()));

    m_distButton = new QPushButton( imageDist, tr(""));
    m_distButton->setToolTip(tr("Distance"));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_distButton, 1 );
    layout->setContentsMargins(0, 0, 0, 0);
    QObject::connect(m_distButton, SIGNAL(clicked()), this, SLOT(onDistanceButton()));

    qtContainer->setLayout( layout );
}

//------------------------------------------------------------------------------

void Distance::stopping()
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_distButton, SIGNAL(clicked()), this, SLOT(onDistanceButton()));

    this->destroy();
}

//------------------------------------------------------------------------------

void Distance::configuring()
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

void Distance::updating()
{
}

//------------------------------------------------------------------------------

void Distance::swapping()
{

}

//------------------------------------------------------------------------------

void Distance::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void Distance::onDistanceButton()
{
    SLM_ASSERT("No scene UID!", !m_scenesUID.empty());
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >("image");
    SLM_ASSERT("'image' key is not found.", image);

    // force distance to be shown
    image->setField("ShowDistances",  ::fwData::Boolean::New(true));

    m_sigDistanceRequested->asyncEmit();
}

//------------------------------------------------------------------------------
} // namespace editor
} // namespace uiMeasurementQt
