/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/editor/SStatus.hpp"

#include <fwCom/Slots.hpp>
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

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiTools::editor::SStatus );

const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_GREEN_SLOT      = "changeToGreen";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_RED_SLOT        = "changeToRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_ORANGE_SLOT     = "changeToOrange";
const ::fwCom::Slots::SlotKeyType SStatus::s_TOGGLE_GREEN_RED_SLOT     = "toggleGreenRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_NTH_TO_GREEN_SLOT  = "changeNthToGreen";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_NTH_TO_RED_SLOT    = "changeNthToRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_NTH_TO_ORANGE_SLOT = "changeNthToOrange";
const ::fwCom::Slots::SlotKeyType SStatus::s_TOGGLE_NTH_GREEN_RED_SLOT = "toggleNthGreenRed";

//-----------------------------------------------------------------------------

SStatus::SStatus() noexcept :
    m_count(1),
    m_isCircular(false),
    m_width(20),
    m_height(20)
{
    newSlot(s_CHANGE_TO_GREEN_SLOT, &SStatus::changeToGreen, this);
    newSlot(s_CHANGE_TO_RED_SLOT, &SStatus::changeToRed, this);
    newSlot(s_CHANGE_TO_ORANGE_SLOT, &SStatus::changeToOrange, this);
    newSlot(s_TOGGLE_GREEN_RED_SLOT, &SStatus::toggleGreenRed, this);
    newSlot(s_CHANGE_NTH_TO_GREEN_SLOT,  &SStatus::changeNthToGreen, this);
    newSlot(s_CHANGE_NTH_TO_RED_SLOT,    &SStatus::changeNthToRed, this);
    newSlot(s_CHANGE_NTH_TO_ORANGE_SLOT, &SStatus::changeNthToOrange, this);
    newSlot(s_TOGGLE_NTH_GREEN_RED_SLOT, &SStatus::toggleNthGreenRed, this);
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
        indicator->setFixedWidth(static_cast<int>(m_width));
        indicator->setFixedHeight(static_cast<int>(m_height));
        m_indicator.push_back(indicator);

        layout->addWidget(indicator);
        layout->addWidget(m_labelStatus.at(static_cast<int>(i)));
    }

    qtContainer->setLayout(layout);

    this->changeToRed();
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

    const auto config = this->getConfigTree();

    m_greenTooltip  = config.get<std::string>("green", "");
    m_redTooltip    = config.get<std::string>("red", "");
    m_orangeTooltip = config.get<std::string>("orange", "");
    m_layout        = config.get<std::string>("layout", "horizontal");
    SLM_ASSERT("Value for element 'layout' should be 'horizontal' or 'vertical'.",
               m_layout == "horizontal" || m_layout == "vertical");

    const std::string form = config.get<std::string>("form", "square");
    SLM_ASSERT("Value for element 'form' should be 'circle' or 'square'.", form == "circle" || form == "square");
    m_isCircular = (form == "circle");

    m_width  = config.get<size_t>("size.width", 20);
    m_height = config.get<size_t>("size.height", 20);

    m_count = config.get<size_t>("count", 1);

    if(m_count == 1)
    {
        const std::string label  = config.get<std::string>("labelStatus", "");
        QPointer < QLabel > qLab = new QLabel();
        qLab->setText(QString::fromStdString(label));
        m_labelStatus.push_back(qLab);
    }
    else
    {
        const auto configLabels = config.get_child_optional("labels");
        // Check if the labels tag exists
        if(configLabels.is_initialized())
        {
            const size_t countLabelStatus = configLabels.get().count("labelStatus");

            OSLM_WARN_IF(
                "Number of 'labelStatus' (" << countLabelStatus << ") is different from needed status (" << m_count << ").",
                    countLabelStatus != m_count);
            OSLM_WARN_IF("'labelStatus' from " << m_count+1 << " to " << countLabelStatus << " will be lost.",
                         countLabelStatus > m_count);

            const auto labelStatusConfig = configLabels.get().equal_range("labelStatus");
            // Fill the labelStatus vector
            BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type& v, labelStatusConfig)
            {
                const std::string label  = v.second.get<std::string>("");
                QPointer < QLabel > qLab = new QLabel();
                qLab->setText(QString::fromStdString(label));

                m_labelStatus.push_back(qLab);
            }

            // If there is less labelStatus than needed ones, fill with empty strings
            if(countLabelStatus < m_count)
            {
                OSLM_WARN("'labelStatus' from " << countLabelStatus+1 << " to " << m_count << " will be empty.");
                for(size_t i = countLabelStatus; i < m_count; ++i)
                {
                    QPointer < QLabel > qLab = new QLabel();
                    qLab->setText("");
                    m_labelStatus.push_back(qLab);
                }
            }
        }
        else
        {
            for(size_t i = 0; i < m_count; ++i)
            {
                QPointer < QLabel > qLab = new QLabel();
                qLab->setText("");
                m_labelStatus.push_back(qLab);
            }
        }
    }
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
    for(QVector< QPointer< QLabel > >::Iterator it = m_indicator.begin(); it != m_indicator.end(); ++it)
    {
        (*it)->setStyleSheet("background-color: green; border-radius: "+QString(
                                 m_isCircular ? "10px;" : "0")+";");
        (*it)->setToolTip(QString::fromStdString(m_greenTooltip));
    }
}

