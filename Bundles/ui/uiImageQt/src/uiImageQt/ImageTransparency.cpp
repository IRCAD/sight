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

#include "uiImageQt/ImageTransparency.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace uiImageQt
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiImageQt::ImageTransparency, ::fwData::Image );

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

ImageTransparency::ImageTransparency() noexcept
{
}

//------------------------------------------------------------------------------

ImageTransparency::~ImageTransparency() noexcept
{
}

//------------------------------------------------------------------------------

void ImageTransparency::starting()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QHBoxLayout* hLayout = new QHBoxLayout();

    QLabel* staticText = new QLabel( QObject::tr("Transparency: "));
    hLayout->addWidget( staticText, 0, Qt::AlignVCenter );

    m_valueSlider = new QSlider( Qt::Horizontal );
    hLayout->addWidget( m_valueSlider, 1, Qt::AlignVCenter );
    m_valueSlider->setRange(0, 100);
    m_valueSlider->setMinimumWidth(100);

    m_valueCheckBox = new QCheckBox( QObject::tr("visible"));
    m_action        = new QAction(m_valueCheckBox);
    m_action->setCheckable(true);
    if (!m_shortcut.empty())
    {
        m_action->setShortcut(QKeySequence(QString::fromStdString(m_shortcut)));
    }
    m_valueCheckBox->addAction(m_action);
    hLayout->addWidget( m_valueCheckBox, 0, Qt::AlignVCenter );

    qtContainer->setLayout( hLayout );

    QObject::connect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
    QObject::connect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
    QObject::connect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));

    this->updating();
}

//------------------------------------------------------------------------------

void ImageTransparency::stopping()
{
    SLM_TRACE_FUNC();
    QObject::disconnect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
    QObject::disconnect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
    QObject::disconnect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));

    this->destroy();
}

//------------------------------------------------------------------------------

void ImageTransparency::configuring()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();

    //<shortcut value="X"/>
    std::vector < ConfigurationType > vectCfg = m_configuration->find("shortcut");
    if(!vectCfg.empty())
    {
        ConfigurationType config = vectCfg.at(0);
        SLM_ASSERT("Missing attribute value", config->hasAttribute("value"));
        m_shortcut = config->getAttributeValue("value");
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::updating()
{
    ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", img);

    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( img );
    m_valueSlider->setEnabled(imageIsValid);
    m_valueCheckBox->setEnabled(imageIsValid);
    if (imageIsValid)
    {
        QObject::disconnect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
        QObject::disconnect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
        QObject::disconnect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));

        if(img->getField( "TRANSPARENCY" ) )
        {
            ::fwData::Integer::sptr transparency = img->getField< ::fwData::Integer >( "TRANSPARENCY" );
            m_valueSlider->setValue( *transparency );
        }
        else
        {
            img->setField( "TRANSPARENCY", ::fwData::Integer::New(0) );
            m_valueSlider->setValue( 0 );
        }
        if(img->getField( "VISIBILITY" ) )
        {
            ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
            m_valueCheckBox->setChecked( *visible );
            m_action->setChecked(*visible);
        }
        else
        {
            img->setField( "VISIBILITY", ::fwData::Boolean::New(true) );
            m_valueCheckBox->setChecked( true );
            m_action->setChecked(true);
        }
        QObject::connect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
        QObject::connect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
        QObject::connect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void ImageTransparency::info( std::ostream& _sstream )
{
    _sstream << "Image Features Editor";
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyTransparency(int value)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", img);

    img->setField( "TRANSPARENCY",  ::fwData::Integer::New(value) );

    auto sig = img->signal< ::fwData::Image::TransparencyModifiedSignalType >(
        ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyVisibility(bool value)
{
    SLM_TRACE_FUNC();
    m_valueCheckBox->setCheckState(value ? Qt::Checked : Qt::Unchecked);
    this->notifyVisibility(value);
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyVisibility(int value)
{
    SLM_TRACE_FUNC();
    m_action->setChecked(value == Qt::Checked);

    this->notifyVisibility(value == Qt::Checked);
}

//------------------------------------------------------------------------------

void ImageTransparency::notifyVisibility(bool isVisible)
{
    ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", img);

    img->setField( "VISIBILITY",  ::fwData::Boolean::New(isVisible) );

    auto sig = img->signal< ::fwData::Image::VisibilityModifiedSignalType >(::fwData::Image::s_VISIBILITY_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(isVisible);
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap ImageTransparency::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

}
