/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/qt/image/ImageTransparency.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/Boolean.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>

#include <geometry/data/IntrasecTypes.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace sight::module::ui::qt::image
{

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
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );

    QHBoxLayout* hLayout = new QHBoxLayout();

    QLabel* staticText = new QLabel(QObject::tr("Transparency: "));
    hLayout->addWidget(staticText, 0, Qt::AlignVCenter);

    m_valueSlider = new QSlider(Qt::Horizontal);
    hLayout->addWidget(m_valueSlider, 1, Qt::AlignVCenter);
    m_valueSlider->setRange(0, 100);
    m_valueSlider->setMinimumWidth(100);

    m_valueCheckBox = new QCheckBox(QObject::tr("visible"));
    m_action        = new QAction(m_valueCheckBox);
    m_action->setCheckable(true);
    if(!m_shortcut.empty())
    {
        m_action->setShortcut(QKeySequence(QString::fromStdString(m_shortcut)));
    }

    m_valueCheckBox->addAction(m_action);
    hLayout->addWidget(m_valueCheckBox, 0, Qt::AlignVCenter);

    qtContainer->setLayout(hLayout);

    QObject::connect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
    QObject::connect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
    QObject::connect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));

    this->updating();
}

//------------------------------------------------------------------------------

void ImageTransparency::stopping()
{
    QObject::disconnect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
    QObject::disconnect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
    QObject::disconnect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));

    this->destroy();
}

//------------------------------------------------------------------------------

void ImageTransparency::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();

    //<shortcut value="X"/>
    std::vector<ConfigurationType> vectCfg = m_configuration->find("shortcut");
    if(!vectCfg.empty())
    {
        ConfigurationType config = vectCfg.at(0);
        SIGHT_ASSERT("Missing attribute value", config->hasAttribute("value"));
        m_shortcut = config->getAttributeValue("value");
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::updating()
{
    const auto img = m_image.lock();
    SIGHT_ASSERT("The input '" << s_IMAGE << "' is not defined", img);

    bool imageIsValid = data::fieldHelper::MedicalImageHelpers::checkImageValidity(img.get_shared());
    m_valueSlider->setEnabled(imageIsValid);
    m_valueCheckBox->setEnabled(imageIsValid);
    if(imageIsValid)
    {
        QObject::disconnect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
        QObject::disconnect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
        QObject::disconnect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));

        if(img->getField("TRANSPARENCY"))
        {
            data::Integer::sptr transparency = img->getField<data::Integer>("TRANSPARENCY");
            m_valueSlider->setValue(static_cast<int>(*transparency));
        }
        else
        {
            img->setField("TRANSPARENCY", data::Integer::New(0));
            m_valueSlider->setValue(0);
        }

        if(img->getField("VISIBILITY"))
        {
            data::Boolean::sptr visible = img->getField<data::Boolean>("VISIBILITY");
            m_valueCheckBox->setChecked(*visible);
            m_action->setChecked(*visible);
        }
        else
        {
            img->setField("VISIBILITY", data::Boolean::New(true));
            m_valueCheckBox->setChecked(true);
            m_action->setChecked(true);
        }

        QObject::connect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
        QObject::connect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
        QObject::connect(m_action, SIGNAL(triggered(bool)), this, SLOT(onModifyVisibility(bool)));
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::info(std::ostream& _sstream)
{
    _sstream << "Image Features Editor";
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyTransparency(int value)
{
    const auto img = m_image.lock();
    SIGHT_ASSERT("The input '" << s_IMAGE << "' is not defined", img);

    img->setField("TRANSPARENCY", data::Integer::New(value));

    auto sig = img->signal<data::Image::TransparencyModifiedSignalType>(
        data::Image::s_TRANSPARENCY_MODIFIED_SIG
    );
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyVisibility(bool value)
{
    m_valueCheckBox->setCheckState(value ? Qt::Checked : Qt::Unchecked);
    this->notifyVisibility(value);
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyVisibility(int value)
{
    m_action->setChecked(value == Qt::Checked);

    this->notifyVisibility(value == Qt::Checked);
}

//------------------------------------------------------------------------------

void ImageTransparency::notifyVisibility(bool isVisible)
{
    const auto img = m_image.lock();
    SIGHT_ASSERT("The input '" << s_IMAGE << "' is not defined", img);

    img->setField("VISIBILITY", data::Boolean::New(isVisible));

    auto sig = img->signal<data::Image::VisibilityModifiedSignalType>(data::Image::s_VISIBILITY_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(isVisible);
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap ImageTransparency::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE, data::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE, data::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module
