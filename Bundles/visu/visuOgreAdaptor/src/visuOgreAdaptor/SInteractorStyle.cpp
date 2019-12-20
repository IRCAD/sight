/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SInteractorStyle.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/interactor/IMovementInteractor.hpp>
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/interactor/MeshPickerInteractor.hpp>
#include <fwRenderOgre/interactor/Negato2DInteractor.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SInteractorStyle,
                        ::fwData::Object)

namespace visuOgreAdaptor
{

const ::fwCom::Signals::SignalKeyType visuOgreAdaptor::SInteractorStyle::s_PICKED_SIG = "picked";

const ::fwCom::Slots::SlotKeyType visuOgreAdaptor::SInteractorStyle::s_PICK_SLOT = "pick";

static const std::string s_PICKER_CONFIG   = "picker";
static const std::string s_MOVEMENT_CONFIG = "movement";
static const std::string s_QUERY_CONFIG    = "queryMask";

//------------------------------------------------------------------------------

SInteractorStyle::SInteractorStyle() noexcept
{
    newSlot( s_PICK_SLOT, &::visuOgreAdaptor::SInteractorStyle::picked, this );

    m_sigPicked = newSignal< PointClickedSigType >( s_PICKED_SIG );
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

    m_pickerStyle   = config.get<std::string>(s_PICKER_CONFIG, m_pickerStyle);
    m_movementStyle = config.get<std::string>(s_MOVEMENT_CONFIG, m_movementStyle);

    if(m_movementStyle == "VR" )
    {
        FW_DEPRECATED_MSG("VR Interactor is deprecated use a 'Trackball' interactor instead.", "21.0");
    }

    if(config.count(s_QUERY_CONFIG))
    {
        const std::string hexaMask = config.get<std::string>(s_QUERY_CONFIG);
        SLM_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2 &&
            hexaMask.substr(0, 2) == "0x");
        m_queryMask = static_cast< std::uint32_t >(std::stoul(hexaMask, nullptr, 16));
    }
    if(config.count("queryFlags"))
    {
        m_queryMask = config.get< std::uint32_t >("queryFlags");
        FW_DEPRECATED_MSG("The config `queryFlags` is deprecated, please use `" + s_QUERY_CONFIG +"`", "21.0");
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::starting()
{
    this->initialize();

    this->setInteractorStyle();
}

//------------------------------------------------------------------------------

void SInteractorStyle::updating()
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::stopping()
{
    const auto layer = this->getLayer();

    if(m_selectInteractor)
    {
        layer->removeInteractor(m_selectInteractor);
        m_selectInteractor.reset();
    }

    if(m_moveInteractor)
    {
        layer->removeInteractor(m_moveInteractor);
        m_moveInteractor.reset();
    }

    m_connections.disconnect();
}

//------------------------------------------------------------------------------

void SInteractorStyle::setInteractorStyle()
{
    const auto layer = this->getLayer();

    if(!m_pickerStyle.empty())
    {
        FW_DEPRECATED_MSG("::visuOgreAdaptor::SPicker should be used instead of SInteractorStyle", "21.0");
        auto meshPicker = std::make_shared< ::fwRenderOgre::interactor::MeshPickerInteractor >(layer);
        meshPicker->setQueryMask(m_queryMask);
        meshPicker->setPointClickedSig(m_sigPicked);

        m_selectInteractor = meshPicker;
        layer->addInteractor(m_selectInteractor);
    }
    else
    {
        SLM_WARN(this->getID() + " : '" + s_PICKER_CONFIG +"' is not set.");
    }

    if(!m_movementStyle.empty())
    {
        if(m_movementStyle == "Trackball")
        {
            m_moveInteractor = std::make_shared< ::fwRenderOgre::interactor::TrackballInteractor >(layer);
            FW_DEPRECATED_MSG("::visuOgreAdaptor::STrackballCamera should be used instead SInteractorStyle.", "21.0");
            layer->addInteractor(m_moveInteractor);
        }
        else if(m_movementStyle == "VR")
        {
            // HACK: temporarily fix the legacy VR interactor until everybody uses the new API.
            m_moveInteractor = std::make_shared< ::fwRenderOgre::interactor::VRWidgetsInteractor >(layer);
            FW_DEPRECATED_MSG("::visuOgreAdaptor::STrackballCamera should be used instead SInteractorStyle.", "21.0");
            layer->setMoveInteractor(m_moveInteractor);
        }
        else if(m_movementStyle == "Fixed")
        {
            FW_DEPRECATED_MSG("'Fixed' interaction is now the default,"
                              "remove SInteractorStyle for no interactions", "21.0");
        }
        else if(m_movementStyle == "Negato2D")
        {
            m_moveInteractor = std::make_shared< ::fwRenderOgre::interactor::Negato2DInteractor >();
            layer->addInteractor(m_moveInteractor);
            m_moveInteractor->setSceneID(this->getSceneManager()->getName());
        }
        else
        {
            SLM_WARN(this->getID() + " : '" + s_MOVEMENT_CONFIG +"' has an unknown value : '" + m_movementStyle +"'");
        }
    }
    else
    {
        SLM_WARN(this->getID() + " : '" + s_MOVEMENT_CONFIG +"' is not set.");
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::picked(::fwDataTools::PickingInfo _info)
{
    m_sigPicked->asyncEmit(_info);
}

} //namespace visuOgreAdaptor
