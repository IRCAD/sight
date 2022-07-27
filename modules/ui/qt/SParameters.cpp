/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SParameters.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/tools/Object.hpp>

#include <data/tools/Color.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <QColorDialog>
#include <QEvent>
#include <QFormLayout>
#include <QSpinBox>
#include <QString>
#include <QStyle>

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

static const core::com::Signals::SignalKeyType BOOLEAN_CHANGED_SIG    = "boolChanged";
static const core::com::Signals::SignalKeyType COLOR_CHANGED_SIG      = "colorChanged";
static const core::com::Signals::SignalKeyType DOUBLE_CHANGED_SIG     = "doubleChanged";
static const core::com::Signals::SignalKeyType DOUBLE2_CHANGED_SIG    = "double2Changed";
static const core::com::Signals::SignalKeyType DOUBLE3_CHANGED_SIG    = "double3Changed";
static const core::com::Signals::SignalKeyType INTEGER_CHANGED_SIG    = "intChanged";
static const core::com::Signals::SignalKeyType INTEGER2_CHANGED_SIG   = "int2Changed";
static const core::com::Signals::SignalKeyType INTEGER3_CHANGED_SIG   = "int3Changed";
static const core::com::Signals::SignalKeyType ENUM_CHANGED_SIG       = "enumChanged";
static const core::com::Signals::SignalKeyType ENUM_INDEX_CHANGED_SIG = "enumIndexChanged";

static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT       = "setBoolParameter";
static const core::com::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT      = "setColorParameter";
static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT     = "setDoubleParameter";
static const core::com::Slots::SlotKeyType s_SET_DOUBLE2_PARAMETER_SLOT    = "setDouble2Parameter";
static const core::com::Slots::SlotKeyType s_SET_DOUBLE3_PARAMETER_SLOT    = "setDouble3Parameter";
static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT        = "setIntParameter";
static const core::com::Slots::SlotKeyType s_SET_INT2_PARAMETER_SLOT       = "setInt2Parameter";
static const core::com::Slots::SlotKeyType s_SET_INT3_PARAMETER_SLOT       = "setInt3Parameter";
static const core::com::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT       = "setEnumParameter";
static const core::com::Slots::SlotKeyType s_SET_ENUM_INDEX_PARAMETER_SLOT = "setEnumIndexParameter";
static const core::com::Slots::SlotKeyType s_SET_ENUM_VALUES_SLOT          = "setEnumValues";
static const core::com::Slots::SlotKeyType s_SET_INT_MIN_PARAMETER_SLOT    = "setIntMinParameter";
static const core::com::Slots::SlotKeyType s_SET_INT_MAX_PARAMETER_SLOT    = "setIntMaxParameter";
static const core::com::Slots::SlotKeyType s_SET_DOUBLE_MIN_PARAMETER_SLOT = "setDoubleMinParameter";
static const core::com::Slots::SlotKeyType s_SET_DOUBLE_MAX_PARAMETER_SLOT = "setDoubleMaxParameter";

//-----------------------------------------------------------------------------

SParameters::SParameters() noexcept :
    m_blockSignals(false)
{
    newSignal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG);
    newSignal<ColorChangedSignalType>(COLOR_CHANGED_SIG);
    newSignal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG);
    newSignal<Double2ChangedSignalType>(DOUBLE2_CHANGED_SIG);
    newSignal<Double3ChangedSignalType>(DOUBLE3_CHANGED_SIG);
    newSignal<IntegerChangedSignalType>(INTEGER_CHANGED_SIG);
    newSignal<Integer2ChangedSignalType>(INTEGER2_CHANGED_SIG);
    newSignal<Integer3ChangedSignalType>(INTEGER3_CHANGED_SIG);
    newSignal<EnumChangedSignalType>(ENUM_CHANGED_SIG);
    newSignal<EnumChangedIndexSignalType>(ENUM_INDEX_CHANGED_SIG);

    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SParameters::setBoolParameter, this);
    newSlot(s_SET_COLOR_PARAMETER_SLOT, &SParameters::setColorParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SParameters::setDoubleParameter, this);
    newSlot(s_SET_DOUBLE2_PARAMETER_SLOT, &SParameters::setDouble2Parameter, this);
    newSlot(s_SET_DOUBLE3_PARAMETER_SLOT, &SParameters::setDouble3Parameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SParameters::setIntParameter, this);
    newSlot(s_SET_INT2_PARAMETER_SLOT, &SParameters::setInt2Parameter, this);
    newSlot(s_SET_INT3_PARAMETER_SLOT, &SParameters::setInt3Parameter, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SParameters::setEnumParameter, this);
    newSlot(s_SET_ENUM_INDEX_PARAMETER_SLOT, &SParameters::setEnumIndexParameter, this);
    newSlot(s_SET_ENUM_VALUES_SLOT, &SParameters::setEnumValues, this);
    newSlot(s_SET_INT_MIN_PARAMETER_SLOT, &SParameters::setIntMinParameter, this);
    newSlot(s_SET_INT_MAX_PARAMETER_SLOT, &SParameters::setIntMaxParameter, this);
    newSlot(s_SET_DOUBLE_MIN_PARAMETER_SLOT, &SParameters::setDoubleMinParameter, this);
    newSlot(s_SET_DOUBLE_MAX_PARAMETER_SLOT, &SParameters::setDoubleMaxParameter, this);
}

//-----------------------------------------------------------------------------

SParameters::~SParameters() noexcept
{
}

//-----------------------------------------------------------------------------

void SParameters::configuring()
{
    this->initialize();

    const ConfigType configTree = this->getConfigTree();
}

//-----------------------------------------------------------------------------

