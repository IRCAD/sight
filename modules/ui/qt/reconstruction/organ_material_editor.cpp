/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/ui/qt/reconstruction/organ_material_editor.hpp"

#include <core/runtime/path.hpp>

#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/reconstruction.hpp>

#include <service/op.hpp>

#include <ui/qt/container/widget.hpp>

#include <QColor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSlider>
#include <QStyle>
#include <QVBoxLayout>

namespace sight::module::ui::qt::reconstruction
{

organ_material_editor::organ_material_editor() noexcept =
    default;

//------------------------------------------------------------------------------

organ_material_editor::~organ_material_editor() noexcept =
    default;

//------------------------------------------------------------------------------

service::connections_t organ_material_editor::auto_connections() const
{
    connections_t connections;
    connections.push(s_RECONSTRUCTION, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

void organ_material_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void organ_material_editor::starting()
{
    this->create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    qtContainer->getQtContainer()->setObjectName(serviceID);

    m_diffuseColourButton = new QPushButton(tr("Diffuse"));
    m_diffuseColourButton->setObjectName(serviceID + "/" + m_diffuseColourButton->text());
    m_diffuseColourButton->setToolTip(tr("Selected organ's diffuse color"));
    m_diffuseColourButton->setMinimumSize(m_diffuseColourButton->sizeHint());

    m_ambientColourButton = new QPushButton(tr("Ambient"));
    m_ambientColourButton->setObjectName(serviceID + "/" + m_ambientColourButton->text());
    m_ambientColourButton->setToolTip(tr("Selected organ's ambient color"));
    m_ambientColourButton->setMinimumSize(m_ambientColourButton->sizeHint());

    const char* transparency      = "Transparency";
    auto* const transparencyLabel = new QLabel(tr((std::string(transparency) + " : ").c_str()));
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setObjectName(serviceID + "/" + transparency);
    m_opacitySlider->setToolTip(tr("Selected organ's opacity"));
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setTickInterval(20);
    m_opacitySlider->setTickPosition(QSlider::TicksBelow);
    m_opacitySlider->setMinimumSize(m_opacitySlider->sizeHint());

    m_transparencyValue = new QLabel("");
    m_transparencyValue->setObjectName(serviceID + "/transparencyValue");
    m_transparencyValue->setMinimumSize(m_transparencyValue->sizeHint());

    auto* const mainLayout = new QVBoxLayout();

    auto* const buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_diffuseColourButton, 0);
    buttonLayout->addWidget(m_ambientColourButton, 0);
    mainLayout->addLayout(buttonLayout, 0);

    auto* const transparencyLayout = new QHBoxLayout();
    transparencyLayout->addWidget(transparencyLabel, 0);
    transparencyLayout->addWidget(m_opacitySlider, 1);
    transparencyLayout->addWidget(m_transparencyValue, 0);
    mainLayout->addLayout(transparencyLayout, 0);

    qtContainer->setLayout(mainLayout);
    qtContainer->setEnabled(false);

    QObject::connect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySlider(int)));
    QObject::connect(m_diffuseColourButton, SIGNAL(clicked()), this, SLOT(onDiffuseColorButton()));
    QObject::connect(m_ambientColourButton, SIGNAL(clicked()), this, SLOT(onAmbientColorButton()));

    this->updating();
}

//------------------------------------------------------------------------------

void organ_material_editor::updating()
{
    this->refreshMaterial();
}

//------------------------------------------------------------------------------

void organ_material_editor::stopping()
{
    QObject::disconnect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySlider(int)));
    QObject::disconnect(m_diffuseColourButton, SIGNAL(clicked()), this, SLOT(onDiffuseColorButton()));
    QObject::disconnect(m_ambientColourButton, SIGNAL(clicked()), this, SLOT(onAmbientColorButton()));

    this->destroy();
}

//------------------------------------------------------------------------------

