/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/editor/SStatus.hpp"

#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

#include <QColor>
#include <QHBoxLayout>
#include <QPalette>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

namespace uiTools
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTools::editor::SStatus, ::fwData::Object );

const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_GREEN_SLOT      = "changeToGreen";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_RED_SLOT        = "changeToRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_ORANGE_SLOT     = "changeToOrange";
const ::fwCom::Slots::SlotKeyType SStatus::s_TOGGLE_GREEN_RED_SLOT     = "toggleGreenRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_INT_CHANGE_TO_GREEN_SLOT  = "intChangeToGreen";
const ::fwCom::Slots::SlotKeyType SStatus::s_INT_CHANGE_TO_RED_SLOT    = "intChangeToRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_INT_CHANGE_TO_ORANGE_SLOT = "intChangeToOrange";
const ::fwCom::Slots::SlotKeyType SStatus::s_INT_TOGGLE_GREEN_RED_SLOT = "intToggleGreenRed";

//-----------------------------------------------------------------------------

SStatus::SStatus() noexcept :
    m_isCircular(false),
    m_width(20),
    m_height(20)
{
    m_slotChangeToGreen     = ::fwCom::newSlot( &SStatus::changeToGreen,  this );
    m_slotChangeToRed       = ::fwCom::newSlot( &SStatus::changeToRed,    this );
    m_slotChangeToOrange    = ::fwCom::newSlot( &SStatus::changeToOrange, this );
    m_slotToggleGreenRed    = ::fwCom::newSlot( &SStatus::toggleGreenRed, this );
    m_slotIntChangeToRed    = ::fwCom::newSlot( &SStatus::intChangeToRed,    this );
    m_slotIntChangeToGreen  = ::fwCom::newSlot( &SStatus::intChangeToGreen,  this );
    m_slotIntChangeToOrange = ::fwCom::newSlot( &SStatus::intChangeToOrange, this );
    m_slotIntToggleGreenRed = ::fwCom::newSlot( &SStatus::intToggleGreenRed, this );

    ::fwCom::HasSlots::m_slots
        ( s_CHANGE_TO_GREEN_SLOT, m_slotChangeToGreen   )
        ( s_CHANGE_TO_RED_SLOT, m_slotChangeToRed    )
        ( s_CHANGE_TO_ORANGE_SLOT, m_slotChangeToOrange    )
        ( s_TOGGLE_GREEN_RED_SLOT, m_slotToggleGreenRed )
        ( s_INT_CHANGE_TO_GREEN_SLOT, m_slotIntChangeToGreen   )
        ( s_INT_CHANGE_TO_RED_SLOT, m_slotIntChangeToRed    )
        ( s_INT_CHANGE_TO_ORANGE_SLOT, m_slotIntChangeToOrange    )
        ( s_INT_TOGGLE_GREEN_RED_SLOT, m_slotIntToggleGreenRed )
    ;

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//------------------------------------------------------------------------------

SStatus::~SStatus() noexcept
{
}

//------------------------------------------------------------------------------

void SStatus::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QBoxLayout* layout;
    if(m_layout == "horizontal")
    {
        layout = new QHBoxLayout();
    }
    else
    {
        layout = new QVBoxLayout();
    }
    for(size_t i = 0; i < m_count; ++i)
    {
        QPointer<QLabel> indicator = new QLabel();
        indicator->setFixedWidth(m_width);
        indicator->setFixedHeight(m_height);
        m_indicator.push_back(indicator);

        layout->addWidget(indicator);
        layout->addWidget(m_labelStatus.at(i));
    }
    qtContainer->setLayout(layout);
    for(int i = 0; i < m_count; ++i)
    {
        this->intChangeToRed(i);
    }
}

//------------------------------------------------------------------------------

void SStatus::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SStatus::configuring()
{
    this->initialize();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    m_greenTooltip  = this->getConfigTree().get<std::string>("green", "");
    m_redTooltip    = this->getConfigTree().get<std::string>("red", "");
    m_orangeTooltip = this->getConfigTree().get<std::string>("orange", "");
    m_layout        = this->getConfigTree().get<std::string>("layout", "horizontal");
    SLM_ASSERT("Value for element 'layout' should be 'horizontal' or 'vertical'.",
               m_layout == "horizontal" || m_layout == "vertical");

    const std::string form = this->getConfigTree().get<std::string>("form", "square");
    SLM_ASSERT("Value for element 'form' should be 'circle' or 'square'.", form == "circle" || form == "square");
    m_isCircular = (form == "circle");

    m_count = this->getConfigTree().get<size_t>("count", 1);
    if(m_count == 1)
    {
        const std::string label  = this->getConfigTree().get<std::string>("labelStatus", "");
        QPointer < QLabel > qLab = new QLabel();
        qLab->setText(QString::fromStdString(label));
        m_labelStatus.push_back(qLab);
    }
    else
    {
        const auto labelStatusConfig = this->getConfigTree().get_child("labels");
        const size_t nbLabel         = labelStatusConfig.count("labelStatus");
        SLM_ASSERT("Number of 'labelStatus' ("<<nbLabel<<") must be equal to number of needed status ("<<m_count <<")",
                   nbLabel == m_count);
        BOOST_FOREACH( const ::fwServices::IService::ConfigType::value_type &v,
                       labelStatusConfig.equal_range("labelStatus") )
        {
            const std::string label  = v.second.get<std::string>("");
            QPointer < QLabel > qLab = new QLabel();
            qLab->setText(QString::fromStdString(label));
            m_labelStatus.push_back(qLab);
        }

    }
    const ::fwServices::IService::ConfigType sizeConfig = this->getConfigTree().get_child("size");
    m_width  = sizeConfig.get<size_t>("width", 20);
    m_height = sizeConfig.get<size_t>("height", 20);
}