void SParameters::starting()
{
    this->create();

    const std::string serviceID = getID().substr(getID().find_last_of('_') + 1);

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    qtContainer->getQtContainer()->setObjectName(QString::fromStdString(serviceID));

    QGridLayout* layout = new QGridLayout();

    service::IService::ConfigType config               = this->getConfigTree();
    const service::IService::ConfigType& parametersCfg = config.get_child("parameters");

    int row = 0;

    this->blockSignals(true);

    // Create widgets
    BOOST_FOREACH(const auto& param, parametersCfg.equal_range("param"))
    {
        const service::IService::ConfigType& cfg = param.second;

        const std::string name         = cfg.get<std::string>("<xmlattr>.name");
        const std::string key          = cfg.get<std::string>("<xmlattr>.key");
        const std::string type         = cfg.get<std::string>("<xmlattr>.type");
        const std::string defaultValue = cfg.get<std::string>("<xmlattr>.defaultValue");
        const bool resetButton         = cfg.get<bool>("<xmlattr>.reset", true);

        if(!name.empty())
        {
            layout->addWidget(new QLabel(QString(name.c_str())), row, 0);
        }

        if(type == "bool")
        {
            this->createBoolWidget(*layout, row, key, defaultValue, resetButton);
        }
        else if(type == "color")
        {
            this->createColorWidget(*layout, row, key, defaultValue, resetButton);
        }
        else if(type == "double" || type == "double2" || type == "double3")
        {
            const std::string widget = cfg.get<std::string>("<xmlattr>.widget", "spin");

            const double min                = cfg.get<double>("<xmlattr>.min", 0.);
            const double max                = cfg.get<double>("<xmlattr>.max", 1.);
            const double defaultValueDouble = cfg.get<double>("<xmlattr>.defaultValue", 0.5);

            const int count = (type == "double3") ? 3 : (type == "double2" ? 2 : 1);

            if(widget == "spin")
            {
                this->createDoubleWidget(*layout, row, key, defaultValueDouble, min, max, count, resetButton);
            }
            else if(widget == "slider")
            {
                // We don't support multiple sliders because we will not have the room in a single row
                SIGHT_ASSERT("Count > 1 is not supported with sliders", count == 1);

                const std::uint8_t decimals = cfg.get<std::uint8_t>("<xmlattr>.decimals", 2);
                const bool onRelease        = cfg.get<bool>("<xmlattr>.emitOnRelease", false);
                this->createDoubleSliderWidget(
                    *layout,
                    row,
                    key,
                    defaultValueDouble,
                    min,
                    max,
                    decimals,
                    resetButton,
                    onRelease
                );
            }
            else
            {
                SIGHT_ERROR("Unknown widget type : '" + widget + "' for " + name + ". Must be 'spin' or 'slider'.");
            }
        }
        else if(type == "int" || type == "int2" || type == "int3")
        {
            const std::string widget = cfg.get<std::string>("<xmlattr>.widget", "slider");

            const int min                = cfg.get<int>("<xmlattr>.min", 0);
            const int max                = cfg.get<int>("<xmlattr>.max", 100);
            const int defaultValueDouble = cfg.get<int>("<xmlattr>.defaultValue", 50);

            const int count = (type == "int3") ? 3 : (type == "int2" ? 2 : 1);

            if(widget == "spin")
            {
                this->createIntegerSpinWidget(*layout, row, key, defaultValueDouble, min, max, count, resetButton);
            }
            else if(widget == "slider")
            {
                // We don't support multiple sliders because we will not have the room in a single row
                SIGHT_ASSERT("Count > 1 is not supported with sliders", count == 1);

                const bool onRelease = cfg.get<bool>("<xmlattr>.emitOnRelease", false);
                this->createIntegerSliderWidget(
                    *layout,
                    row,
                    key,
                    defaultValueDouble,
                    min,
                    max,
                    resetButton,
                    onRelease
                );
            }
            else
            {
                SIGHT_ERROR("Unknown widget type : '" + widget + "' for " + name + ". Must be 'spin' or 'slider'.");
            }
        }
        else if(type == "enum")
        {
            const std::string options = cfg.get<std::string>("<xmlattr>.values");

            //split values separated by ',', ' ', ';'
            std::vector<std::string> values;
            std::vector<std::string> data;

            this->parseEnumString(options, values, data);

            this->createEnumWidget(*layout, row, key, defaultValue, values, data);
        }

        ++row;
    }

    BOOST_FOREACH(const auto& param, parametersCfg.equal_range("param"))
    {
        const service::IService::ConfigType& cfg = param.second;

        const std::string key          = cfg.get<std::string>("<xmlattr>.key");
        const std::string depends      = cfg.get<std::string>("<xmlattr>.depends", "");
        const std::string dependsValue = cfg.get<std::string>("<xmlattr>.dependsValue", "");
        const bool dependsreverse      = cfg.get<bool>("<xmlattr>.dependsReverse", false);

        if(!depends.empty())
        {
            QWidget* widget = nullptr;
            for(int i = 0 ; i < layout->count() ; ++i)
            {
                widget = layout->itemAt(i)->widget();
                if(widget != nullptr)
                {
                    QString foundedKey = widget->property("key").toString();
                    if(key.compare(foundedKey.toStdString()) == 0)
                    {
                        break;
                    }
                }
            }

            QWidget* dependsWidget = nullptr;
            for(int i = 0 ; i < layout->count() ; ++i)
            {
                dependsWidget = layout->itemAt(i)->widget();
                if(dependsWidget != nullptr)
                {
                    QString foundedKey = dependsWidget->property("key").toString();
                    if(depends.compare(foundedKey.toStdString()) == 0)
                    {
                        break;
                    }
                }
            }

            widget->installEventFilter(this);
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(dependsWidget);
            if(checkBox != nullptr)
            {
                QObject::connect(
                    checkBox,
                    &QCheckBox::stateChanged,
                    this,
                    [ = ]
                    {
                        onDependsChanged(checkBox, widget, dependsreverse);
                    });
                onDependsChanged(checkBox, widget, dependsreverse);
            }
            else
            {
                QComboBox* comboBox = qobject_cast<QComboBox*>(dependsWidget);
                if(comboBox != nullptr)
                {
                    QObject::connect(
                        comboBox,
                        static_cast<void (QComboBox::*)(
                                        int
                                    )>(&QComboBox::currentIndexChanged),
                        this,
                        [ = ]
                        {
                            onDependsChanged(comboBox, widget, dependsValue, dependsreverse);
                        });
                    onDependsChanged(comboBox, widget, dependsValue, dependsreverse);
                }
            }
        }
    }

    qtContainer->setLayout(layout);

    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void SParameters::updating()
{
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* widget  = qtContainer->getQtContainer();

    service::IService::ConfigType config               = this->getConfigTree();
    const service::IService::ConfigType& parametersCfg = config.get_child("parameters");

    // emit signal for each widget
    BOOST_FOREACH(const auto& param, parametersCfg.equal_range("param"))
    {
        const service::IService::ConfigType& cfg = param.second;

        const std::string key  = cfg.get<std::string>("<xmlattr>.key");
        const std::string type = cfg.get<std::string>("<xmlattr>.type");

        QWidget* child = widget->findChild<QWidget*>(QString::fromStdString(key));
        if(child)
        {
            if(type == "bool")
            {
                const QCheckBox* box = dynamic_cast<QCheckBox*>(child);
                SIGHT_ASSERT("Widget must be a QCheckBox", box);

                const bool state = (box->checkState() == Qt::Checked);

                if(!m_blockSignals)
                {
                    this->signal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG)->asyncEmit(state, key);
                    SIGHT_DEBUG(
                        "[EMIT] " << BOOLEAN_CHANGED_SIG << "(" << (state ? "true" : "false") << ", " << key
                        << ")"
                    );
                }
            }
            else if(type == "color")
            {
                const QColor colorQt = child->property("color").value<QColor>();
                this->emitColorSignal(colorQt, key);
            }
            else if(type == "double" || type == "double2" || type == "double3")
            {
                this->emitDoubleSignal(child);
            }
            else if(type == "int" || type == "int2" || type == "int3")
            {
                this->emitIntegerSignal(child);
            }
            else if(type == "enum")
            {
                QComboBox* box = qobject_cast<QComboBox*>(child);

                SIGHT_ASSERT("Widget must be a QComboBox", box);

                const QString data = box->itemData(box->currentIndex()).toString();

                if(!m_blockSignals)
                {
                    this->signal<EnumChangedSignalType>(ENUM_CHANGED_SIG)->asyncEmit(data.toStdString(), key);
                    SIGHT_DEBUG("[EMIT] " << ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key << ")");
                    this->signal<EnumChangedIndexSignalType>(ENUM_INDEX_CHANGED_SIG)->asyncEmit(
                        box->currentIndex(),
                        key
                    );
                    SIGHT_DEBUG(
                        "[EMIT] " << ENUM_INDEX_CHANGED_SIG << "(" << box->currentIndex() << ", " << key
                        << ")"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

bool SParameters::eventFilter(QObject* _watched, QEvent* _event)
{
    if(_event->type() == ::QEvent::EnabledChange)
    {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(_watched);
        if(checkBox != nullptr)
        {
            checkBox->stateChanged(checkBox->isChecked() ? Qt::Checked : Qt::Unchecked);
        }
        else
        {
            QComboBox* comboBox = qobject_cast<QComboBox*>(_watched);
            if(comboBox != nullptr)
            {
                comboBox->currentIndexChanged(comboBox->currentIndex());
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------

void SParameters::onDependsChanged(QCheckBox* _checkBox, QWidget* _widget, bool _reverse)
{
    if(!_checkBox->isEnabled())
    {
        _widget->setDisabled(true);
    }
    else if(_reverse)
    {
        _widget->setDisabled(_checkBox->checkState());
    }
    else
    {
        _widget->setEnabled(_checkBox->checkState());
    }
}

//------------------------------------------------------------------------------

void SParameters::onDependsChanged(QComboBox* _comboBox, QWidget* _widget, const std::string& _value, bool _reverse)
{
    if(!_comboBox->isEnabled())
    {
        _widget->setDisabled(true);
    }
    else if(_reverse)
    {
        _widget->setDisabled(_comboBox->currentText().toStdString().compare(_value) == 0);
    }
    else
    {
        _widget->setEnabled(_comboBox->currentText().toStdString().compare(_value) == 0);
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeEnum(int value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();

    const QComboBox* box = dynamic_cast<const QComboBox*>(sender);

    SIGHT_ASSERT("Wrong widget type", box);

    const QString data = box->itemData(value).toString();

    if(!m_blockSignals)
    {
        this->signal<EnumChangedSignalType>(ENUM_CHANGED_SIG)->asyncEmit(data.toStdString(), key.toStdString());
        SIGHT_DEBUG("[EMIT] " << ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key.toStdString() << ")");
        this->signal<EnumChangedIndexSignalType>(ENUM_INDEX_CHANGED_SIG)->asyncEmit(value, key.toStdString());
        SIGHT_DEBUG("[EMIT] " << ENUM_INDEX_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeBoolean(int value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();
    const bool checked    = value == Qt::Checked;

    if(!m_blockSignals)
    {
        this->signal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG)->asyncEmit(checked, key.toStdString());
        SIGHT_DEBUG(
            "[EMIT] " << BOOLEAN_CHANGED_SIG << "(" << (checked ? "true" : "false") << ", " << key.toStdString()
            << ")"
        );
    }
}

//------------------------------------------------------------------------------

void SParameters::onColorButton()
{
    QObject* sender = this->sender();

    // Create Color choice dialog.
    auto qtContainer         = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    const QColor oldColor = sender->property("color").value<QColor>();
    const QColor colorQt  = QColorDialog::getColor(oldColor, container);
    if(colorQt.isValid())
    {
        const QString key = sender->property("key").toString();

        QPushButton* colourButton = dynamic_cast<QPushButton*>(sender);
        colourButton->setProperty("color", colorQt);

        int iconSize = colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(iconSize, iconSize);
        pix.fill(colorQt);

        colourButton->setIcon(QIcon(pix));

        this->emitColorSignal(colorQt, key.toStdString());
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeInteger(int)
{
    QObject* sender = this->sender();
    this->emitIntegerSignal(sender);
}

//------------------------------------------------------------------------------

void SParameters::emitIntegerSignal(QObject* widget)
{
    if(!m_blockSignals)
    {
        const QString key = widget->property("key").toString();
        const int count   = widget->property("count").toInt();

        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        const QSpinBox* spinbox = dynamic_cast<const QSpinBox*>(widget);
        const QSlider* slider   = dynamic_cast<const QSlider*>(widget);
        SIGHT_ASSERT("Wrong widget type", spinbox || slider);

        if(count == 1)
        {
            int value;
            if(spinbox)
            {
                value = spinbox->value();
            }
            else
            {
                value = slider->value();
            }

            this->signal<IntegerChangedSignalType>(INTEGER_CHANGED_SIG)->asyncEmit(value, key.toStdString());
            SIGHT_DEBUG("[EMIT] " << INTEGER_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
        }
        else
        {
            int value1;
            int value2;

            if(spinbox)
            {
                const QSpinBox* spin1 = widget->property("widget#0").value<QSpinBox*>();
                const QSpinBox* spin2 = widget->property("widget#1").value<QSpinBox*>();

                value1 = spin1->value();
                value2 = spin2->value();
            }
            else
            {
                const QSlider* spin1 = widget->property("widget#0").value<QSlider*>();
                const QSlider* spin2 = widget->property("widget#1").value<QSlider*>();

                value1 = spin1->value();
                value2 = spin2->value();
            }

            if(count == 2)
            {
                this->signal<Integer2ChangedSignalType>(INTEGER2_CHANGED_SIG)->asyncEmit(
                    value1,
                    value2,
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << INTEGER2_CHANGED_SIG << "(" << value1 << ", " << value2 << ", "
                    << key.toStdString() << ")"
                );
            }
            else
            {
                int value3;
                if(spinbox)
                {
                    const QSpinBox* spin3 = widget->property("widget#2").value<QSpinBox*>();
                    value3 = spin3->value();
                }
                else
                {
                    const QSlider* spin3 = widget->property("widget#2").value<QSlider*>();
                    value3 = spin3->value();
                }

                this->signal<Integer3ChangedSignalType>(INTEGER3_CHANGED_SIG)->asyncEmit(
                    value1,
                    value2,
                    value3,
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << INTEGER3_CHANGED_SIG << "(" << value1 << ", " << value2 << ", " << value3
                    << ", " << key.toStdString() << ")"
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeDouble(double)
{
    QObject* sender = this->sender();
    this->emitDoubleSignal(sender);
}

//------------------------------------------------------------------------------

void SParameters::emitDoubleSignal(QObject* widget)
{
    if(!m_blockSignals)
    {
        const QString key = widget->property("key").toString();
        const int count   = widget->property("count").toInt();

        QDoubleSpinBox* spinbox = qobject_cast<QDoubleSpinBox*>(widget);
        QSlider* slider         = qobject_cast<QSlider*>(widget);

        if(slider)
        {
            const double value = this->getDoubleSliderValue(slider);
            this->signal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG)->asyncEmit(value, key.toStdString());
            SIGHT_DEBUG("[EMIT] " << DOUBLE_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
        }
        else if(spinbox)
        {
            SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

            if(count == 1)
            {
                this->signal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG)->asyncEmit(
                    spinbox->value(),
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << DOUBLE_CHANGED_SIG << "(" << spinbox->value() << ", "
                    << key.toStdString() << ")"
                );
            }
            else
            {
                const QDoubleSpinBox* spin1 = spinbox->property("widget#0").value<QDoubleSpinBox*>();
                const QDoubleSpinBox* spin2 = spinbox->property("widget#1").value<QDoubleSpinBox*>();

                const double value1 = spin1->value();
                const double value2 = spin2->value();

                if(count == 2)
                {
                    this->signal<Double2ChangedSignalType>(DOUBLE2_CHANGED_SIG)->asyncEmit(
                        value1,
                        value2,
                        key.toStdString()
                    );
                    SIGHT_DEBUG(
                        "[EMIT] " << DOUBLE2_CHANGED_SIG << "(" << value1 << ", " << value2
                        << ", " << key.toStdString() << ")"
                    );
                }
                else
                {
                    const QDoubleSpinBox* spin3 = spinbox->property("widget#2").value<QDoubleSpinBox*>();
                    const double value3         = spin3->value();

                    this->signal<Double3ChangedSignalType>(DOUBLE3_CHANGED_SIG)->asyncEmit(
                        value1,
                        value2,
                        value3,
                        key.toStdString()
                    );
                    SIGHT_DEBUG(
                        "[EMIT] " << DOUBLE3_CHANGED_SIG << "(" << value1 << ", " << value2
                        << ", " << value3 << ", " << key.toStdString() << ")"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeDoubleSlider(int)
{
    QSlider* sender = qobject_cast<QSlider*>(this->sender());
    this->emitDoubleSignal(sender);
}

//-----------------------------------------------------------------------------

void SParameters::onSliderMapped(QLabel* label, QSlider* slider)
{
    label->setText(QString::number(slider->value()));
}

//-----------------------------------------------------------------------------

void SParameters::onSliderRangeMapped(QLabel* minLabel, QLabel* maxLabel, QSlider* slider)
{
    const int min = slider->minimum();
    const int max = slider->maximum();

    minLabel->setText(QString::number(min));
    maxLabel->setText(QString::number(max));
}

//-----------------------------------------------------------------------------

void SParameters::onDoubleSliderMapped(QLabel* label, QSlider* slider)
{
    const double newValue = getDoubleSliderValue(slider);
    const int decimals    = slider->property("decimals").toInt();

    label->setText(QString::number(newValue, 'f', decimals));
}

//-----------------------------------------------------------------------------

void SParameters::onDoubleSliderRangeMapped(QLabel* minLabel, QLabel* maxLabel, QSlider* slider)
{
    const double min   = slider->property("min").toDouble();
    const double max   = slider->property("max").toDouble();
    const int decimals = slider->property("decimals").toInt();

    minLabel->setText(QString::number(min, 'g', decimals));
    maxLabel->setText(QString::number(max, 'g', decimals));
}

//-----------------------------------------------------------------------------

void SParameters::onResetBooleanMapped(QWidget* widget)
{
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(widget);
    if(checkbox)
    {
        int value = checkbox->property("defaultValue").toInt();
        checkbox->setCheckState(Qt::CheckState(value));

        const QString key = checkbox->property("key").toString();
        if(!m_blockSignals)
        {
            this->signal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG)->asyncEmit(value, key.toStdString());
            SIGHT_DEBUG(
                "[EMIT] " << BOOLEAN_CHANGED_SIG << "(" << (value ? "true" : "false") << ", "
                << key.toStdString() << ")"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetColorMapped(QWidget* widget)
{
    QPushButton* colourButton = qobject_cast<QPushButton*>(widget);
    if(colourButton)
    {
        const QColor color = colourButton->property("defaultValue").value<QColor>();
        const QString key  = colourButton->property("key").toString();

        int iconSize = colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(iconSize, iconSize);
        pix.fill(color);

        colourButton->setIcon(QIcon(pix));
        colourButton->setProperty("color", color);

        const std::array<std::uint8_t, 4> newColor = {{static_cast<std::uint8_t>(color.red()),
            static_cast<std::uint8_t>(color.green()),
            static_cast<std::uint8_t>(color.blue()),
            static_cast<std::uint8_t>(color.alpha())
        }
        };
        if(!m_blockSignals)
        {
            this->signal<ColorChangedSignalType>(COLOR_CHANGED_SIG)->asyncEmit(newColor, key.toStdString());
            SIGHT_DEBUG(
                "[EMIT] " << COLOR_CHANGED_SIG << "(" << int(newColor[0]) << ", "
                << int(newColor[1]) << ", " << int(newColor[2]) << ", " << int(newColor[3]) << ", "
                << key.toStdString() << ")"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetIntegerMapped(QWidget* widget)
{
    QSlider* slider   = dynamic_cast<QSlider*>(widget);
    QSpinBox* spinbox = dynamic_cast<QSpinBox*>(widget);
    this->blockSignals(true);
    if(slider)
    {
        const int value = slider->property("defaultValue").toInt();
        slider->setValue(value);
    }
    else if(spinbox)
    {
        const QString key = spinbox->property("key").toString();
        const int value   = spinbox->property("defaultValue").toInt();
        const int count   = spinbox->property("count").toInt();
        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        QSpinBox* spin1 = spinbox->property("widget#0").value<QSpinBox*>();
        spin1->setValue(value);

        if(count > 1)
        {
            QSpinBox* spin2 = spinbox->property("widget#1").value<QSpinBox*>();
            spin2->setValue(value);

            if(count == 3)
            {
                QSpinBox* spin3 = spinbox->property("widget#2").value<QSpinBox*>();
                spin3->setValue(value);
            }
        }
    }

    this->blockSignals(false);
    this->emitIntegerSignal(widget);
}

//-----------------------------------------------------------------------------

void SParameters::onResetDoubleMapped(QWidget* widget)
{
    QDoubleSpinBox* spinbox = qobject_cast<QDoubleSpinBox*>(widget);
    QSlider* slider         = qobject_cast<QSlider*>(widget);

    this->blockSignals(true);
    if(slider)
    {
        const double value      = slider->property("defaultValue").toDouble();
        const double min        = slider->property("min").toDouble();
        const double max        = slider->property("max").toDouble();
        const double valueRange = max - min;
        const int sliderVal     = int(std::round(((value - min) / valueRange) * double(slider->maximum())));
        slider->setValue(sliderVal);
    }
    else if(spinbox)
    {
        const QString key  = spinbox->property("key").toString();
        const double value = spinbox->property("defaultValue").toDouble();
        const int count    = spinbox->property("count").toInt();
        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        QDoubleSpinBox* spin1 = spinbox->property("widget#0").value<QDoubleSpinBox*>();
        spin1->setValue(value);

        if(count > 1)
        {
            QDoubleSpinBox* spin2 = spinbox->property("widget#1").value<QDoubleSpinBox*>();
            spin2->setValue(value);

            if(count == 3)
            {
                QDoubleSpinBox* spin3 = spinbox->property("widget#2").value<QDoubleSpinBox*>();
                spin3->setValue(value);
            }
        }
    }

    this->blockSignals(false);
    this->emitDoubleSignal(widget);
}

//-----------------------------------------------------------------------------

QPushButton* SParameters::createResetButton(const std::string& key)
{
    const std::string serviceID = getID().substr(getID().find_last_of('_') + 1);
    QPushButton* resetButton    = new QPushButton("R");
    resetButton->setObjectName(QString::fromStdString(serviceID + "/Reset " + key));
    resetButton->setFocusPolicy(Qt::NoFocus);
    resetButton->setToolTip("Reset to the default value.");
    resetButton->setMaximumWidth(20);

    return resetButton;
}

//-----------------------------------------------------------------------------

void SParameters::createBoolWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    const std::string& defaultValue,
    bool addResetButton
)
{
    QCheckBox* checkbox = new QCheckBox();
    checkbox->setTristate(false);
    checkbox->setObjectName(QString::fromStdString(key));

    if(defaultValue == "true")
    {
        checkbox->setCheckState(Qt::Checked);
    }

    checkbox->setProperty("key", QString(key.c_str()));
    checkbox->setProperty("defaultValue", checkbox->checkState());
    layout.addWidget(checkbox, row, 2);
    QObject::connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onChangeBoolean(int)));

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 5);

        // Connect reset button to the slider
        QObject::connect(resetButton, &QPushButton::clicked, this, [ = ]{onResetBooleanMapped(checkbox);});
    }
}

//-----------------------------------------------------------------------------

void SParameters::createColorWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    const std::string& defaultValue,
    bool addResetButton
)
{
    QPushButton* colourButton = new QPushButton("Color");
    colourButton->setObjectName(QString::fromStdString(key));
    colourButton->setToolTip(tr("Selected color"));
    colourButton->setMinimumSize(120, 35);

    std::string colorStr = "#ffffffff";
    if(!defaultValue.empty())
    {
        std::uint8_t color[4];

        data::tools::Color::hexaStringToRGBA(defaultValue, color);

        colorStr = defaultValue;
    }

    std::uint8_t color[4];
    data::tools::Color::hexaStringToRGBA(colorStr, color);

    const int iconSize = colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(iconSize, iconSize);

    QColor colorQt(color[0], color[1], color[2], color[3]);
    pix.fill(colorQt);

    colourButton->setIcon(QIcon(pix));

    colourButton->setProperty("key", QString(key.c_str()));
    colourButton->setProperty("defaultValue", colorQt);
    colourButton->setProperty("color", colorQt);

    layout.addWidget(colourButton, row, 2);

    QObject::connect(colourButton, &QPushButton::clicked, this, &SParameters::onColorButton);

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 5);

        // Connect reset button to the button
        QObject::connect(resetButton, &QPushButton::clicked, this, [ = ]{onResetColorMapped(colourButton);});
    }
}

//-----------------------------------------------------------------------------

void SParameters::createDoubleWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    double defaultValue,
    double min,
    double max,
    int count,
    bool addResetButton
)
{
    QDoubleSpinBox* spinboxes[3];

    // Spinboxes
    for(int i = 0 ; i < count ; ++i)
    {
        QDoubleSpinBox* spinbox = new QDoubleSpinBox();
        spinboxes[i] = spinbox;

        auto countDecimals = [](double _num) -> int
                             {
                                 std::stringstream out;
                                 out << _num;
                                 const std::string s = out.str();
                                 const std::string t = s.substr(s.find(".") + 1);
                                 return static_cast<int>(t.length());
                             };

        spinbox->setDecimals(std::max(std::max(countDecimals(min), countDecimals(max)), 2));

        spinbox->setRange(min, max);

        // Beware, set setSingleStep after setRange() and setDecimals() otherwise it may fail
        spinbox->setSingleStep(std::abs(spinbox->maximum() - spinbox->minimum()) / 100.);

        // Set value last only after setting range and decimals, otherwise the value may be truncated
        spinbox->setValue(defaultValue);

        spinbox->setProperty("key", QString(key.c_str()));
        spinbox->setProperty("count", count);
        spinbox->setProperty("defaultValue", spinbox->value());

        layout.addWidget(spinbox, row, 2 + i);

        QObject::connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(onChangeDouble(double)));
    }

    QDoubleSpinBox* spinbox = spinboxes[0];
    spinbox->setObjectName(QString::fromStdString(key));

    // Set a property with a pointer on each member of the group
    for(int i = 0 ; i < count ; ++i)
    {
        for(int j = 0 ; j < count ; ++j)
        {
            const std::string propName = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(propName.c_str(), QVariant::fromValue<QDoubleSpinBox*>(spinboxes[j]));
        }
    }

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 5);

        // Connect reset button to the spinbox
        QObject::connect(resetButton, &QPushButton::clicked, this, [ = ]{onResetDoubleMapped(spinbox);});
    }
}

//-----------------------------------------------------------------------------

void SParameters::createDoubleSliderWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    double defaultValue,
    double min,
    double max,
    std::uint8_t decimals,
    bool addResetButton,
    bool onRelease
)
{
    const double valueRange = max - min;

    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setObjectName(QString::fromStdString(key));

    slider->setProperty("key", QString::fromStdString(key));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", defaultValue);
    slider->setProperty("decimals", decimals);
    slider->setProperty("min", min);
    slider->setProperty("max", max);

    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!onRelease);

    setDoubleSliderRange(slider, defaultValue);

    const int defaultSliderValue = int(std::round(((defaultValue - min) / valueRange) * double(slider->maximum())));
    slider->setValue(defaultSliderValue);

    QFont font;
    font.setPointSize(7);
    font.setItalic(true);

    QLabel* minValueLabel = new QLabel();
    minValueLabel->setFont(font);
    minValueLabel->setText(QString::number(min, 'g', decimals));
    minValueLabel->setToolTip("Minimum value.");

    QLabel* maxValueLabel = new QLabel();
    maxValueLabel->setFont(font);
    maxValueLabel->setText(QString::number(max, 'g', decimals));
    maxValueLabel->setToolTip("Maximum value.");

    QLabel* valueLabel = new QLabel();
    valueLabel->setStyleSheet("QLabel { font: bold; }");
    valueLabel->setText(QString::number(defaultValue, 'f', decimals));
    valueLabel->setToolTip("Current value.");
    this->setLabelMinimumSize(valueLabel, min, max, decimals);

    layout.addWidget(minValueLabel, row, 1);
    layout.addWidget(slider, row, 2);
    layout.addWidget(maxValueLabel, row, 3);
    layout.addWidget(valueLabel, row, 4);

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeDoubleSlider(int)));

    // Connect slider value to the label
    QObject::connect(slider, &QSlider::valueChanged, this, [ = ]{onDoubleSliderMapped(valueLabel, slider);});
    QObject::connect(
        slider,
        &QSlider::rangeChanged,
        this,
        [ = ]
        {
            onDoubleSliderRangeMapped(
                minValueLabel,
                maxValueLabel,
                slider
            );
        });

    const std::string propName = std::string("widget#0");
    slider->setProperty(propName.c_str(), QVariant::fromValue<QSlider*>(slider));

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 5);

        // Connect reset button to the slider
        QObject::connect(resetButton, &QPushButton::clicked, this, [ = ]{onResetDoubleMapped(slider);});
    }
}

//-----------------------------------------------------------------------------

void SParameters::createIntegerSliderWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    int defaultValue,
    int min,
    int max,
    bool addResetButton,
    bool onRelease
)
{
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setObjectName(QString::fromStdString(key));
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(defaultValue);

    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!onRelease);

    QFont font;
    font.setPointSize(7);
    font.setItalic(true);

    QLabel* minValueLabel = new QLabel();
    minValueLabel->setFont(font);
    minValueLabel->setText(QString::number(slider->minimum()));
    minValueLabel->setToolTip("Minimum value.");

    QLabel* maxValueLabel = new QLabel();
    maxValueLabel->setFont(font);
    maxValueLabel->setText(QString::number(slider->maximum()));
    maxValueLabel->setToolTip("Maximum value.");

    QLabel* valueLabel = new QLabel();
    valueLabel->setStyleSheet("QLabel { font: bold; }");
    valueLabel->setText(QString("%1").arg(slider->value()));
    valueLabel->setToolTip("Current value.");
    this->setLabelMinimumSize(valueLabel, min, max);

    layout.addWidget(minValueLabel, row, 1);
    layout.addWidget(slider, row, 2);
    layout.addWidget(maxValueLabel, row, 3);
    layout.addWidget(valueLabel, row, 4);

    slider->setProperty("key", QString(key.c_str()));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", slider->value());

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeInteger(int)));

    // Connect slider value to the label
    QObject::connect(slider, &QSlider::valueChanged, this, [ = ]{onSliderMapped(valueLabel, slider);});
    QObject::connect(
        slider,
        &QSlider::rangeChanged,
        this,
        [ = ]
        {
            onSliderRangeMapped(
                minValueLabel,
                maxValueLabel,
                slider
            );
        });

    const std::string propName = std::string("widget#0");
    slider->setProperty(propName.c_str(), QVariant::fromValue<QSlider*>(slider));

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 5);

        // Connect reset button to the slider
        QObject::connect(resetButton, &QPushButton::clicked, this, [ = ]{onResetIntegerMapped(slider);});
    }
}

//-----------------------------------------------------------------------------

void SParameters::createIntegerSpinWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    int defaultValue,
    int min,
    int max,
    int count,
    bool addResetButton
)
{
    QSpinBox* spinboxes[3];

    // Spinboxes
    for(int i = 0 ; i < count ; ++i)
    {
        QSpinBox* spinbox = new QSpinBox();
        spinboxes[i] = spinbox;

        spinbox->setMinimum(min);
        spinbox->setMaximum(max);
        spinbox->setValue(defaultValue);

        spinbox->setProperty("key", QString(key.c_str()));
        spinbox->setProperty("count", count);
        spinbox->setProperty("defaultValue", spinbox->value());

        layout.addWidget(spinbox, row, i + 2);

        // Connect spinbox value with our editor
        QObject::connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onChangeInteger(int)));
    }

    QSpinBox* spinbox = spinboxes[0];
    spinbox->setObjectName(QString::fromStdString(key));

    // Set a property with a pointer on each member of the group
    for(int i = 0 ; i < count ; ++i)
    {
        for(int j = 0 ; j < count ; ++j)
        {
            const std::string propName = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(propName.c_str(), QVariant::fromValue<QSpinBox*>(spinboxes[j]));
        }
    }

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 5);

        // Connect reset button to the spinbox
        QObject::connect(resetButton, &QPushButton::clicked, this, [ = ]{onResetIntegerMapped(spinbox);});
    }
}

//-----------------------------------------------------------------------------

void SParameters::parseEnumString(
    const std::string& options,
    std::vector<std::string>& values,
    std::vector<std::string>& data,
    std::string separators
)
{
    const boost::char_separator<char> sep(separators.c_str());
    const boost::tokenizer<boost::char_separator<char> > tokens {options, sep};

    for(const auto& token : tokens)
    {
        //split again values separated by '='
        const boost::char_separator<char> subsep("=");
        const boost::tokenizer<boost::char_separator<char> > subtokens {token, subsep};
        auto it = subtokens.begin();

        if(it != subtokens.end())
        {
            values.push_back(*it);
            ++it;
        }

        if(it != subtokens.end())
        {
            data.push_back(*it);
            ++it;
        }
        else
        {
            data.push_back(values.back());
        }
    }
}

//------------------------------------------------------------------------------

void SParameters::createEnumWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    const std::string& defaultValue,
    const std::vector<std::string>& values,
    const std::vector<std::string>& data
)
{
    QComboBox* menu = new QComboBox();
    menu->setObjectName(QString::fromStdString(key));

    menu->setProperty("key", QString(key.c_str()));
    int idx = 0;
    for(const auto& value : values)
    {
        menu->insertItem(idx, QString::fromStdString(value));
        ++idx;
    }

    // Add optional data
    idx = 0;
    for(const auto& choice : data)
    {
        menu->setItemData(idx, QString::fromStdString(choice));
        ++idx;
    }

    layout.addWidget(menu, row, 2);

    QObject::connect(menu, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeEnum(int)));

    //Set the comboBox to the default value
    menu->setCurrentText(QString::fromStdString(defaultValue));
}

//-----------------------------------------------------------------------------

double SParameters::getDoubleSliderValue(const QSlider* slider)
{
    const double min = slider->property("min").toDouble();
    const double max = slider->property("max").toDouble();

    const double valueRange = max - min;
    double doubleValue      = min;
    if(slider->maximum() != 0)
    {
        doubleValue = (double(slider->value()) / slider->maximum()) * valueRange + min;
    }

    return doubleValue;
}

//------------------------------------------------------------------------------

void SParameters::setBoolParameter(bool val, std::string key)
{
    this->blockSignals(true);
    QWidget* child      = this->getParamWidget(key);
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(child);

    if(checkbox)
    {
        checkbox->setCheckState(val ? Qt::Checked : Qt::Unchecked);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setColorParameter(std::array<std::uint8_t, 4> color, std::string key)
{
    this->blockSignals(true);
    QWidget* child           = this->getParamWidget(key);
    QPushButton* colorButton = qobject_cast<QPushButton*>(child);

    if(colorButton)
    {
        const int iconSize = colorButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(iconSize, iconSize);
        QColor colorQt(color[0], color[1], color[2], color[3]);
        pix.fill(colorQt);

        colorButton->setIcon(QIcon(pix));
        colorButton->setProperty("color", colorQt);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setDoubleParameter(double val, std::string key)
{
    this->blockSignals(true);
    QWidget* child = this->getParamWidget(key);

    QDoubleSpinBox* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    QSlider* slider         = qobject_cast<QSlider*>(child);

    if(spinbox)
    {
        spinbox->setValue(val);
    }
    else if(slider)
    {
        const double min        = slider->property("min").toDouble();
        const double max        = slider->property("max").toDouble();
        const double valueRange = max - min;
        const int sliderVal     = int(std::round(((val - min) / valueRange) * double(slider->maximum())));
        slider->setValue(sliderVal);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setDouble2Parameter(double val0, double val1, std::string key)
{
    this->blockSignals(true);
    QWidget* child = this->getParamWidget(key);

    if(child)
    {
        QDoubleSpinBox* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        QDoubleSpinBox* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();

        spin0->setValue(val0);
        spin1->setValue(val1);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setDouble3Parameter(double val0, double val1, double val2, std::string key)
{
    this->blockSignals(true);
    QWidget* child = this->getParamWidget(key);

    if(child)
    {
        QDoubleSpinBox* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        QDoubleSpinBox* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
        QDoubleSpinBox* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();

        spin0->setValue(val0);
        spin1->setValue(val1);
        spin2->setValue(val2);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setIntParameter(int val, std::string key)
{
    this->blockSignals(true);
    QWidget* child = this->getParamWidget(key);

    QSpinBox* spinbox = qobject_cast<QSpinBox*>(child);
    QSlider* slider   = qobject_cast<QSlider*>(child);

    if(spinbox)
    {
        spinbox->setValue(val);
    }
    else if(slider)
    {
        slider->setValue(val);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setInt2Parameter(int val0, int val1, std::string key)
{
    this->blockSignals(true);
    QWidget* child = this->getParamWidget(key);

    if(child)
    {
        QSpinBox* spin0 = child->property("widget#0").value<QSpinBox*>();
        QSpinBox* spin1 = child->property("widget#1").value<QSpinBox*>();

        spin0->setValue(val0);
        spin1->setValue(val1);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setInt3Parameter(int val0, int val1, int val2, std::string key)
{
    this->blockSignals(true);
    QWidget* widget = this->getParamWidget(key);

    if(widget)
    {
        QSpinBox* spin0 = widget->property("widget#0").value<QSpinBox*>();
        QSpinBox* spin1 = widget->property("widget#1").value<QSpinBox*>();
        QSpinBox* spin2 = widget->property("widget#2").value<QSpinBox*>();

        spin0->setValue(val0);
        spin1->setValue(val1);
        spin2->setValue(val2);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setEnumParameter(std::string val, std::string key)
{
    this->blockSignals(true);

    QWidget* widget = this->getParamWidget(key);

    QComboBox* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox)
    {
        // Find first in text
        auto res = combobox->findText(QString::fromStdString(val));
        if(res == -1)
        {
            // fallback, try to find in optional data.
            res = combobox->findData(QString::fromStdString(val));
        }

        if(res >= 0)
        {
            combobox->setCurrentIndex(res);
        }
        else
        {
            SIGHT_WARN("value '" + val + "' isn't found in Enum ComboBox '" + key + "'.");
        }
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setEnumIndexParameter(int val, std::string key)
{
    this->blockSignals(true);

    QWidget* widget = this->getParamWidget(key);

    QComboBox* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox)
    {
        combobox->setCurrentIndex(val);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setEnumValues(std::string options, std::string key)
{
    this->blockSignals(true);

    QWidget* widget = this->getParamWidget(key);

    QComboBox* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox)
    {
        combobox->clear();

        std::vector<std::string> values;
        std::vector<std::string> data;

        this->parseEnumString(options, values, data);

        int idx = 0;
        for(const auto& value : values)
        {
            combobox->insertItem(idx, QString::fromStdString(value));
            ++idx;
        }

        // Add optional data
        idx = 0;
        for(const auto& choice : data)
        {
            combobox->setItemData(idx, QString::fromStdString(choice));
            ++idx;
        }
    }

    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void SParameters::emitColorSignal(const QColor color, const std::string& key)
{
    const std::array<std::uint8_t, 4> newColor = {{static_cast<std::uint8_t>(color.red()),
        static_cast<std::uint8_t>(color.green()),
        static_cast<std::uint8_t>(color.blue()),
        static_cast<std::uint8_t>(color.alpha())
    }
    };
    if(!m_blockSignals)
    {
        this->signal<ColorChangedSignalType>(COLOR_CHANGED_SIG)->asyncEmit(newColor, key);
        SIGHT_DEBUG(
            "[EMIT] " << COLOR_CHANGED_SIG << "(" << int(newColor[0]) << ", "
            << int(newColor[1]) << ", " << int(newColor[2]) << ", " << int(newColor[3]) << ", " << key << ")"
        );
    }
}

//-----------------------------------------------------------------------------

void SParameters::blockSignals(bool block)
{
    m_blockSignals = block;
}

//------------------------------------------------------------------------------

void SParameters::setIntMinParameter(int min, std::string key)
{
    QWidget* child = this->getParamWidget(key);

    QSpinBox* spinbox = qobject_cast<QSpinBox*>(child);
    QSlider* slider   = qobject_cast<QSlider*>(child);

    if(spinbox)
    {
        const int count = child->property("count").toInt();
        QSpinBox* spin0 = child->property("widget#0").value<QSpinBox*>();
        spin0->setMinimum(min);

        if(count >= 2)
        {
            QSpinBox* spin1 = child->property("widget#1").value<QSpinBox*>();
            spin1->setMinimum(min);
        }

        if(count >= 3)
        {
            QSpinBox* spin2 = child->property("widget#2").value<QSpinBox*>();
            spin2->setMinimum(min);
        }
    }
    else if(slider)
    {
        slider->setMinimum(min);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void SParameters::setIntMaxParameter(int max, std::string key)
{
    QWidget* child = this->getParamWidget(key);

    QSpinBox* spinbox = qobject_cast<QSpinBox*>(child);
    QSlider* slider   = qobject_cast<QSlider*>(child);

    if(spinbox)
    {
        const int count = child->property("count").toInt();

        QSpinBox* spin0 = child->property("widget#0").value<QSpinBox*>();
        spin0->setMaximum(max);

        if(count >= 2)
        {
            QSpinBox* spin1 = child->property("widget#1").value<QSpinBox*>();
            spin1->setMaximum(max);
        }

        if(count >= 3)
        {
            QSpinBox* spin2 = child->property("widget#2").value<QSpinBox*>();
            spin2->setMaximum(max);
        }
    }
    else if(slider)
    {
        slider->setMaximum(max);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void SParameters::setDoubleMinParameter(double min, std::string key)
{
    QWidget* child = this->getParamWidget(key);

    QDoubleSpinBox* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    QSlider* slider         = qobject_cast<QSlider*>(child);

    if(spinbox)
    {
        const int count = child->property("count").toInt();

        QDoubleSpinBox* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        spin0->setMinimum(min);

        if(count >= 2)
        {
            QDoubleSpinBox* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
            spin1->setMinimum(min);
        }

        if(count >= 3)
        {
            QDoubleSpinBox* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();
            spin2->setMinimum(min);
        }
    }
    else if(slider)
    {
        const double value = getDoubleSliderValue(slider);
        slider->setProperty("min", min);
        setDoubleSliderRange(slider, value);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void SParameters::setDoubleMaxParameter(double max, std::string key)
{
    QWidget* child = this->getParamWidget(key);

    QDoubleSpinBox* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    QSlider* slider         = qobject_cast<QSlider*>(child);

    if(spinbox)
    {
        const int count = child->property("count").toInt();

        QDoubleSpinBox* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        spin0->setMaximum(max);

        if(count >= 2)
        {
            QDoubleSpinBox* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
            spin1->setMaximum(max);
        }

        if(count >= 3)
        {
            QDoubleSpinBox* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();
            spin2->setMaximum(max);
        }
    }
    else if(slider)
    {
        const double value = getDoubleSliderValue(slider);
        slider->setProperty("max", max);
        setDoubleSliderRange(slider, value);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }
}

//-----------------------------------------------------------------------------

void SParameters::setDoubleSliderRange(QSlider* slider, double currentValue)
{
    const std::string key       = slider->property("key").toString().toStdString();
    const double min            = slider->property("min").toDouble();
    const double max            = slider->property("max").toDouble();
    const std::uint8_t decimals = static_cast<std::uint8_t>(slider->property("decimals").toUInt());
    int maxSliderValue          = 1;
    for(std::uint8_t i = 0 ; i < decimals ; ++i)
    {
        maxSliderValue *= 10;
    }

    const double valueRange = max - min;
    maxSliderValue = int(maxSliderValue * valueRange);

    // The slider's maximum internal range is [0; 2 147 483 647]
    // We could technically extend this range by setting the minimum to std::numeric_limits<int>::min()
    // but it would be ridiculous to use a slider handling so many values.
    slider->setMinimum(0);

    SIGHT_ERROR_IF(
        "The requested value range for '" + key + "' is too large to be handled by a double slider. "
                                                  "Please reduce your range, the number of decimals or use a 'spin' widget.",
        maxSliderValue < std::numeric_limits<double>::epsilon()
    );
    if(maxSliderValue < std::numeric_limits<double>::epsilon())
    {
        maxSliderValue = 1.;
    }

    slider->setMaximum(maxSliderValue);

    // Update the slider integer value according to the new mix/max
    if(currentValue <= min)
    {
        slider->setValue(0);

        // qt does not emit the signal if the value does not change, we have to force qt signal to update the displayed
        // value and emit 'doubleChanged' signal
        Q_EMIT slider->valueChanged(0);
    }
    else if(currentValue > max)
    {
        slider->setValue(maxSliderValue);
    }
    else
    {
        const int sliderVal = int(std::round(((currentValue - min) / valueRange) * double(slider->maximum())));
        slider->setValue(sliderVal);
    }
}

//-----------------------------------------------------------------------------

QWidget* SParameters::getParamWidget(const std::string& key)
{
    auto qtContainer      = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    const QWidget* widget = qtContainer->getQtContainer();

    QWidget* child = widget->findChild<QWidget*>(QString::fromStdString(key));
    SIGHT_ERROR_IF("Widget '" + key + "' is not found", !child);

    return child;
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui::qt
