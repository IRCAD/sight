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

const ::fwCom::Signals::SignalKeyType visuOgreAdaptor::SInteractorStyle::s_ADD_POINT_SIG    = "pointAdded";
const ::fwCom::Signals::SignalKeyType visuOgreAdaptor::SInteractorStyle::s_REMOVE_POINT_SIG = "pointRemoved";

const ::fwCom::Slots::SlotKeyType visuOgreAdaptor::SInteractorStyle::s_ADD_POINT_SLOT    = "addPoint";
const ::fwCom::Slots::SlotKeyType visuOgreAdaptor::SInteractorStyle::s_REMOVE_POINT_SLOT = "removePoint";

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

SInteractorStyle::SInteractorStyle() noexcept
{
    newSlot( s_ADD_POINT_SLOT, &::visuOgreAdaptor::SInteractorStyle::addPoint, this );
    newSlot( s_REMOVE_POINT_SLOT, &::visuOgreAdaptor::SInteractorStyle::removePoint, this );

    m_sigAddPoint    = newSignal< PointClickedSignalType >( s_ADD_POINT_SIG );
    m_sigRemovePoint = newSignal< PointClickedSignalType >( s_REMOVE_POINT_SIG );
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

    m_queryFlags = config.get<std::uint32_t>("queryFlags", m_queryFlags);
}

//------------------------------------------------------------------------------

void SInteractorStyle::starting()
{
    this->initialize();

    this->setInteractorStyle();

    if(!m_pickerStyle.empty())
    {
        ::fwRenderOgre::interactor::IPickerInteractor::sptr pickerInteractor =
            this->getRenderService()->getLayer(m_layerID)->getSelectInteractor();

        if(pickerInteractor)
        {
            m_connections.connect(pickerInteractor, ::fwRenderOgre::interactor::IPickerInteractor::s_ADD_POINT_SIG,
                                  this->getSptr(), ::visuOgreAdaptor::SInteractorStyle::s_ADD_POINT_SLOT);
            m_connections.connect(pickerInteractor, ::fwRenderOgre::interactor::IPickerInteractor::s_REMOVE_POINT_SIG,
                                  this->getSptr(), ::visuOgreAdaptor::SInteractorStyle::s_REMOVE_POINT_SLOT);
        }
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
    if(!m_pickerStyle.empty())
    {
        if(s_STYLES_PICKER.count(m_pickerStyle))
        {
            const auto style = s_STYLES_PICKER.at(m_pickerStyle);

            ::fwRenderOgre::interactor::IInteractor::sptr interactor = ::fwRenderOgre::interactorFactory::New(style,
                                                                                                              this->getSceneManager()->getName());
            SLM_ASSERT(this->getID() + " : Unknown picker interactor style : " + style, interactor);

            auto layer            = this->getRenderService()->getLayer(m_layerID);
            auto pickerInteractor = ::fwRenderOgre::interactor::IPickerInteractor::dynamicCast(interactor);
            pickerInteractor->initPicker();
            pickerInteractor->setQueryFlags(m_queryFlags);
            layer->setSelectInteractor(::fwRenderOgre::interactor::IPickerInteractor::dynamicCast(interactor));
        }
        else
        {
            SLM_WARN(this->getID() + " : '" + s_CONFIG_PICKER +"' has an unknown value : '" + m_pickerStyle +"'");
        }
    }
    else
    {
        SLM_WARN(this->getID() + " : '" + s_CONFIG_PICKER +"' is not set.");
    }

    if(!m_movementStyle.empty())
    {
        if(s_STYLES_MOVEMENT.count(m_movementStyle))
        {
            const auto style = s_STYLES_MOVEMENT.at(m_movementStyle);

            ::fwRenderOgre::interactor::IInteractor::sptr interactor = ::fwRenderOgre::interactorFactory::New(style,
                                                                                                              this->getSceneManager()->getName());
            SLM_ASSERT(this->getID() + " : Unknown movement interactor style : " + style, interactor);

            auto layer = this->getRenderService()->getLayer(m_layerID);
            layer->setMoveInteractor(::fwRenderOgre::interactor::IMovementInteractor::dynamicCast(interactor));
        }
        else
        {
            SLM_WARN(this->getID() + " : '" + s_CONFIG_MOVEMENT +"' has an unknown value : '" + m_movementStyle +"'");
        }
    }
    else
    {
        SLM_WARN(this->getID() + " : '" + s_CONFIG_MOVEMENT +"' is not set.");
    }

}

//------------------------------------------------------------------------------

void SInteractorStyle::addPoint( ::fwData::Object::sptr obj )
{
    m_sigAddPoint->asyncEmit( obj );
}

//------------------------------------------------------------------------------

void SInteractorStyle::removePoint( ::fwData::Object::sptr obj )
{
    m_sigRemovePoint->asyncEmit( obj );
}

} //namespace visuOgreAdaptor
