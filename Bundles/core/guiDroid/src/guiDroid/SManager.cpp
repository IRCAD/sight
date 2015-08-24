/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiDroid/SManager.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Object.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <JUIButton.h>
#include <JUITextView.h>
#include <JUIWindow.h>

#include <sstream>
#include <locale>

namespace guiDroid
{

fwServicesRegisterMacro( ::fwServices::IService, ::guiDroid::SManager, ::fwData::Object);

//------------------------------------------------------------------------------
SManager::SManager() throw()
{
}

//------------------------------------------------------------------------------

SManager::~SManager() throw()
{
    for(WidgetData* widget : m_widgets)
    {
        delete widget;
    }
}

//------------------------------------------------------------------------------

void SManager::configuring() throw(::fwTools::Failed)
{
    SLM_ASSERT("Service hasn't configuration", m_configuration);

    // Create initializeLayoutManager
    // find gui configuration
    ::fwRuntime::ConfigurationElement::sptr cfgGui        = m_configuration->findConfigurationElement("gui");
    ::fwRuntime::ConfigurationElementContainer widgetCfgs = cfgGui->findAllConfigurationElement("widget");

    for(::fwRuntime::ConfigurationElement::sptr elem :   widgetCfgs.getElements())
    {
        WidgetData* widget = new WidgetData();

        SLM_ASSERT("Missing attribute 'type'", elem->hasAttribute("type"));
        SLM_ASSERT("Missing attribute 'label'", elem->hasAttribute("label"));
        SLM_ASSERT("Missing attribute 'align'", elem->hasAttribute("align"));

        widget->m_config.m_label = elem->getAttributeValue("label");

        std::string strType = elem->getAttributeValue("type");
        if(strType == "button")
        {
            widget->m_config.m_type = WidgetConfig::TYPE_BUTTON;
        }
        else if(strType == "textview")
        {
            widget->m_config.m_type = WidgetConfig::TYPE_TEXTVIEW;
        }

        std::string strAlign = elem->getAttributeValue("align");

        std::string hAlign, vAlign;

        size_t pos = strAlign.find("|");
        if( std::string::npos != pos)
        {
            vAlign.assign(strAlign, 0,pos);
            hAlign.assign(strAlign,pos +1,strAlign.size() - pos);

            if(vAlign == "top")
            {
                widget->m_config.m_vAlign = ndkGui::LAYOUT_PARAMETER_ALIGN_PARENT_TOP;
            }
            else if(vAlign == "bottom")
            {
                widget->m_config.m_vAlign = ndkGui::LAYOUT_PARAMETER_ALIGN_PARENT_BOTTOM;
            }
            else
            {
                widget->m_config.m_vAlign = ndkGui::LAYOUT_PARAMETER_CENTER_VERTICAL;
            }
            if(hAlign == "left")
            {
                widget->m_config.m_hAlign = ndkGui::LAYOUT_PARAMETER_ALIGN_PARENT_LEFT;
            }
            else if(hAlign == "right")
            {
                widget->m_config.m_hAlign = ndkGui::LAYOUT_PARAMETER_ALIGN_PARENT_RIGHT;
            }
            else
            {
                widget->m_config.m_hAlign = ndkGui::LAYOUT_PARAMETER_CENTER_HORIZONTAL;
            }
        }

        ::fwRuntime::ConfigurationElement::sptr widthCfg = elem->findConfigurationElement("width");

        if(widthCfg)
        {
            std::istringstream(widthCfg->getValue()) >> widget->m_config.m_width;
        }

        ::fwRuntime::ConfigurationElement::sptr heightCfg = elem->findConfigurationElement("height");
        if(heightCfg)
        {
            std::istringstream(heightCfg->getValue()) >> widget->m_config.m_height;
        }

        ::fwRuntime::ConfigurationElement::sptr marginsCfg = elem->findConfigurationElement("margins");

        if(marginsCfg)
        {
            marginsCfg->getValue();
            std::string strMargins, currentMargin;

            size_t prevPos = -1;
            strMargins = marginsCfg->getValue();
            for(unsigned int i = 0; i < 3; ++i)
            {

                pos = strMargins.find(",",prevPos+1);
                currentMargin.assign(strMargins, prevPos+1,pos - prevPos);
                std::istringstream(currentMargin) >> widget->m_config.m_margins[i];

                prevPos = pos;

            }
            currentMargin.assign(strMargins, prevPos+1,strMargins.size() - 1 - prevPos );
            std::istringstream(currentMargin) >> widget->m_config.m_margins[3];
        }

        ::fwRuntime::ConfigurationElement::sptr textSizeCfg = elem->findConfigurationElement("textSize");
        if(textSizeCfg)
        {
            widget->m_config.m_textSize = boost::lexical_cast<float>(textSizeCfg->getValue());
        }

        ::fwRuntime::ConfigurationElement::sptr slotCfg = elem->findConfigurationElement("slot");
        if(slotCfg)
        {
            std::string src, uid, key;

            src = slotCfg->getValue();
            pos = src.find("/");
            if( std::string::npos != pos)
            {
                uid.assign(src, 0,pos);
                key.assign(src,pos +1,src.size() - pos);

                SLM_ASSERT("Missing hasSlotsId attribute", !uid.empty());
                SLM_ASSERT("Missing slotKey attribute", !key.empty());

                widget->m_config.m_slotPair = std::make_pair(uid, key);
            }
        }

        m_widgets.push_back(widget);
    }

}

//------------------------------------------------------------------------------

void SManager::starting() throw(::fwTools::Failed)
{

    for(WidgetData* widget : m_widgets)
    {
        ndkGui::JUITextView* juiWidget;

        switch(widget->m_config.m_type)
        {
            case WidgetConfig::TYPE_BUTTON:
            {
                ndkGui::JUIButton *button = new ndkGui::JUIButton(widget->m_config.m_label.c_str());

                std::string HasSlotId = widget->m_config.m_slotPair.first;
                ::fwCom::Slots::SlotKeyType slotKey = widget->m_config.m_slotPair.second;

                SLM_ASSERT(" the slot  "<<HasSlotId<<" not found",::fwTools::fwID::exist(HasSlotId));
                ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(HasSlotId);
                ::fwCom::HasSlots::sptr hasSlots = std::dynamic_pointer_cast< ::fwCom::HasSlots >(obj);
                SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

                widget->m_slot = hasSlots->slot(slotKey);

                std::function<void(ndkGui::JUIView *, const int32_t)> button_handlers =
                    [widget](ndkGui::JUIView *button, const int32_t msg)
                    {
                        if (msg == ndkGui::JUICALLBACK_BUTTON_UP)
                        {
                            widget->m_state = !widget->m_state;
                            widget->m_slot->asyncRun(widget->m_state);
                        }
                    };

                button->SetCallback(button_handlers);

                juiWidget = static_cast<ndkGui::JUITextView*> ( button );

            }
            break;

            case  WidgetConfig::TYPE_TEXTVIEW:
            {
                juiWidget = new ndkGui::JUITextView(widget->m_config.m_label.c_str());
            }
            break;

            default:
                break;
        }

        juiWidget->AddRule(widget->m_config.m_vAlign,
                           ndkGui::LAYOUT_PARAMETER_TRUE);
        juiWidget->AddRule(widget->m_config.m_hAlign,
                           ndkGui::LAYOUT_PARAMETER_TRUE);
        juiWidget->SetAttribute("MinimumWidth", widget->m_config.m_width);
        juiWidget->SetAttribute("MinimumHeight", widget->m_config.m_height);
        juiWidget->SetAttribute("TextSize", ndkGui::ATTRIBUTE_UNIT_SP,widget->m_config.m_textSize);
        juiWidget->SetMargins(widget->m_config.m_margins[0], widget->m_config.m_margins[1],
                              widget->m_config.m_margins[2], widget->m_config.m_margins[3]);

        ndkGui::JUIWindow::GetInstance()->AddView(juiWidget);

    }

}

//------------------------------------------------------------------------------

void SManager::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SManager::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

} // namespace guiDroid
