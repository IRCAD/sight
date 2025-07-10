/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include "module/ui/qt/reconstruction/organ_material_editor.hpp"

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
    connections.push(RECONSTRUCTION, data::object::MODIFIED_SIG, service::slots::UPDATE);
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

    const QString service_id = QString::fromStdString(base_id());

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(service_id);

    m_diffuse_colour_button = new QPushButton(tr("Diffuse"));
    m_diffuse_colour_button->setObjectName(service_id + "/" + m_diffuse_colour_button->text());
    m_diffuse_colour_button->setToolTip(tr("Selected organ's diffuse color"));
    m_diffuse_colour_button->setMinimumSize(m_diffuse_colour_button->sizeHint());

    m_ambient_colour_button = new QPushButton(tr("Ambient"));
    m_ambient_colour_button->setObjectName(service_id + "/" + m_ambient_colour_button->text());
    m_ambient_colour_button->setToolTip(tr("Selected organ's ambient color"));
    m_ambient_colour_button->setMinimumSize(m_ambient_colour_button->sizeHint());

    const char* transparency       = "Transparency";
    auto* const transparency_label = new QLabel(tr((std::string(transparency) + " : ").c_str()));
    m_opacity_slider = new QSlider(Qt::Horizontal);
    m_opacity_slider->setObjectName(service_id + "/" + transparency);
    m_opacity_slider->setToolTip(tr("Selected organ's opacity"));
    m_opacity_slider->setRange(0, 100);
    m_opacity_slider->setTickInterval(20);
    m_opacity_slider->setTickPosition(QSlider::TicksBelow);
    m_opacity_slider->setMinimumSize(m_opacity_slider->sizeHint());

    m_transparency_value = new QLabel("");
    m_transparency_value->setObjectName(service_id + "/transparencyValue");
    m_transparency_value->setMinimumSize(m_transparency_value->sizeHint());

    auto* const main_layout = new QVBoxLayout();

    auto* const button_layout = new QHBoxLayout();
    button_layout->addWidget(m_diffuse_colour_button, 0);
    button_layout->addWidget(m_ambient_colour_button, 0);
    main_layout->addLayout(button_layout, 0);

    auto* const transparency_layout = new QHBoxLayout();
    transparency_layout->addWidget(transparency_label, 0);
    transparency_layout->addWidget(m_opacity_slider, 1);
    transparency_layout->addWidget(m_transparency_value, 0);
    main_layout->addLayout(transparency_layout, 0);

    qt_container->set_layout(main_layout);
    qt_container->set_enabled(false);

    QObject::connect(m_opacity_slider, &QSlider::valueChanged, this, &self_t::on_opacity_slider);
    QObject::connect(m_diffuse_colour_button, &QPushButton::clicked, this, &self_t::on_diffuse_color_button);
    QObject::connect(m_ambient_colour_button, &QPushButton::clicked, this, &self_t::on_ambient_color_button);

    this->updating();
}

//------------------------------------------------------------------------------

void organ_material_editor::updating()
{
    this->refresh_material();
}

//------------------------------------------------------------------------------

void organ_material_editor::stopping()
{
    QObject::disconnect(m_opacity_slider, &QSlider::valueChanged, this, &self_t::on_opacity_slider);
    QObject::disconnect(m_diffuse_colour_button, &QPushButton::clicked, this, &self_t::on_diffuse_color_button);
    QObject::disconnect(m_ambient_colour_button, &QPushButton::clicked, this, &self_t::on_ambient_color_button);

    this->destroy();
}

//------------------------------------------------------------------------------

void organ_material_editor::on_diffuse_color_button()
{
    data::material::sptr material;
    {
        SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        material = reconstruction->get_material();
    }

    bool needrefresh = false;
    {
        data::mt::locked_ptr<data::material> lock(material);

        int red   = static_cast<int>(material->diffuse()->red() * 255);
        int green = static_cast<int>(material->diffuse()->green() * 255);
        int blue  = static_cast<int>(material->diffuse()->blue() * 255);

        // Create Color choice dialog.
        auto qt_container =
            std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
        QWidget* const container = qt_container->get_qt_container();
        SIGHT_ASSERT("container not instanced", container);

        const QColor old_color(red, green, blue);
        const QColor color = QColorDialog::getColor(old_color, container);
        if(color.isValid())
        {
            material->diffuse()->red()   = static_cast<float>(color.redF());
            material->diffuse()->green() = static_cast<float>(color.greenF());
            material->diffuse()->blue()  = static_cast<float>(color.blueF());
            this->material_notification();
            needrefresh = true;
        }
    }

    if(needrefresh)
    {
        refresh_material();
    }
}