//------------------------------------------------------------------------------

void SStatus::changeToRed()
{
    for(QVector< QPointer< QLabel > >::Iterator it = m_indicator.begin(); it != m_indicator.end(); ++it)
    {
        (*it)->setStyleSheet("background-color: red; border-radius: "+QString(m_isCircular ? "10px;" : "0")+
                             ";");
        (*it)->setToolTip(QString::fromStdString(m_redTooltip));
    }
}

//------------------------------------------------------------------------------

void SStatus::changeToOrange()
{
    for(QVector< QPointer< QLabel > >::Iterator it = m_indicator.begin(); it != m_indicator.end(); ++it)
    {
        (*it)->setStyleSheet("background-color: orange; border-radius: "+QString(
                                 m_isCircular ? "10px;" : "0")+";");
        (*it)->setToolTip(QString::fromStdString(m_orangeTooltip));
    }
}

//------------------------------------------------------------------------------

void SStatus::toggleGreenRed(const bool green)
{
    for(QVector< QPointer< QLabel > >::Iterator it = m_indicator.begin(); it != m_indicator.end(); ++it)
    {
        (*it)->setStyleSheet("background-color:"+ QString(green ? "green" : "red")+"; border-radius: "+
                             QString(m_isCircular ? "10px;" : "0")+";");
        (*it)->setToolTip(green ? QString::fromStdString(m_greenTooltip) : QString::fromStdString(m_redTooltip));
    }
}

//------------------------------------------------------------------------------

void SStatus::changeNthToGreen(const int index)
{
    OSLM_INFO_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
            index < 0 || index >= m_count);

    if(index >= 0 && index < m_count)
    {
        m_indicator.at(index)->setStyleSheet("background-color: green; border-radius: "
                                             + QString(m_isCircular ? "10px;" : "0") + ";");
        m_indicator.at(index)->setToolTip(QString::fromStdString(m_greenTooltip));
    }
}

//------------------------------------------------------------------------------

void SStatus::changeNthToRed(const int index)
{
    OSLM_INFO_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 <<"]",
            index < 0 || index >= m_count);

    if(index >= 0 && index < m_count)
    {
        m_indicator.at(index)->setStyleSheet("background-color: red; border-radius: "
                                             + QString(m_isCircular ? "10px;" : "0") + ";");
        m_indicator.at(index)->setToolTip(QString::fromStdString(m_redTooltip));
    }
}

//------------------------------------------------------------------------------

void SStatus::changeNthToOrange(const int index)
{
    OSLM_INFO_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
            index < 0 || index >= m_count);

    if(index >= 0 && index < m_count)
    {
        m_indicator.at(index)->setStyleSheet("background-color: orange; border-radius: "
                                             + QString(m_isCircular ? "10px;" : "0") + ";");
        m_indicator.at(index)->setToolTip(QString::fromStdString(m_orangeTooltip));
    }
}

//------------------------------------------------------------------------------

void SStatus::toggleNthGreenRed(const int index, const bool green)
{
    OSLM_INFO_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 <<"]",
            index < 0 || index >= m_count);

    if(index >= 0 && index < m_count)
    {
        m_indicator.at(index)->setStyleSheet("background-color:"+ QString(green ? "green" : "red")
                                             + "; border-radius: " + QString(m_isCircular ? "10px;" : "0") + ";");
        m_indicator.at(index)->setToolTip(green ? QString::fromStdString(m_greenTooltip)
                                          : QString::fromStdString(m_redTooltip));
    }
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiTools
