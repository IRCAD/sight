/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
    QObject::disconnect(m_distButton, SIGNAL(clicked()), this, SLOT(onDistanceButton()));

    this->destroy();
}

//------------------------------------------------------------------------------

void Distance::configuring()
{
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void Distance::updating()
{
}

//------------------------------------------------------------------------------

void Distance::onDistanceButton()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >("image");
    SLM_ASSERT("'image' key is not found.", image);

    // force distance to be shown
    image->setField("ShowDistances",  ::fwData::Boolean::New(true));

    m_sigDistanceRequested->asyncEmit();
}

//------------------------------------------------------------------------------
} // namespace editor
} // namespace uiMeasurementQt