//------------------------------------------------------------------------------

void SStatus::updating()
{
}

//------------------------------------------------------------------------------

void SStatus::swapping()
{

}

//------------------------------------------------------------------------------

void SStatus::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SStatus::changeToGreen()
{
    m_indicator.at(0)->setStyleSheet("background-color: green; border-radius: "+QString(
                                         m_isCircular ? "10px;" : "0")+";");
    m_indicator.at(0)->setToolTip(QString::fromStdString(m_greenTooltip));
}

//------------------------------------------------------------------------------

void SStatus::changeToRed()
{
    m_indicator.at(0)->setStyleSheet("background-color: red; border-radius: "+QString(m_isCircular ? "10px;" : "0")+
                                     ";");
    m_indicator.at(0)->setToolTip(QString::fromStdString(m_redTooltip));
}

//------------------------------------------------------------------------------

void SStatus::changeToOrange()
{
    m_indicator.at(0)->setStyleSheet("background-color: orange; border-radius: "+QString(
                                         m_isCircular ? "10px;" : "0")+";");
    m_indicator.at(0)->setToolTip(QString::fromStdString(m_orangeTooltip));
}

//------------------------------------------------------------------------------

void SStatus::toggleGreenRed(const bool green)
{
    m_indicator.at(0)->setStyleSheet("background-color:"+ QString(green ? "green" : "red")+"; border-radius: "+
                                     QString(m_isCircular ? "10px;" : "0")+";");
    m_indicator.at(0)->setToolTip(green ? QString::fromStdString(m_greenTooltip) : QString::fromStdString(m_redTooltip));
}

//------------------------------------------------------------------------------

void SStatus::intChangeToGreen(const int index)
{
    OSLM_FATAL_IF(
        "Index("<<index <<") must be in vector range [0:" <<m_indicator.size()-1 <<"]",
        index < 0 || index >= m_count);
    m_indicator.at(index)->setStyleSheet("background-color: green; border-radius: "+QString(
                                             m_isCircular ? "10px;" : "0")+";");
    m_indicator.at(index)->setToolTip(QString::fromStdString(m_greenTooltip));
}

//------------------------------------------------------------------------------

void SStatus::intChangeToRed(const int index)
{
    OSLM_ERROR(" change du red cool");
    OSLM_FATAL_IF(
        "Index("<<index <<") must be in vector range [0:" <<m_indicator.size()-1 <<"]",
        index < 0 || index >= m_count);
    m_indicator.at(index)->setStyleSheet("background-color: red; border-radius: "+QString(
                                             m_isCircular ? "10px;" : "0")+";");
    m_indicator.at(index)->setToolTip(QString::fromStdString(m_redTooltip));
}

//------------------------------------------------------------------------------

void SStatus::intChangeToOrange(const int index)
{
    OSLM_FATAL_IF(
        "Index("<<index <<") must be in vector range [0:" <<m_indicator.size()-1 <<"]",
        index < 0 || index >= m_count);
    m_indicator.at(index)->setStyleSheet("background-color: orange; border-radius: "+
                                         QString(m_isCircular ? "10px;" : "0")+";");
    m_indicator.at(index)->setToolTip(QString::fromStdString(m_orangeTooltip));
}

//------------------------------------------------------------------------------

void SStatus::intToggleGreenRed(const int index, const bool green)
{
    OSLM_FATAL_IF(
        "Index("<<index <<") must be in vector range [0:" <<m_indicator.size()-1 <<"]",
        index < 0 || index >= m_count);
    m_indicator.at(index)->setStyleSheet("background-color:"+ QString(green ? "green" : "red")+"; border-radius: "+
                                         QString(m_isCircular ? "10px;" : "0")+";");
    m_indicator.at(index)->setToolTip(green ? QString::fromStdString(m_greenTooltip) : QString::fromStdString(
                                          m_redTooltip));
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiTools
