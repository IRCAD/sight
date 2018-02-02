/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SInteractorStyle.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/interactor/IMovementInteractor.hpp>
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SInteractorStyle,
                        ::fwData::Object);

namespace visuOgreAdaptor
{

const ::fwCom::Signals::SignalKeyType visuOgreAdaptor::SInteractorStyle::s_POINT_CLICKED_SIG = "pointClickedSignal";
const ::fwCom::Slots::SlotKeyType visuOgreAdaptor::SInteractorStyle::s_POINT_CLICKED_SLOT    = "pointClickedSlot";

static const std::string s_CONFIG_PICKER   = "picker";
static const std::string s_CONFIG_MOVEMENT = "movement";

static const std::map<std::string, std::string> s_STYLES_PICKER = {
    { "Mesh", "::fwRenderOgre::interactor::MeshPickerInteractor"},
    { "Video", "::fwRenderOgre::interactor::VideoPickerInteractor"},
};

static const std::map<std::string, std::string> s_STYLES_MOVEMENT = {
    { "Trackball", "::fwRenderOgre::interactor::TrackballInteractor"},
    { "Fixed", "::fwRenderOgre::interactor::FixedStyleInteractor"},
    { "Negato2D", "::fwRenderOgre::interactor::Negato2DInteractor"},
    { "VR", "::fwRenderOgre::interactor::VRWidgetsInteractor"}
};

//------------------------------------------------------------------------------

SInteractorStyle::SInteractorStyle() noexcept :
    m_pickerStyle(""),
    m_movementStyle(""),
    m_sigPointClicked(nullptr)
{
    newSlot( s_POINT_CLICKED_SLOT, &::visuOgreAdaptor::SInteractorStyle::clickedPoint, this );

    m_sigPointClicked = newSignal< PointClickedSignalType >( s_POINT_CLICKED_SIG );
}

//------------------------------------------------------------------------------

SInteractorStyle::~SInteractorStyle() noexcept
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count(s_CONFIG_PICKER))
    {
        m_pickerStyle = config.get<std::string>(s_CONFIG_PICKER);
    }

    if(config.count(s_CONFIG_MOVEMENT))
    {
        m_movementStyle = config.get<std::string>(s_CONFIG_MOVEMENT);
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::starting()
{
    this->initialize();

    this->setInteractorStyle();

    if(std::strcmp("", m_pickerStyle.c_str()) != 0)
    {
        ::fwRenderOgre::interactor::IPickerInteractor::sptr pickerInteractor =
            this->getRenderService()->getLayer(m_layerID)->getSelectInteractor();

        OSLM_ASSERT("There is no interactor found for this layer", pickerInteractor);
        m_connections.connect(pickerInteractor,
                              ::fwRenderOgre::interactor::IInteractor::s_POINT_CLICKED_SIG,
                              this->getSptr(),
                              ::visuOgreAdaptor::SInteractorStyle::s_POINT_CLICKED_SLOT);
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::updating()
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::stopping()
{
    m_connections.disconnect();
}

//------------------------------------------------------------------------------

void SInteractorStyle::setInteractorStyle()
{
    if(std::strcmp("", m_pickerStyle.c_str()) != 0)
    {
        const auto style = s_STYLES_PICKER.at(m_pickerStyle);

        ::fwRenderOgre::interactor::IInteractor::sptr interactor = ::fwRenderOgre::interactorFactory::New(style);
        interactor->setSceneID(this->getSceneManager()->getName());

        OSLM_WARN_IF("Unknown picker interactor style : " << style, interactor);
        if(interactor)
        {
            this->getRenderService()->getLayer(m_layerID)->setSelectInteractor(::fwRenderOgre::interactor::IPickerInteractor::dynamicCast(
                                                                                   interactor));
        }
    }

    if(std::strcmp("", m_movementStyle.c_str()) != 0)
    {
        const auto style = s_STYLES_MOVEMENT.at(m_movementStyle);

        ::fwRenderOgre::interactor::IInteractor::sptr interactor = ::fwRenderOgre::interactorFactory::New(style);
        interactor->setSceneID(this->getSceneManager()->getName());

        OSLM_WARN_IF("Unknown movement interactor style : " << style, interactor);
        if(interactor)
        {
            this->getRenderService()->getLayer(m_layerID)->setMoveInteractor(::fwRenderOgre::interactor::IMovementInteractor::dynamicCast(
                                                                                 interactor));
        }
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::clickedPoint( ::fwData::Object::sptr obj )
{
    m_sigPointClicked->asyncEmit( obj );
}

} //namespace visuOgreAdaptor
