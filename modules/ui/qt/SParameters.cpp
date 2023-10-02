/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "SParameters.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/Object.hpp>

#include <data/tools/Color.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>
#include <ui/qt/widget/NonLinearSlider.hpp>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QColorDialog>
#include <QEvent>
#include <QFormLayout>
#include <QGraphicsColorizeEffect>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QString>
#include <QStyle>
#include <QToolButton>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

// Internal static function to split a string using separator (usually =), mainly used to split enum into value, data
static std::pair<std::string, std::string> splitString(const std::string& _str, const std::string& _sep = "=")
{
    std::string left;
    std::string right;

    const boost::char_separator<char> subsep(_sep.c_str());
    const boost::tokenizer<boost::char_separator<char> > subtokens {_str, subsep};
    auto it = subtokens.begin();

    if(it != subtokens.end())
    {
        left = *it;
        ++it;
        // check if we have another part
        if(it != subtokens.end())
        {
            right = *it;
        }
    }

    return {left, right};
}

//-----------------------------------------------------------------------------

SParameters::SParameters() noexcept
{
    newSignal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG);
    newSignal<signals::BooleanChangedSignalType>(signals::BOOLEAN_CHANGED_SIG);
    newSignal<signals::ColorChangedSignalType>(signals::COLOR_CHANGED_SIG);
    newSignal<signals::DoubleChangedSignalType>(signals::DOUBLE_CHANGED_SIG);
    newSignal<signals::Double2ChangedSignalType>(signals::DOUBLE2_CHANGED_SIG);
    newSignal<signals::Double3ChangedSignalType>(signals::DOUBLE3_CHANGED_SIG);
    newSignal<signals::IntegerChangedSignalType>(signals::INTEGER_CHANGED_SIG);
    newSignal<signals::Integer2ChangedSignalType>(signals::INTEGER2_CHANGED_SIG);
    newSignal<signals::Integer3ChangedSignalType>(signals::INTEGER3_CHANGED_SIG);
    newSignal<signals::EnumChangedSignalType>(signals::ENUM_CHANGED_SIG);
    newSignal<signals::EnumChangedIndexSignalType>(signals::ENUM_INDEX_CHANGED_SIG);

    newSlot(slots::s_SET_PARAMETER_SLOT, &SParameters::setParameter, this);
    newSlot(slots::s_SET_BOOL_PARAMETER_SLOT, &SParameters::setBoolParameter, this);
    newSlot(slots::s_SET_COLOR_PARAMETER_SLOT, &SParameters::setColorParameter, this);
    newSlot(slots::s_SET_DOUBLE_PARAMETER_SLOT, &SParameters::setDoubleParameter, this);
    newSlot(slots::s_SET_DOUBLE2_PARAMETER_SLOT, &SParameters::setDouble2Parameter, this);
    newSlot(slots::s_SET_DOUBLE3_PARAMETER_SLOT, &SParameters::setDouble3Parameter, this);
    newSlot(slots::s_SET_INT_PARAMETER_SLOT, &SParameters::setIntParameter, this);
    newSlot(slots::s_SET_INT2_PARAMETER_SLOT, &SParameters::setInt2Parameter, this);
    newSlot(slots::s_SET_INT3_PARAMETER_SLOT, &SParameters::setInt3Parameter, this);
    newSlot(slots::s_SET_ENUM_PARAMETER_SLOT, &SParameters::setEnumParameter, this);
    newSlot(slots::s_SET_ENUM_INDEX_PARAMETER_SLOT, &SParameters::setEnumIndexParameter, this);
    newSlot(slots::s_UPDATE_ENUM_RANGE_SLOT, &SParameters::updateEnumRange, this);
    newSlot(slots::s_UPDATE_INT_MIN_PARAMETER_SLOT, &SParameters::updateIntMinParameter, this);
    newSlot(slots::s_UPDATE_INT_MAX_PARAMETER_SLOT, &SParameters::updateIntMaxParameter, this);
    newSlot(slots::s_UPDATE_DOUBLE_MIN_PARAMETER_SLOT, &SParameters::updateDoubleMinParameter, this);
    newSlot(slots::s_UPDATE_DOUBLE_MAX_PARAMETER_SLOT, &SParameters::updateDoubleMaxParameter, this);
}

//-----------------------------------------------------------------------------

SParameters::~SParameters() noexcept =
    default;

//-----------------------------------------------------------------------------

void SParameters::configuring()
{
    this->initialize();

    const ConfigType configTree = this->getConfiguration();
}

//-----------------------------------------------------------------------------