void organ_material_editor::onDiffuseColorButton()
{
    data::material::sptr material;
    {
        SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        material = reconstruction->getMaterial();
    }

    bool needrefresh = false;
    {
        data::mt::locked_ptr<data::material> lock(material);

        int red   = static_cast<int>(material->diffuse()->red() * 255);
        int green = static_cast<int>(material->diffuse()->green() * 255);
        int blue  = static_cast<int>(material->diffuse()->blue() * 255);

        // Create Color choice dialog.
        auto qtContainer =
            std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
        QWidget* const container = qtContainer->getQtContainer();
        SIGHT_ASSERT("container not instanced", container);

        const QColor oldColor(red, green, blue);
        const QColor color = QColorDialog::getColor(oldColor, container);
        if(color.isValid())
        {
            material->diffuse()->red()   = static_cast<float>(color.redF());
            material->diffuse()->green() = static_cast<float>(color.greenF());
            material->diffuse()->blue()  = static_cast<float>(color.blueF());
            this->materialNotification();
            needrefresh = true;
        }
    }

    if(needrefresh)
    {
        refreshMaterial();
    }
}

//------------------------------------------------------------------------------

void organ_material_editor::onAmbientColorButton()
{
    data::material::sptr material;
    {
        SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        material = reconstruction->getMaterial();
    }

    bool needrefresh = false;
    {
        data::mt::locked_ptr<data::material> lock(material);

        const int red   = static_cast<int>(material->ambient()->red() * 255.F);
        const int green = static_cast<int>(material->ambient()->green() * 255.F);
        const int blue  = static_cast<int>(material->ambient()->blue() * 255.F);

        // Create Color choice dialog.
        auto qtContainer =
            std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
        QWidget* const container = qtContainer->getQtContainer();
        SIGHT_ASSERT("container not instanced", container);

        const QColor oldColor(red, green, blue);
        const QColor color = QColorDialog::getColor(oldColor, container);
        if(color.isValid())
        {
            material->ambient()->red()   = static_cast<float>(color.redF());
            material->ambient()->green() = static_cast<float>(color.greenF());
            material->ambient()->blue()  = static_cast<float>(color.blueF());
            this->materialNotification();
            needrefresh = true;
        }
    }

    if(needrefresh)
    {
        refreshMaterial();
    }
}

//------------------------------------------------------------------------------

void organ_material_editor::onOpacitySlider(int _value)
{
    data::material::sptr material;
    {
        SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        material = reconstruction->getMaterial();
    }

    data::mt::locked_ptr<data::material> lock(material);

    material->diffuse()->alpha() = static_cast<float>(_value) / 100.F;
    std::stringstream ss;
    ss << _value << "%";
    m_transparencyValue->setText(QString::fromStdString(ss.str()));

    this->materialNotification();
}

//------------------------------------------------------------------------------

void organ_material_editor::refreshMaterial()
{
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* const container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    data::material::csptr material;
    {
        SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        container->setEnabled(!reconstruction->getOrganName().empty());
        material = reconstruction->getMaterial();
    }

    int alpha = 0;
    {
        data::mt::locked_ptr<const data::material> lock(material);

        {
            const QColor materialDiffuseColor = QColor(
                static_cast<int>(material->diffuse()->red() * 255.F),
                static_cast<int>(material->diffuse()->green() * 255.F),
                static_cast<int>(material->diffuse()->blue() * 255.F),
                static_cast<int>(material->diffuse()->alpha() * 255.F)
            );

            const int iconSize = m_diffuseColourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
            QPixmap pix(iconSize, iconSize);
            pix.fill(materialDiffuseColor);
            m_diffuseColourButton->setIcon(QIcon(pix));
        }

        {
            const QColor materialAmbientColor = QColor(
                static_cast<int>(material->ambient()->red() * 255.F),
                static_cast<int>(material->ambient()->green() * 255.F),
                static_cast<int>(material->ambient()->blue() * 255.F),
                static_cast<int>(material->ambient()->alpha() * 255.F)
            );

            const int iconSize = m_ambientColourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
            QPixmap pix(iconSize, iconSize);
            pix.fill(materialAmbientColor);
            m_ambientColourButton->setIcon(QIcon(pix));
        }

        alpha = static_cast<int>(material->diffuse()->alpha() * 100.F);
    }

    m_opacitySlider->setValue(alpha);
    std::stringstream ss;
    ss << alpha << "%";
    m_transparencyValue->setText(QString::fromStdString(ss.str()));
}

//------------------------------------------------------------------------------

void organ_material_editor::materialNotification()
{
    SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    data::object::ModifiedSignalType::sptr sig =
        reconstruction->getMaterial()->signal<data::object::ModifiedSignalType>(
            data::object::MODIFIED_SIG
        );
    sig->async_emit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::reconstruction