//------------------------------------------------------------------------------

void organ_material_editor::on_ambient_color_button()
{
    data::material::sptr material;
    {
        SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        material = reconstruction->get_material();
    }

    bool needrefresh = false;
    {
        data::mt::locked_ptr<data::material> lock(material);

        const int red   = static_cast<int>(material->ambient()->red() * 255.F);
        const int green = static_cast<int>(material->ambient()->green() * 255.F);
        const int blue  = static_cast<int>(material->ambient()->blue() * 255.F);

        // Create Color choice dialog.
        auto qt_container =
            std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
        QWidget* const container = qt_container->get_qt_container();
        SIGHT_ASSERT("container not instanced", container);

        const QColor old_color(red, green, blue);
        const QColor color = QColorDialog::getColor(old_color, container);
        if(color.isValid())
        {
            material->ambient()->red()   = static_cast<float>(color.redF());
            material->ambient()->green() = static_cast<float>(color.greenF());
            material->ambient()->blue()  = static_cast<float>(color.blueF());
            this->material_notification();
            needrefresh = true;
        }
    }

    if(needrefresh)
    {
        refresh_material();
    }
}

//------------------------------------------------------------------------------

void organ_material_editor::on_opacity_slider(int _value)
{
    data::material::sptr material;
    {
        SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        material = reconstruction->get_material();
    }

    data::mt::locked_ptr<data::material> lock(material);

    material->diffuse()->alpha() = static_cast<float>(_value) / 100.F;
    std::stringstream ss;
    ss << _value << "%";
    m_transparency_value->setText(QString::fromStdString(ss.str()));

    this->material_notification();
}

//------------------------------------------------------------------------------

void organ_material_editor::refresh_material()
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    QWidget* const container = qt_container->get_qt_container();
    SIGHT_ASSERT("container not instanced", container);

    data::material::csptr material;
    {
        SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
        auto reconstruction = m_rec.lock();

        container->setEnabled(!reconstruction->get_organ_name().empty());
        material = reconstruction->get_material();
    }

    int alpha = 0;
    {
        data::mt::locked_ptr<const data::material> lock(material);

        {
            const QColor material_diffuse_color = QColor(
                static_cast<int>(material->diffuse()->red() * 255.F),
                static_cast<int>(material->diffuse()->green() * 255.F),
                static_cast<int>(material->diffuse()->blue() * 255.F),
                static_cast<int>(material->diffuse()->alpha() * 255.F)
            );

            const int icon_size = m_diffuse_colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
            QPixmap pix(icon_size, icon_size);
            pix.fill(material_diffuse_color);
            m_diffuse_colour_button->setIcon(QIcon(pix));
        }

        {
            const QColor material_ambient_color = QColor(
                static_cast<int>(material->ambient()->red() * 255.F),
                static_cast<int>(material->ambient()->green() * 255.F),
                static_cast<int>(material->ambient()->blue() * 255.F),
                static_cast<int>(material->ambient()->alpha() * 255.F)
            );

            const int icon_size = m_ambient_colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
            QPixmap pix(icon_size, icon_size);
            pix.fill(material_ambient_color);
            m_ambient_colour_button->setIcon(QIcon(pix));
        }

        alpha = static_cast<int>(material->diffuse()->alpha() * 100.F);
    }

    m_opacity_slider->setValue(alpha);
    std::stringstream ss;
    ss << alpha << "%";
    m_transparency_value->setText(QString::fromStdString(ss.str()));
}

//------------------------------------------------------------------------------

void organ_material_editor::material_notification()
{
    SIGHT_ASSERT("The inout key '" << RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    data::object::modified_signal_t::sptr sig =
        reconstruction->get_material()->signal<data::object::modified_signal_t>(
            data::object::MODIFIED_SIG
        );
    sig->async_emit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::reconstruction