static QWidget* recursivelyFindWidgetInLayoutByKey(const QLayout* layout, const std::string& key)
{
    for(int i = 0 ; i < layout->count() ; ++i)
    {
        auto* item = layout->itemAt(i);
        if(QWidget* widget = item->widget(); widget != nullptr)
        {
            if(key == widget->property("key").toString().toStdString())
            {
                return widget;
            }
        }
        else if(QLayout* subLayout = item->layout(); subLayout != nullptr)
        {
            if(QWidget* subWidget = recursivelyFindWidgetInLayoutByKey(subLayout, key); subWidget != nullptr)
            {
                return subWidget;
            }
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

static QVector<QWidget*> recursivelyFindWidgetsInLayoutByKey(const QLayout* layout, const std::string& key)
{
    QVector<QWidget*> res;
    for(int i = 0 ; i < layout->count() ; ++i)
    {
        auto* item = layout->itemAt(i);
        if(QWidget* widget = item->widget(); widget != nullptr)
        {
            if(key == widget->property("key").toString().toStdString())
            {
                res.append(widget);
            }
        }
        else if(QLayout* subLayout = item->layout(); subLayout != nullptr)
        {
            res.append(recursivelyFindWidgetsInLayoutByKey(subLayout, key));
        }
    }

    return res;
}

//------------------------------------------------------------------------------

void SParameters::starting()
{
    this->create();

    const std::string serviceID = getID().substr(getID().find_last_of('_') + 1);

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    qtContainer->getQtContainer()->setObjectName(QString::fromStdString(serviceID));

    auto* layout            = new QGridLayout();
    QScrollArea* scrollArea = nullptr;
    QWidget* viewport       = nullptr;

    service::IService::ConfigType config               = this->getConfiguration();
    const service::IService::ConfigType& parametersCfg = config.get_child("parameters");
    const bool scrollable                              = parametersCfg.get<bool>("<xmlattr>.scrollable", false);
    if(scrollable)
    {
        scrollArea = new QScrollArea(qtContainer->getQtContainer()->parentWidget());
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        viewport = new QWidget(qtContainer->getQtContainer());
    }

    int row = 0;

    this->blockSignals(true);

    // Create widgets
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& param, parametersCfg.equal_range("param"))
    {
        const service::IService::ConfigType& cfg = param.second;

        const auto name         = cfg.get<std::string>("<xmlattr>.name");
        const auto key          = cfg.get<std::string>("<xmlattr>.key");
        const auto type         = cfg.get<std::string>("<xmlattr>.type");
        const auto defaultValue = cfg.get<std::string>("<xmlattr>.defaultValue");
        const bool resetButton  = cfg.get<bool>("<xmlattr>.reset", true);

        // If orientation is true => horizontal
        // If orientation is false => vertical
        const bool orientation = cfg.get<std::string>("<xmlattr>.orientation", "horizontal") != "vertical";

        if(!name.empty())
        {
            auto* parameterLabel = new QLabel(QString::fromStdString(name));
            parameterLabel->setWordWrap(true);
            parameterLabel->setStyleSheet("margin: -0.1em;");
            layout->addWidget(parameterLabel, row, 0);
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
                    onRelease,
                    orientation
                );
            }
            else
            {
                SIGHT_ERROR(
                    std::string("Unknown widget type : '") + widget + "' for " + name
                    + ". Must be 'spin' or 'slider'."
                );
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
                    onRelease,
                    orientation
                );
            }
            else
            {
                SIGHT_ERROR(
                    std::string("Unknown widget type : '") + widget + "' for " + name
                    + ". Must be 'spin' or 'slider'."
                );
            }
        }
        else if(type == "enum")
        {
            const auto widget = cfg.get<std::string>("<xmlattr>.widget", "combobox");

            if(widget == "combobox")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                //split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::SParameters::parseEnumString(options, values, data);
                this->createEnumWidget(*layout, row, key, defaultValue, values, data);
            }
            else if(widget == "slider")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                //split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::SParameters::parseEnumString(options, values, data);
                const bool onRelease = cfg.get<bool>("<xmlattr>.emitOnRelease", false);
                this->createSliderEnumWidget(*layout, row, key, defaultValue, values, onRelease, orientation);
            }
            else if(widget == "buttonBar")
            {
                const int hOffset       = cfg.get<int>("<xmlattr>.hOffset", 0);
                const int width         = cfg.get<int>("<xmlattr>.width", 0);
                const int height        = cfg.get<int>("<xmlattr>.height", 0);
                const std::string style = cfg.get<std::string>("<xmlattr>.style", "iconOnly");

                const auto valueConfig = cfg.equal_range("item");
                std::vector<enumButtonParam> buttonList;
                for(auto valueConfigIt = valueConfig.first ;
                    valueConfigIt != valueConfig.second ;
                    ++valueConfigIt)
                {
                    const auto value = valueConfigIt->second.get<std::string>(
                        "<xmlattr>.value"
                    );

                    const auto label = valueConfigIt->second.get<std::string>(
                        "<xmlattr>.label",
                        ""
                    );

                    const auto iconPathRelative = valueConfigIt->second.get<std::string>(
                        "<xmlattr>.icon",
                        ""
                    );

                    const std::string iconPath =
                        core::runtime::getModuleResourceFilePath(iconPathRelative).generic_string();

                    buttonList.push_back(
                        enumButtonParam(
                        {
                            value,
                            label,
                            iconPath
                        })
                    );
                }

                this->createButtonBarEnumWidget(
                    *layout,
                    row,
                    key,
                    defaultValue,
                    buttonList,
                    width,
                    height,
                    hOffset,
                    style
                );
            }
        }

        ++row;
    }

    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& param, parametersCfg.equal_range("param"))
    {
        const service::IService::ConfigType& cfg = param.second;

        const auto key                 = cfg.get<std::string>("<xmlattr>.key");
        const std::string depends      = cfg.get<std::string>("<xmlattr>.depends", "");
        const std::string dependsValue = cfg.get<std::string>("<xmlattr>.dependsValue", "");
        const bool dependsreverse      = cfg.get<bool>("<xmlattr>.dependsReverse", false);

        if(!depends.empty())
        {
            QVector<QWidget*> widgets = recursivelyFindWidgetsInLayoutByKey(layout, key);

            SIGHT_ASSERT("there are no widgets", !widgets.isEmpty());

            QWidget* dependsWidget = recursivelyFindWidgetInLayoutByKey(layout, depends);

            for(QWidget* widget : widgets)
            {
                widget->installEventFilter(this);
                auto* checkBox = qobject_cast<QCheckBox*>(dependsWidget);
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
                    auto* comboBox = qobject_cast<QComboBox*>(dependsWidget);
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
    }

    if(scrollArea != nullptr)
    {
        viewport->setLayout(layout);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(viewport);
        auto* mainLayout = new QGridLayout();
        qtContainer->setLayout(mainLayout);
        mainLayout->addWidget(scrollArea);

        // The size of the vertical scroll bar isn't taken into account when the QGridLayout fill the space, as such
        // some buttons (particularly reset buttons) get hidden. The workaround is to biggen the right margin a little.
        int scrollBarWidth = QScrollBar().sizeHint().width();
        QMargins margins   = layout->contentsMargins();
        margins.setRight(scrollBarWidth);
        layout->setContentsMargins(margins);
    }
    else
    {
        qtContainer->setLayout(layout);
    }

    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void SParameters::updating()
{
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* widget  = qtContainer->getQtContainer();

    service::IService::ConfigType config               = this->getConfiguration();
    const service::IService::ConfigType& parametersCfg = config.get_child("parameters");

    // emit signal for each widget
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& param, parametersCfg.equal_range("param"))
    {
        const service::IService::ConfigType& cfg = param.second;

        const auto key  = cfg.get<std::string>("<xmlattr>.key");
        const auto type = cfg.get<std::string>("<xmlattr>.type");

        auto* child = widget->findChild<QObject*>(QString::fromStdString(key));
        if(child != nullptr)
        {
            if(type == "bool")
            {
                const QCheckBox* box = dynamic_cast<QCheckBox*>(child);
                SIGHT_ASSERT("Widget must be a QCheckBox", box);

                const bool state = (box->checkState() == Qt::Checked);

                if(!m_blockSignals)
                {
                    this->signal<signals::BooleanChangedSignalType>(signals::BOOLEAN_CHANGED_SIG)->asyncEmit(
                        state,
                        key
                    );
                    this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(state, key);
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (state ? "true" : "false") << ", " << key
                        << ")"
                    );
                }
            }
            else if(type == "color")
            {
                const auto colorQt = child->property("color").value<QColor>();
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
                if(auto* box = qobject_cast<QComboBox*>(child); box != nullptr)
                {
                    const QString data = box->itemData(box->currentIndex()).toString();

                    if(!m_blockSignals)
                    {
                        this->signal<signals::EnumChangedSignalType>(signals::ENUM_CHANGED_SIG)->asyncEmit(
                            data.toStdString(),
                            key
                        );
                        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                            data.toStdString(),
                            key
                        );
                        SIGHT_DEBUG(
                            "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key << ")"
                        );
                        this->signal<signals::EnumChangedIndexSignalType>(signals::ENUM_INDEX_CHANGED_SIG)->asyncEmit(
                            box->currentIndex(),
                            key
                        );
                        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                            box->currentIndex(),
                            key
                        );
                        SIGHT_DEBUG(
                            "[EMIT] " << signals::ENUM_INDEX_CHANGED_SIG << "(" << box->currentIndex() << ", " << key
                            << ")"
                        );
                    }
                }
                else if(auto* slider = qobject_cast<sight::ui::qt::widget::NonLinearSlider*>(child); slider != nullptr)
                {
                    if(!m_blockSignals)
                    {
                        int value = slider->value();
                        this->signal<signals::IntegerChangedSignalType>(signals::INTEGER_CHANGED_SIG)->asyncEmit(
                            value,
                            key
                        );
                        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(value, key);
                        SIGHT_DEBUG("[EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << value << ", " << key << ")");
                        this->signal<signals::EnumChangedSignalType>(signals::ENUM_CHANGED_SIG)->asyncEmit(
                            std::to_string(
                                value
                            ),
                            key
                        );
                        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                            std::to_string(
                                value
                            ),
                            key
                        );
                        SIGHT_DEBUG("[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << value << ", " << key << ")");
                        this->signal<signals::EnumChangedIndexSignalType>(signals::ENUM_INDEX_CHANGED_SIG)->asyncEmit(
                            int(slider->index()),
                            key
                        );
                    }
                }
                else if(auto* buttonGroup = qobject_cast<QButtonGroup*>(child);
                        buttonGroup != nullptr)
                {
                    auto* checkedButton = buttonGroup->checkedButton();

                    // Find the currently checked button
                    int buttonIndex = buttonGroup->buttons().indexOf(checkedButton);

                    if(checkedButton != nullptr)
                    {
                        const std::string value = checkedButton->property("value").toString().toStdString();
                        this->signal<signals::EnumChangedSignalType>(signals::ENUM_CHANGED_SIG)
                        ->asyncEmit(value, key);
                        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)
                        ->asyncEmit(value, key);
                        SIGHT_DEBUG(
                            "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << value << ", " << key << ")"
                        );
                        this->signal<signals::EnumChangedIndexSignalType>(signals::ENUM_INDEX_CHANGED_SIG)
                        ->asyncEmit(buttonIndex, key);
                    }
                }
                else
                {
                    SIGHT_ASSERT("Widget must either be a ComboBox or NonLinearSlider", false);
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
        auto* checkBox = qobject_cast<QCheckBox*>(_watched);
        if(checkBox != nullptr)
        {
            checkBox->stateChanged(checkBox->isChecked() ? Qt::Checked : Qt::Unchecked);
        }
        else
        {
            auto* comboBox = qobject_cast<QComboBox*>(_watched);
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
        _widget->setDisabled(_checkBox->checkState() != 0U);
    }
    else
    {
        _widget->setEnabled(_checkBox->checkState() != 0U);
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
        _widget->setDisabled(_comboBox->currentText().toStdString() == _value);
    }
    else
    {
        _widget->setEnabled(_comboBox->currentText().toStdString() == _value);
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeEnum(int value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();

    const auto* box = dynamic_cast<const QComboBox*>(sender);

    SIGHT_ASSERT("Wrong widget type", box);

    const QString data = box->itemData(value).toString();

    if(!m_blockSignals)
    {
        this->signal<signals::EnumChangedSignalType>(signals::ENUM_CHANGED_SIG)->asyncEmit(
            data.toStdString(),
            key.toStdString()
        );
        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
            data.toStdString(),
            key.toStdString()
        );
        SIGHT_DEBUG(
            "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key.toStdString()
            << ")"
        );
        this->signal<signals::EnumChangedIndexSignalType>(signals::ENUM_INDEX_CHANGED_SIG)->asyncEmit(
            value,
            key.toStdString()
        );
        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
            box->currentIndex(),
            key.toStdString()
        );
        SIGHT_DEBUG("[EMIT] " << signals::ENUM_INDEX_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
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
        this->signal<signals::BooleanChangedSignalType>(signals::BOOLEAN_CHANGED_SIG)->asyncEmit(
            checked,
            key.toStdString()
        );
        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(checked, key.toStdString());
        SIGHT_DEBUG(
            "[EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (checked ? "true" : "false") << ", " << key.toStdString()
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

    const auto oldColor  = sender->property("color").value<QColor>();
    const QColor colorQt = QColorDialog::getColor(oldColor, container);
    if(colorQt.isValid())
    {
        const QString key = sender->property("key").toString();

        auto* colourButton = dynamic_cast<QPushButton*>(sender);
        colourButton->setProperty("color", colorQt);

        int iconSize = colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(iconSize, iconSize);
        pix.fill(colorQt);

        colourButton->setIcon(QIcon(pix));

        this->emitColorSignal(colorQt, key.toStdString());
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeInteger(int /*unused*/)
{
    QObject* sender = this->sender();
    this->emitIntegerSignal(sender);
}

//------------------------------------------------------------------------------

void SParameters::emitIntegerSignal(QObject* widget)
{
    if(!m_blockSignals)
    {
        SIGHT_ASSERT("widget is null", widget != nullptr);

        const QString key = widget->property("key").toString();
        const int count   = widget->property("count").toInt();

        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        const auto* spinbox = dynamic_cast<const QSpinBox*>(widget);
        const auto* slider  = dynamic_cast<const QSlider*>(widget);
        SIGHT_ASSERT("Wrong widget type", spinbox || slider);

        if(count == 1)
        {
            int value = 0;
            if(spinbox != nullptr)
            {
                value = spinbox->value();
            }
            else
            {
                value = slider->value();
            }

            this->signal<signals::IntegerChangedSignalType>(signals::INTEGER_CHANGED_SIG)->asyncEmit(
                value,
                key.toStdString()
            );
            this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                value,
                key.toStdString()
            );
            SIGHT_DEBUG("[EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
        }
        else
        {
            int value1 = 0;
            int value2 = 0;

            if(spinbox != nullptr)
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
                this->signal<signals::Integer2ChangedSignalType>(signals::INTEGER2_CHANGED_SIG)->asyncEmit(
                    value1,
                    value2,
                    key.toStdString()
                );
                const sight::ui::base::int2_t values = {value1, value2};
                this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                    values,
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << signals::INTEGER2_CHANGED_SIG << "(" << value1 << ", " << value2 << ", "
                    << key.toStdString() << ")"
                );
            }
            else
            {
                int value3 = 0;
                if(spinbox != nullptr)
                {
                    const QSpinBox* spin3 = widget->property("widget#2").value<QSpinBox*>();
                    value3 = spin3->value();
                }
                else
                {
                    const QSlider* spin3 = widget->property("widget#2").value<QSlider*>();
                    value3 = spin3->value();
                }

                this->signal<signals::Integer3ChangedSignalType>(signals::INTEGER3_CHANGED_SIG)->asyncEmit(
                    value1,
                    value2,
                    value3,
                    key.toStdString()
                );
                const sight::ui::base::int3_t values = {value1, value2, value3};
                this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                    values,
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << signals::INTEGER3_CHANGED_SIG << "(" << value1 << ", " << value2 << ", " << value3
                    << ", " << key.toStdString() << ")"
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeDouble(double /*unused*/)
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

        auto* spinbox = qobject_cast<QDoubleSpinBox*>(widget);
        auto* slider  = qobject_cast<QSlider*>(widget);

        if(slider != nullptr)
        {
            const double value = sight::module::ui::qt::SParameters::getDoubleSliderValue(slider);
            this->signal<signals::DoubleChangedSignalType>(signals::DOUBLE_CHANGED_SIG)->asyncEmit(
                value,
                key.toStdString()
            );
            this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                value,
                key.toStdString()
            );
            SIGHT_DEBUG("[EMIT] " << signals::DOUBLE_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
        }
        else if(spinbox != nullptr)
        {
            SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

            if(count == 1)
            {
                this->signal<signals::DoubleChangedSignalType>(signals::DOUBLE_CHANGED_SIG)->asyncEmit(
                    spinbox->value(),
                    key.toStdString()
                );
                this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                    spinbox->value(),
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << signals::DOUBLE_CHANGED_SIG << "(" << spinbox->value() << ", "
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
                    this->signal<signals::Double2ChangedSignalType>(signals::DOUBLE2_CHANGED_SIG)->asyncEmit(
                        value1,
                        value2,
                        key.toStdString()
                    );
                    const sight::ui::base::double2_t values = {value1, value2};
                    this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                        values,
                        key.toStdString()
                    );
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::DOUBLE2_CHANGED_SIG << "(" << value1 << ", " << value2
                        << ", " << key.toStdString() << ")"
                    );
                }
                else
                {
                    const QDoubleSpinBox* spin3 = spinbox->property("widget#2").value<QDoubleSpinBox*>();
                    const double value3         = spin3->value();

                    this->signal<signals::Double3ChangedSignalType>(signals::DOUBLE3_CHANGED_SIG)->asyncEmit(
                        value1,
                        value2,
                        value3,
                        key.toStdString()
                    );
                    const sight::ui::base::double3_t values = {value1, value2, value3};
                    this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                        values,
                        key.toStdString()
                    );
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::DOUBLE3_CHANGED_SIG << "(" << value1 << ", " << value2
                        << ", " << value3 << ", " << key.toStdString() << ")"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeDoubleSlider(int /*unused*/)
{
    auto* sender = qobject_cast<QSlider*>(this->sender());
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
    auto* checkbox = qobject_cast<QCheckBox*>(widget);
    if(checkbox != nullptr)
    {
        int value = checkbox->property("defaultValue").toInt();
        checkbox->setCheckState(Qt::CheckState(value));

        const QString key = checkbox->property("key").toString();
        if(!m_blockSignals)
        {
            this->signal<signals::BooleanChangedSignalType>(signals::BOOLEAN_CHANGED_SIG)->asyncEmit(
                value != 0,
                key.toStdString()
            );
            this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                value != 0,
                key.toStdString()
            );
            SIGHT_DEBUG(
                "[EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (value ? "true" : "false") << ", "
                << key.toStdString() << ")"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetColorMapped(QWidget* widget)
{
    auto* colourButton = qobject_cast<QPushButton*>(widget);
    if(colourButton != nullptr)
    {
        const auto color  = colourButton->property("defaultValue").value<QColor>();
        const QString key = colourButton->property("key").toString();

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
            this->signal<signals::ColorChangedSignalType>(signals::COLOR_CHANGED_SIG)->asyncEmit(
                newColor,
                key.toStdString()
            );
            this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(
                newColor,
                key.toStdString()
            );
            SIGHT_DEBUG(
                "[EMIT] " << signals::COLOR_CHANGED_SIG << "(" << int(newColor[0]) << ", "
                << int(newColor[1]) << ", " << int(newColor[2]) << ", " << int(newColor[3]) << ", "
                << key.toStdString() << ")"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetIntegerMapped(QWidget* widget)
{
    auto* slider  = dynamic_cast<QSlider*>(widget);
    auto* spinbox = dynamic_cast<QSpinBox*>(widget);
    this->blockSignals(true);
    if(slider != nullptr)
    {
        const int value = slider->property("defaultValue").toInt();
        slider->setValue(value);
    }
    else if(spinbox != nullptr)
    {
        const QString key = spinbox->property("key").toString();
        const int value   = spinbox->property("defaultValue").toInt();
        const int count   = spinbox->property("count").toInt();
        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        auto* spin1 = spinbox->property("widget#0").value<QSpinBox*>();
        spin1->setValue(value);

        if(count > 1)
        {
            auto* spin2 = spinbox->property("widget#1").value<QSpinBox*>();
            spin2->setValue(value);

            if(count == 3)
            {
                auto* spin3 = spinbox->property("widget#2").value<QSpinBox*>();
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
    auto* spinbox = qobject_cast<QDoubleSpinBox*>(widget);
    auto* slider  = qobject_cast<QSlider*>(widget);

    this->blockSignals(true);
    if(slider != nullptr)
    {
        const double value      = slider->property("defaultValue").toDouble();
        const double min        = slider->property("min").toDouble();
        const double max        = slider->property("max").toDouble();
        const double valueRange = max - min;
        const int sliderVal     = int(std::round(((value - min) / valueRange) * double(slider->maximum())));
        slider->setValue(sliderVal);
    }
    else if(spinbox != nullptr)
    {
        const QString key  = spinbox->property("key").toString();
        const double value = spinbox->property("defaultValue").toDouble();
        const int count    = spinbox->property("count").toInt();
        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        auto* spin1 = spinbox->property("widget#0").value<QDoubleSpinBox*>();
        spin1->setValue(value);

        if(count > 1)
        {
            auto* spin2 = spinbox->property("widget#1").value<QDoubleSpinBox*>();
            spin2->setValue(value);

            if(count == 3)
            {
                auto* spin3 = spinbox->property("widget#2").value<QDoubleSpinBox*>();
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
    std::string serviceID = getID().substr(getID().find_last_of('_') + 1);
    auto* resetButton     = new QPushButton("R");
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
    auto* checkbox = new QCheckBox();
    checkbox->setTristate(false);
    checkbox->setObjectName(QString::fromStdString(key));

    if(defaultValue == "true")
    {
        checkbox->setCheckState(Qt::Checked);
    }

    checkbox->setProperty("key", QString(key.c_str()));
    checkbox->setProperty("defaultValue", checkbox->checkState());
    layout.addWidget(checkbox, row, 1);
    QObject::connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onChangeBoolean(int)));

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 2);

        // Connect reset button to the slider
        QObject::connect(resetButton, &QPushButton::clicked, this, [this, checkbox]{onResetBooleanMapped(checkbox);});
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
    auto* colourButton = new QPushButton("Color");
    colourButton->setObjectName(QString::fromStdString(key));
    colourButton->setToolTip(tr("Selected color"));
    colourButton->setMinimumSize(120, 35);

    std::string colorStr = "#ffffffff";
    if(!defaultValue.empty())
    {
        std::array<std::uint8_t, 4> color {};

        data::tools::Color::hexaStringToRGBA(defaultValue, color);

        colorStr = defaultValue;
    }

    std::array<std::uint8_t, 4> color {};
    data::tools::Color::hexaStringToRGBA(colorStr, color);

    const int iconSize = colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(iconSize, iconSize);

    QColor colorQt(color[0], color[1], color[2], color[3]);
    pix.fill(colorQt);

    colourButton->setIcon(QIcon(pix));

    colourButton->setProperty("key", QString(key.c_str()));
    colourButton->setProperty("defaultValue", colorQt);
    colourButton->setProperty("color", colorQt);

    layout.addWidget(colourButton, row, 1);

    QObject::connect(colourButton, &QPushButton::clicked, this, &SParameters::onColorButton);

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 2);

        // Connect reset button to the button
        QObject::connect(
            resetButton,
            &QPushButton::clicked,
            this,
            [this, colourButton]
            {
                onResetColorMapped(colourButton);
            });
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
    auto* subLayout = new QGridLayout();
    layout.addLayout(subLayout, row, 1);

    std::array<QDoubleSpinBox*, 3> spinboxes {};

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(count) ; ++i)
    {
        auto* spinbox = new QDoubleSpinBox();
        spinbox->setObjectName(QString::fromStdString(key + "/" + std::to_string(i)));
        spinboxes[i] = spinbox;

        auto countDecimals = [](double _num) -> int
                             {
                                 std::stringstream out;
                                 out << _num;
                                 const std::string s = out.str();
                                 const std::string t = s.substr(s.find('.') + 1);
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

        subLayout->addWidget(spinbox, 0, int(i));

        QObject::connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(onChangeDouble(double)));
    }

    QDoubleSpinBox* spinbox = spinboxes[0];
    spinbox->setObjectName(QString::fromStdString(key));

    // Set a property with a pointer on each member of the group
    for(std::size_t i = 0 ; i < std::size_t(count) ; ++i)
    {
        for(std::size_t j = 0 ; j < std::size_t(count) ; ++j)
        {
            const std::string propName = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(propName.c_str(), QVariant::fromValue<QDoubleSpinBox*>(spinboxes[j]));
        }
    }

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 2);

        // Connect reset button to the spinbox
        QObject::connect(resetButton, &QPushButton::clicked, this, [this, spinbox]{onResetDoubleMapped(spinbox);});
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
    bool onRelease,
    bool orientation
)
{
    auto* subLayout = new QGridLayout();
    layout.addLayout(subLayout, row, 1);

    const double valueRange = max - min;

    auto* slider = new QSlider();
    if(orientation)
    {
        slider->setOrientation(Qt::Horizontal);
    }
    else
    {
        slider->setOrientation(Qt::Vertical);
    }

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

    auto* minValueLabel = new QLabel();
    minValueLabel->setFont(font);
    minValueLabel->setText(QString::number(min, 'g', decimals));
    minValueLabel->setToolTip("Minimum value.");
    minValueLabel->setObjectName(QString::fromStdString(key + "/minValueLabel"));

    auto* maxValueLabel = new QLabel();
    maxValueLabel->setFont(font);
    maxValueLabel->setText(QString::number(max, 'g', decimals));
    maxValueLabel->setToolTip("Maximum value.");
    maxValueLabel->setObjectName(QString::fromStdString(key + "/maxValueLabel"));

    auto* valueLabel = new QLabel();
    valueLabel->setStyleSheet("QLabel { font: bold; }");
    valueLabel->setText(QString::number(defaultValue, 'f', decimals));
    valueLabel->setToolTip("Current value.");
    sight::module::ui::qt::SParameters::setLabelMinimumSize(valueLabel, min, max, decimals);
    valueLabel->setObjectName(QString::fromStdString(key + "/valueLabel"));

    if(orientation)
    {
        subLayout->addWidget(minValueLabel, 0, 0);
        subLayout->addWidget(slider, 0, 1);
        subLayout->addWidget(maxValueLabel, 0, 2);
        subLayout->addWidget(valueLabel, 0, 3);
    }
    else
    {
        subLayout->addWidget(maxValueLabel, 0, 0);
        subLayout->addWidget(slider, 1, 0);
        subLayout->addWidget(minValueLabel, 2, 0);
        subLayout->addWidget(valueLabel, 3, 0);
    }

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeDoubleSlider(int)));

    // Connect slider value to the label
    QObject::connect(
        slider,
        &QSlider::valueChanged,
        this,
        [this, valueLabel, slider]
        {
            this->onDoubleSliderMapped(valueLabel, slider);
        });
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

        if(orientation)
        {
            subLayout->addWidget(resetButton, 0, 4);
        }
        else
        {
            subLayout->addWidget(resetButton, 4, 0);
        }

        // Connect reset button to the slider
        QObject::connect(resetButton, &QPushButton::clicked, this, [this, slider]{onResetDoubleMapped(slider);});
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
    bool onRelease,
    bool orientation
)
{
    auto* subLayout = new QGridLayout();
    layout.addLayout(subLayout, row, 1);

    auto* slider = new QSlider();
    if(orientation)
    {
        slider->setOrientation(Qt::Horizontal);
    }
    else
    {
        slider->setOrientation(Qt::Vertical);
    }

    slider->setObjectName(QString::fromStdString(key));
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(defaultValue);

    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!onRelease);

    QFont font;
    font.setPointSize(7);
    font.setItalic(true);

    auto* minValueLabel = new QLabel();
    minValueLabel->setFont(font);
    minValueLabel->setText(QString::number(slider->minimum()));
    minValueLabel->setToolTip("Minimum value.");
    minValueLabel->setObjectName(QString::fromStdString(key + "/minValueLabel"));

    auto* maxValueLabel = new QLabel();
    maxValueLabel->setFont(font);
    maxValueLabel->setText(QString::number(slider->maximum()));
    maxValueLabel->setToolTip("Maximum value.");
    maxValueLabel->setObjectName(QString::fromStdString(key + "/maxValueLabel"));

    auto* valueLabel = new QLabel();
    valueLabel->setStyleSheet("QLabel { font: bold; }");
    valueLabel->setText(QString("%1").arg(slider->value()));
    valueLabel->setToolTip("Current value.");
    sight::module::ui::qt::SParameters::setLabelMinimumSize(valueLabel, min, max);
    valueLabel->setObjectName(QString::fromStdString(key + "/valueLabel"));

    if(orientation)
    {
        subLayout->addWidget(minValueLabel, 0, 0);
        subLayout->addWidget(slider, 0, 1);
        subLayout->addWidget(maxValueLabel, 0, 2);
        subLayout->addWidget(valueLabel, 0, 3);
    }
    else
    {
        subLayout->addWidget(maxValueLabel, 0, 0);
        subLayout->addWidget(slider, 1, 0);
        subLayout->addWidget(minValueLabel, 2, 0);
        subLayout->addWidget(valueLabel, 3, 0);
    }

    slider->setProperty("key", QString(key.c_str()));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", slider->value());

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeInteger(int)));

    // Connect slider value to the label
    QObject::connect(
        slider,
        &QSlider::valueChanged,
        this,
        [this, valueLabel, slider]
        {
            this->onSliderMapped(valueLabel, slider);
        });
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

        if(orientation)
        {
            subLayout->addWidget(resetButton, 0, 4);
        }
        else
        {
            subLayout->addWidget(resetButton, 4, 0);
        }

        // Connect reset button to the slider
        QObject::connect(resetButton, &QPushButton::clicked, this, [this, slider]{onResetIntegerMapped(slider);});
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
    auto* subLayout = new QGridLayout();
    layout.addLayout(subLayout, row, 1);

    std::array<QSpinBox*, 3> spinboxes {};

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(count) ; ++i)
    {
        auto* spinbox = new QSpinBox();
        spinbox->setObjectName(QString::fromStdString(key + "/" + std::to_string(i)));
        spinboxes[i] = spinbox;

        spinbox->setMinimum(min);
        spinbox->setMaximum(max);
        spinbox->setValue(defaultValue);

        spinbox->setProperty("key", QString(key.c_str()));
        spinbox->setProperty("count", count);
        spinbox->setProperty("defaultValue", spinbox->value());

        subLayout->addWidget(spinbox, 0, int(i));

        // Connect spinbox value with our editor
        QObject::connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onChangeInteger(int)));
    }

    QSpinBox* spinbox = spinboxes[0];
    spinbox->setObjectName(QString::fromStdString(key));

    // Set a property with a pointer on each member of the group
    for(std::size_t i = 0 ; i < std::size_t(count) ; ++i)
    {
        for(std::size_t j = 0 ; j < std::size_t(count) ; ++j)
        {
            const std::string propName = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(propName.c_str(), QVariant::fromValue<QSpinBox*>(spinboxes[j]));
        }
    }

    // Reset button
    if(addResetButton)
    {
        QPushButton* resetButton = this->createResetButton(key);

        layout.addWidget(resetButton, row, 2);

        // Connect reset button to the spinbox
        QObject::connect(resetButton, &QPushButton::clicked, this, [this, spinbox]{onResetIntegerMapped(spinbox);});
    }
}

//-----------------------------------------------------------------------------

void SParameters::updateEnumList(const std::vector<std::string>& _list, const std::string _key)
{
    std::vector<std::string> labels;
    std::vector<std::string> keys;
    for(const auto& s : _list)
    {
        const auto [left_part, right_part] = splitString(s);
        labels.push_back(left_part);
        if(!right_part.empty())
        {
            keys.push_back(right_part);
        }
        else
        {
            keys.push_back(left_part);
        }
    }

    this->blockSignals(true);

    QObject* widget = this->getParamWidget(_key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
    {
        combobox->clear();

        int idx = 0;
        for(const auto& l : labels)
        {
            combobox->insertItem(idx, QString::fromStdString(l));
            ++idx;
        }

        // Add optional data
        idx = 0;
        for(const auto& k : keys)
        {
            combobox->setItemData(idx, QString::fromStdString(k));
            ++idx;
        }
    }
    else if(auto* nonLinearSlider = qobject_cast<sight::ui::qt::widget::NonLinearSlider*>(widget);
            nonLinearSlider != nullptr)
    {
        std::vector<int> values;
        std::ranges::transform(labels, std::back_inserter(values), [](const std::string& s){return std::stoi(s);});
        nonLinearSlider->setValues(values);
        nonLinearSlider->setValue(values[0]);
    }

    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void SParameters::parseEnumString(
    const std::string& options,
    std::vector<std::string>& labels,
    std::vector<std::string>& keys,
    std::string separators
)
{
    const boost::char_separator<char> sep(separators.c_str());
    const boost::tokenizer<boost::char_separator<char> > tokens {options, sep};

    for(const auto& token : tokens)
    {
        const auto [left_part, right_part] = splitString(token);

        labels.push_back(left_part);
        if(!right_part.empty())
        {
            keys.push_back(right_part);
        }
        else
        {
            keys.push_back(left_part);
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
    auto* menu = new QComboBox();
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

    layout.addWidget(menu, row, 1);

    QObject::connect(menu, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeEnum(int)));

    //Set the comboBox to the default value
    menu->setCurrentText(QString::fromStdString(defaultValue));
}

//------------------------------------------------------------------------------

void SParameters::createSliderEnumWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    const std::string& defaultValue,
    const std::vector<std::string>& values,
    bool onRelease,
    bool orientation
)
{
    auto* subLayout = new QGridLayout();
    layout.addLayout(subLayout, row, 1);

    auto* slider = new sight::ui::qt::widget::NonLinearSlider();
    if(orientation)
    {
        slider->setOrientation(Qt::Horizontal);
    }
    else
    {
        slider->setOrientation(Qt::Vertical);
    }

    slider->setObjectName(QString::fromStdString(key));
    slider->setProperty("key", QString::fromStdString(key));
    std::vector<int> intValues;
    std::ranges::transform(values, std::back_inserter(intValues), [](const std::string& s){return std::stoi(s);});
    slider->setValues(intValues);
    slider->setValue(std::stoi(defaultValue));

    slider->setTracking(!onRelease);

    QFont font;
    font.setPointSize(7);
    font.setItalic(true);

    auto* minValueLabel = new QLabel();
    minValueLabel->setFont(font);
    minValueLabel->setText(QString::fromStdString(values.front()));
    minValueLabel->setToolTip("Minimum value.");
    minValueLabel->setObjectName(QString::fromStdString(key + "/minValueLabel"));

    auto* maxValueLabel = new QLabel();
    maxValueLabel->setFont(font);
    maxValueLabel->setText(QString::fromStdString(values.back()));
    maxValueLabel->setToolTip("Maximum value.");
    maxValueLabel->setObjectName(QString::fromStdString(key + "/maxValueLabel"));

    auto* valueLabel = new QLabel();
    valueLabel->setStyleSheet("QLabel { font: bold; }");
    valueLabel->setText(QString::number(slider->value()));
    valueLabel->setToolTip("Current value.");
    setLabelMinimumSize(valueLabel, intValues.front(), intValues.back());
    valueLabel->setObjectName(QString::fromStdString(key + "/valueLabel"));

    if(orientation)
    {
        subLayout->addWidget(minValueLabel, 0, 0);
        subLayout->addWidget(slider, 0, 1);
        subLayout->addWidget(maxValueLabel, 0, 2);
        subLayout->addWidget(valueLabel, 0, 3);
    }
    else
    {
        subLayout->addWidget(maxValueLabel, 0, 0);
        subLayout->addWidget(slider, 1, 0);
        subLayout->addWidget(minValueLabel, 2, 0);
        subLayout->addWidget(valueLabel, 3, 0);
    }

    QObject::connect(
        slider,
        &sight::ui::qt::widget::NonLinearSlider::valueChanged,
        [this, key, slider, valueLabel](int value)
        {
            if(!m_blockSignals)
            {
                this->signal<signals::IntegerChangedSignalType>(signals::INTEGER_CHANGED_SIG)->asyncEmit(value, key);
                this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(value, key);
                SIGHT_DEBUG("[EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << value << ", " << key << ")");
                this->signal<signals::EnumChangedSignalType>(signals::ENUM_CHANGED_SIG)
                ->asyncEmit(std::to_string(value), key);
                this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)
                ->asyncEmit(std::to_string(value), key);
                SIGHT_DEBUG("[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << value << ", " << key << ")");
                this->signal<signals::EnumChangedIndexSignalType>(signals::ENUM_INDEX_CHANGED_SIG)
                ->asyncEmit(int(slider->index()), key);
            }

            valueLabel->setText(QString::number(value));
        });
    QObject::connect(
        slider,
        &sight::ui::qt::widget::NonLinearSlider::rangeChanged,
        [ = ](int min, int max)
        {
            minValueLabel->setText(QString::number(min));
            maxValueLabel->setText(QString::number(max));
        });
}

//-----------------------------------------------------------------------------

void SParameters::createButtonBarEnumWidget(
    QGridLayout& layout,
    int row,
    const std::string& key,
    const std::string& defaultValue,
    const std::vector<enumButtonParam>& buttonList,
    const int requestedWidth,
    const int requestedHeight,
    const int hOffset,
    const std::string& style
)
{
    //create the grid to store the buttons
    auto* subLayout = new QGridLayout();
    if(hOffset != 0)
    {
        subLayout->setHorizontalSpacing(hOffset);
    }

    layout.addLayout(subLayout, row, 1);

    //create a button group to deactivate the buttons on selection
    auto* buttonBarGroup = new QButtonGroup(subLayout);
    buttonBarGroup->setObjectName(QString::fromStdString(key));

    //create the buttons from the provided list
    int buttonIndex = 0;
    for(const auto& buttonParam : buttonList)
    {
        auto* enumButton = new QToolButton();
        buttonBarGroup->addButton(enumButton);

        //The name needs to be the ky_value, to find it when the SParam is updated through a slot
        enumButton->setObjectName((QString::fromStdString(key + "_" + buttonParam.value)));

        enumButton->setIcon(QIcon(QString::fromStdString(buttonParam.iconPath)));
        enumButton->setToolTip(QString::fromStdString(buttonParam.label));
        enumButton->setMinimumSize(120, 24);
        enumButton->setCheckable(true);
        enumButton->setProperty("class", "buttonBar");
        enumButton->setProperty("value", QString::fromStdString(buttonParam.value));
        enumButton->setText(QString::fromStdString(buttonParam.label));

        if(style == "ToolButtonTextOnly")
        {
            enumButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
        }
        else if(style == "ToolButtonTextBesideIcon")
        {
            enumButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        }
        else if(style == "ToolButtonTextUnderIcon")
        {
            enumButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        }
        else if(style == "ToolButtonFollowStyle")
        {
            enumButton->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        }
        else
        {
            enumButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }

        // create an effect to make it gray when not selected, and full color when selected
        auto* effect = new QGraphicsColorizeEffect;
        effect->setColor(QColor(10, 10, 10));
        effect->setStrength(0.7);
        enumButton->setGraphicsEffect(effect);

        //the size depends on the configuration. xml > qss
        if(requestedWidth != 0 || requestedHeight != 0)
        {
            //the size is provided through the xml. Don't use the qss.
            const int width  = requestedWidth == 0 ? requestedHeight : requestedWidth;
            const int height = requestedHeight == 0 ? requestedWidth : requestedHeight;
            enumButton->setIconSize(
                QSize(
                    width,
                    height
                )
            );
        }
        else
        {
            //the size is not provided through the xml. Use the qss style.
            enumButton->setProperty("class", "buttonBarTouchFriendly");
        }

        //add the button in the grid at its place
        subLayout->addWidget(enumButton, 1, buttonIndex);

        //create the connection to fire signals when the button is clicked
        QObject::connect(
            enumButton,
            &QToolButton::clicked,
            [this, buttonParam, key, buttonIndex]
            {
                if(!m_blockSignals)
                {
                    this->signal<signals::EnumChangedSignalType>(signals::ENUM_CHANGED_SIG)
                    ->asyncEmit(buttonParam.value, key);
                    this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)
                    ->asyncEmit(buttonParam.value, key);
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << buttonParam.value << ", " << key << ")"
                    );
                    this->signal<signals::EnumChangedIndexSignalType>(signals::ENUM_INDEX_CHANGED_SIG)
                    ->asyncEmit(buttonIndex, key);
                }
            });

        //create connection to change the display (gray/full color) when the selection state changes
        QObject::connect(
            enumButton,
            &QAbstractButton::toggled,
            [ = ]
                (bool checked)
            {
                QGraphicsEffect* effect = enumButton->graphicsEffect();
                effect->setEnabled(!checked);
            });

        //set the default value
        if(buttonParam.value == defaultValue)
        {
            enumButton->toggle();
        }

        ++buttonIndex;
    }
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

void SParameters::setParameter(sight::ui::base::parameter_t val, std::string key)
{
    this->blockSignals(true);

    if(std::holds_alternative<bool>(val))
    {
        this->setBoolParameter(std::get<bool>(val), key);
    }
    else if(std::holds_alternative<sight::ui::base::color_t>(val))
    {
        this->setColorParameter(std::get<sight::ui::base::color_t>(val), key);
    }
    else if(std::holds_alternative<double>(val))
    {
        this->setDoubleParameter(std::get<double>(val), key);
    }
    else if(std::holds_alternative<sight::ui::base::double2_t>(val))
    {
        const auto values = std::get<sight::ui::base::double2_t>(val);
        this->setDouble2Parameter(values[0], values[1], key);
    }
    else if(std::holds_alternative<sight::ui::base::double3_t>(val))
    {
        const auto values = std::get<sight::ui::base::double3_t>(val);
        this->setDouble3Parameter(values[0], values[1], values[2], key);
    }
    else if(std::holds_alternative<sight::ui::base::int2_t>(val))
    {
        const auto values = std::get<sight::ui::base::int2_t>(val);
        this->setInt2Parameter(values[0], values[1], key);
    }
    else if(std::holds_alternative<sight::ui::base::int3_t>(val))
    {
        const auto values = std::get<sight::ui::base::int3_t>(val);
        this->setInt3Parameter(values[0], values[1], values[2], key);
    }
    else if(std::holds_alternative<std::string>(val))
    {
        this->setEnumParameter(std::get<std::string>(val), key);
    }
    else if(std::holds_alternative<int>(val))
    {
        // This can be either an int or a enum index
        // Solve the ambiguity by testing the widget type
        QObject* child = this->getParamWidget(key);

        auto* spinbox         = qobject_cast<QSpinBox*>(child);
        auto* slider          = qobject_cast<QSlider*>(child);
        auto* nonLinearSlider = qobject_cast<sight::ui::qt::widget::NonLinearSlider*>(child);

        if(spinbox != nullptr || slider != nullptr || nonLinearSlider != nullptr)
        {
            this->setIntParameter(std::get<int>(val), key);
        }
        else
        {
            this->setEnumIndexParameter(std::get<int>(val), key);
        }
    }
    else if(std::holds_alternative<sight::ui::base::enum_list_t>(val))
    {
        this->updateEnumList(std::get<sight::ui::base::enum_list_t>(val), key);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setBoolParameter(bool val, std::string key)
{
    this->blockSignals(true);
    QObject* child = this->getParamWidget(key);
    auto* checkbox = qobject_cast<QCheckBox*>(child);

    if(checkbox != nullptr)
    {
        checkbox->setCheckState(val ? Qt::Checked : Qt::Unchecked);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setColorParameter(std::array<std::uint8_t, 4> color, std::string key)
{
    this->blockSignals(true);
    QObject* child    = this->getParamWidget(key);
    auto* colorButton = qobject_cast<QPushButton*>(child);

    if(colorButton != nullptr)
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
    QObject* child = this->getParamWidget(key);

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        spinbox->setValue(val);
    }
    else if(slider != nullptr)
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
    QObject* child = this->getParamWidget(key);

    if(child != nullptr)
    {
        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();

        spin0->setValue(val0);
        spin1->setValue(val1);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setDouble3Parameter(double val0, double val1, double val2, std::string key)
{
    this->blockSignals(true);
    QObject* child = this->getParamWidget(key);

    if(child != nullptr)
    {
        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
        auto* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();

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
    QObject* child = this->getParamWidget(key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        spinbox->setValue(val);
    }
    else if(slider != nullptr)
    {
        slider->setValue(val);
    }
    else if(auto* nonLinearSlider = qobject_cast<sight::ui::qt::widget::NonLinearSlider*>(child);
            nonLinearSlider != nullptr)
    {
        nonLinearSlider->setValue(val);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setInt2Parameter(int val0, int val1, std::string key)
{
    this->blockSignals(true);
    QObject* child = this->getParamWidget(key);

    if(child != nullptr)
    {
        auto* spin0 = child->property("widget#0").value<QSpinBox*>();
        auto* spin1 = child->property("widget#1").value<QSpinBox*>();

        spin0->setValue(val0);
        spin1->setValue(val1);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setInt3Parameter(int val0, int val1, int val2, std::string key)
{
    this->blockSignals(true);
    QObject* widget = this->getParamWidget(key);

    if(widget != nullptr)
    {
        auto* spin0 = widget->property("widget#0").value<QSpinBox*>();
        auto* spin1 = widget->property("widget#1").value<QSpinBox*>();
        auto* spin2 = widget->property("widget#2").value<QSpinBox*>();

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

    QObject* widget = this->getParamWidget(key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
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
            SIGHT_WARN(std::string("value '") + val + "' isn't found in Enum ComboBox '" + key + "'.");
        }
    }
    else if(auto* nonLinearSlider = qobject_cast<sight::ui::qt::widget::NonLinearSlider*>(widget);
            nonLinearSlider != nullptr)
    {
        nonLinearSlider->setValue(std::stoi(val));
    }
    else if(auto* buttonGroup = qobject_cast<QButtonGroup*>(widget);
            buttonGroup != nullptr)
    {
        QObject* buttonObject = this->getParamWidget(key + "_" + val);
        auto* button          = qobject_cast<QToolButton*>(buttonObject);

        if(button != nullptr)
        {
            button->toggle();
        }
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::setEnumIndexParameter(int val, std::string key)
{
    this->blockSignals(true);

    QObject* widget = this->getParamWidget(key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
    {
        combobox->setCurrentIndex(val);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void SParameters::updateEnumRange(std::string options, std::string key)
{
    this->blockSignals(true);

    QObject* widget = this->getParamWidget(key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
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
        this->signal<signals::ColorChangedSignalType>(signals::COLOR_CHANGED_SIG)->asyncEmit(newColor, key);
        this->signal<signals::ChangedSignalType>(signals::PARAMETER_CHANGED_SIG)->asyncEmit(newColor, key);
        SIGHT_DEBUG(
            "[EMIT] " << signals::COLOR_CHANGED_SIG << "(" << int(newColor[0]) << ", "
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

void SParameters::updateIntMinParameter(int min, std::string key)
{
    QObject* child = this->getParamWidget(key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();
        auto* spin0     = child->property("widget#0").value<QSpinBox*>();
        spin0->setMinimum(min);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QSpinBox*>();
            spin1->setMinimum(min);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QSpinBox*>();
            spin2->setMinimum(min);
        }
    }
    else if(slider != nullptr)
    {
        slider->setMinimum(min);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void SParameters::updateIntMaxParameter(int max, std::string key)
{
    QObject* child = this->getParamWidget(key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();

        auto* spin0 = child->property("widget#0").value<QSpinBox*>();
        spin0->setMaximum(max);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QSpinBox*>();
            spin1->setMaximum(max);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QSpinBox*>();
            spin2->setMaximum(max);
        }
    }
    else if(slider != nullptr)
    {
        slider->setMaximum(max);
    }
    else
    {
        SIGHT_ERROR("Widget '" + key + "' must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void SParameters::updateDoubleMinParameter(double min, std::string key)
{
    QObject* child = this->getParamWidget(key);

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();

        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        spin0->setMinimum(min);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
            spin1->setMinimum(min);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();
            spin2->setMinimum(min);
        }
    }
    else if(slider != nullptr)
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

void SParameters::updateDoubleMaxParameter(double max, std::string key)
{
    QObject* child = this->getParamWidget(key);

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();

        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        spin0->setMaximum(max);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
            spin1->setMaximum(max);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();
            spin2->setMaximum(max);
        }
    }
    else if(slider != nullptr)
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

QObject* SParameters::getParamWidget(const std::string& key)
{
    auto qtContainer      = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());
    const QObject* object = qtContainer->getQtContainer();

    auto* child = object->findChild<QObject*>(QString::fromStdString(key));
    SIGHT_ERROR_IF("object '" + key + "' is not found", !child);

    return child;
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui::qt
