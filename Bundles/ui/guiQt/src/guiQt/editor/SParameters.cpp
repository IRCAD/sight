/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/editor/SParameters.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwDataTools/Color.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Object.hpp>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QString>
#include <QStyle>

namespace guiQt
{

namespace editor
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::guiQt::editor::SParameters );

static const ::fwCom::Signals::SignalKeyType BOOLEAN_CHANGED_SIG  = "boolChanged";
static const ::fwCom::Signals::SignalKeyType COLOR_CHANGED_SIG    = "colorChanged";
static const ::fwCom::Signals::SignalKeyType DOUBLE_CHANGED_SIG   = "doubleChanged";
static const ::fwCom::Signals::SignalKeyType DOUBLE2_CHANGED_SIG  = "double2Changed";
static const ::fwCom::Signals::SignalKeyType DOUBLE3_CHANGED_SIG  = "double3Changed";
static const ::fwCom::Signals::SignalKeyType INTEGER_CHANGED_SIG  = "intChanged";
static const ::fwCom::Signals::SignalKeyType INTEGER2_CHANGED_SIG = "int2Changed";
static const ::fwCom::Signals::SignalKeyType INTEGER3_CHANGED_SIG = "int3Changed";
static const ::fwCom::Signals::SignalKeyType ENUM_CHANGED_SIG     = "enumChanged";

//-----------------------------------------------------------------------------

SParameters::SParameters() noexcept
{
    newSignal< BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG);
    newSignal< ColorChangedSignalType>(COLOR_CHANGED_SIG);
    newSignal< DoubleChangedSignalType>(DOUBLE_CHANGED_SIG);
    newSignal< Double2ChangedSignalType>(DOUBLE2_CHANGED_SIG);
    newSignal< Double3ChangedSignalType>(DOUBLE3_CHANGED_SIG);
    newSignal< IntegerChangedSignalType>(INTEGER_CHANGED_SIG);
    newSignal< Integer2ChangedSignalType>(INTEGER2_CHANGED_SIG);
    newSignal< Integer3ChangedSignalType>(INTEGER3_CHANGED_SIG);
    newSignal< EnumChangedSignalType >(ENUM_CHANGED_SIG);

    m_signalMapper = new QSignalMapper(this);
    m_resetMapper  = new QSignalMapper(this);
}

//-----------------------------------------------------------------------------

SParameters::~SParameters() noexcept
{
}

//-----------------------------------------------------------------------------

void SParameters::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SParameters::starting()
{
    this->create();

    auto qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    QGridLayout* layout = new QGridLayout();

    ::fwServices::IService::ConfigType config = this->getConfigTree();
    const ::fwServices::IService::ConfigType& parametersCfg = config.get_child("parameters");

    int row = 0;

    // Create widgets
    BOOST_FOREACH( const auto& param, parametersCfg.equal_range("param") )
    {
        const ::fwServices::IService::ConfigType& cfg = param.second;

        const std::string name         = cfg.get< std::string >("<xmlattr>.name");
        const std::string key          = cfg.get< std::string >("<xmlattr>.key");
        const std::string type         = cfg.get< std::string >("<xmlattr>.type");
        const std::string defaultValue = cfg.get< std::string >("<xmlattr>.defaultValue");

        layout->addWidget(new QLabel(QString(name.c_str())), row, 0);

        if(type == "bool")
        {
            this->createBoolWidget(*layout, row, key, defaultValue);
        }
        else if(type == "color")
        {
            this->createColorWidget(*layout, row, key, defaultValue);
        }
        else if(type == "double" || type == "double2" || type == "double3")
        {
            const std::string widget = cfg.get< std::string >("<xmlattr>.widget", "spin");

            const double min          = cfg.get<double>("<xmlattr>.min", 0.);
            const double max          = cfg.get<double>("<xmlattr>.max", 1.);
            const double defaultValue = cfg.get<double>("<xmlattr>.defaultValue", 0.5);

            const int count = (type == "double3") ? 3 : (type == "double2" ? 2 : 1);

            if(widget == "spin")
            {
                this->createDoubleWidget(*layout, row, key, defaultValue, min, max, count);
            }
            else if(widget == "slider")
            {
                // We don't support multiple sliders because we will not have the room in a single row
                SLM_ASSERT("Count > 1 is not supported with sliders", count == 1);

                const std::uint8_t decimals = cfg.get< std::uint8_t >("<xmlattr>.decimals", 2);
                this->createDoubleSliderWidget(*layout, row, key, defaultValue, min, max, decimals);
            }
            else
            {
                SLM_ERROR("Unknown widget type : '" + widget + "' for " + name + ". Must be 'spin' or 'slider'.");
            }
        }
        else if(type == "int" || type == "int2" || type == "int3")
        {
            const std::string widget = cfg.get< std::string >("<xmlattr>.widget", "slider");

            const int min          = cfg.get<int>("<xmlattr>.min", 0);
            const int max          = cfg.get<int>("<xmlattr>.max", 100);
            const int defaultValue = cfg.get<int>("<xmlattr>.defaultValue", 50);

            const int count = (type == "int3") ? 3 : (type == "int2" ? 2 : 1);

            if(widget == "spin")
            {
                this->createIntegerSpinWidget(*layout, row, key, defaultValue, min, max, count);
            }
            else if(widget == "slider")
            {
                // We don't support multiple sliders because we will not have the room in a single row
                SLM_ASSERT("Count > 1 is not supported with sliders", count == 1);

                this->createIntegerSliderWidget(*layout, row, key, defaultValue, min, max);
            }
            else
            {
                SLM_ERROR("Unknown widget type : '" + widget + "' for " + name + ". Must be 'spin' or 'slider'.");
            }
        }
        else if(type == "enum")
        {
            const std::string options = cfg.get<std::string>("<xmlattr>.values");

            //split values separated by ',', ' ', ';'
            std::vector<std::string> values;
            std::vector<std::string> data;

            const ::boost::char_separator<char> sep(", ;");
            const ::boost::tokenizer< ::boost::char_separator<char> > tokens {options, sep};

            for(const auto& token : tokens)
            {
                //split again values separated by '='
                const ::boost::char_separator<char> subsep("=");
                const ::boost::tokenizer< ::boost::char_separator<char> > subtokens {token, subsep};
                auto it = subtokens.begin();

                if(it != subtokens.end() )
                {
                    values.push_back(*it);
                    ++it;
                }

                if(it != subtokens.end() )
                {
                    data.push_back(*it);
                    ++it;
                }
                else
                {
                    data.push_back(values.back());
                }
            }

            this->createEnumWidget(*layout, row, key, defaultValue, values, data);

        }
        ++row;
    }

    qtContainer->setLayout(layout);
}

//-----------------------------------------------------------------------------

void SParameters::updating()
{
}

//-----------------------------------------------------------------------------

void SParameters::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SParameters::onChangeEnum(int value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();

    const QComboBox* box = dynamic_cast<const QComboBox*>(sender);

    SLM_ASSERT("Wrong widget type", box);

    const QString data = box->itemData(value).toString();

    this->signal<EnumChangedSignalType>(ENUM_CHANGED_SIG)->asyncEmit(data.toStdString(),
                                                                     key.toStdString());
}

//-----------------------------------------------------------------------------

void SParameters::onChangeBoolean(int value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();
    const bool checked    = value == Qt::Checked;

    this->signal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG)->asyncEmit(checked, key.toStdString());
}

//------------------------------------------------------------------------------

void SParameters::onColorButton()
{
    QObject* sender = this->sender();

    // Create Color choice dialog.
    auto qtContainer         = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

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

        const std::array<std::uint8_t, 4> newColor = {{ static_cast<std::uint8_t>(colorQt.red()),
                                                        static_cast<std::uint8_t>(colorQt.green()),
                                                        static_cast<std::uint8_t>(colorQt.blue()),
                                                        static_cast<std::uint8_t>(colorQt.alpha()) }};
        this->signal<ColorChangedSignalType>(COLOR_CHANGED_SIG)->asyncEmit(newColor, key.toStdString());
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeInteger(int value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();
    const int count       = sender->property("count").toInt();

    SLM_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

    const QSpinBox* spinbox = dynamic_cast<const QSpinBox*>(sender);
    const QSlider* slider   = dynamic_cast<const QSlider*>(sender);
    SLM_ASSERT("Wrong widget type", spinbox || slider);

    if(count == 1)
    {
        this->signal<IntegerChangedSignalType>(INTEGER_CHANGED_SIG)->asyncEmit(value, key.toStdString());
    }
    else
    {
        int value1;
        int value2;

        if(spinbox)
        {
            const QSpinBox* spin1 = sender->property("widget#0").value< QSpinBox*>();
            const QSpinBox* spin2 = sender->property("widget#1").value< QSpinBox*>();

            value1 = spin1->value();
            value2 = spin2->value();
        }
        else
        {
            const QSlider* spin1 = sender->property("widget#0").value< QSlider*>();
            const QSlider* spin2 = sender->property("widget#1").value< QSlider*>();

            value1 = spin1->value();
            value2 = spin2->value();

        }
        if(count == 2)
        {
            this->signal<Integer2ChangedSignalType>(INTEGER2_CHANGED_SIG)->asyncEmit(value1, value2, key.toStdString());
        }
        else
        {
            int value3;
            if(spinbox)
            {
                const QSpinBox* spin3 = sender->property("widget#2").value< QSpinBox*>();
                value3 = spin3->value();
            }
            else
            {
                const QSlider* spin3 = sender->property("widget#2").value< QSlider*>();
                value3 = spin3->value();
            }

            this->signal<Integer3ChangedSignalType>(INTEGER3_CHANGED_SIG)->asyncEmit(value1, value2, value3,
                                                                                     key.toStdString());
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeDouble(double value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();
    const int count       = sender->property("count").toInt();

    SLM_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

    if(count == 1)
    {
        this->signal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG)->asyncEmit(value, key.toStdString());
    }
    else
    {
        const QDoubleSpinBox* spin1 = sender->property("widget#0").value< QDoubleSpinBox*>();
        const QDoubleSpinBox* spin2 = sender->property("widget#1").value< QDoubleSpinBox*>();

        const double value1 = spin1->value();
        const double value2 = spin2->value();

        if(count == 2)
        {
            this->signal<Double2ChangedSignalType>(DOUBLE2_CHANGED_SIG)->asyncEmit(value1, value2, key.toStdString());
        }
        else
        {
            const QDoubleSpinBox* spin3 = sender->property("widget#2").value< QDoubleSpinBox*>();
            const double value3         = spin3->value();

            this->signal<Double3ChangedSignalType>(DOUBLE3_CHANGED_SIG)->asyncEmit(value1, value2, value3,
                                                                                   key.toStdString());
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::onChangeDoubleSlider(int)
{
    const QSlider* sender = qobject_cast<QSlider*>(this->sender());
    const QString key     = sender->property("key").toString();

    const double realValue = getDoubleSliderValue(sender);

    this->signal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG)->asyncEmit(realValue, key.toStdString());
}

//-----------------------------------------------------------------------------

void SParameters::onSliderMapped(QWidget* widget)
{
    const QSlider* slider = qobject_cast<QSlider*>(m_signalMapper->mapping(widget));
    QLabel* label         = qobject_cast<QLabel*>(widget);
    if (label && slider)
    {
        label->setText(QString::number(slider->value()));
    }
}

//-----------------------------------------------------------------------------

void SParameters::onDoubleSliderMapped(QWidget* widget)
{
    const QSlider* slider = qobject_cast<QSlider*>(m_signalMapper->mapping(widget));
    QLabel* label         = qobject_cast<QLabel*>(widget);

    if (label && slider)
    {
        const double newValue = getDoubleSliderValue(slider);
        const int decimals    = slider->property("decimals").toInt();

        label->setText(QString::number(newValue, 'f', decimals));
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetBooleanMapped(QWidget* widget)
{
    const QPushButton* button = qobject_cast<QPushButton*>(m_resetMapper->mapping(widget));
    QCheckBox* checkbox       = qobject_cast<QCheckBox*>(widget);
    if (button && checkbox)
    {
        int value = checkbox->property("defaultValue").toInt();
        checkbox->setCheckState(::Qt::CheckState(value));

        const QString key = checkbox->property("key").toString();
        this->signal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG)->asyncEmit(value, key.toStdString());
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetColorMapped(QWidget* widget)
{
    const QPushButton* button = qobject_cast<QPushButton*>(m_resetMapper->mapping(widget));
    QPushButton* colourButton = qobject_cast<QPushButton*>(widget);
    if (button && colourButton)
    {
        const QColor color = colourButton->property("defaultValue").value<QColor>();
        const QString key  = colourButton->property("key").toString();

        int iconSize = colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(iconSize, iconSize);
        pix.fill(color);

        colourButton->setIcon(QIcon(pix));
        colourButton->setProperty("color", color);

        const std::array<std::uint8_t, 4> newColor = {{ static_cast<std::uint8_t>(color.red()),
                                                        static_cast<std::uint8_t>(color.green()),
                                                        static_cast<std::uint8_t>(color.blue()),
                                                        static_cast<std::uint8_t>(color.alpha()) }};
        this->signal<ColorChangedSignalType>(COLOR_CHANGED_SIG)->asyncEmit(newColor, key.toStdString());
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetIntegerMapped(QWidget* widget)
{
    const QPushButton* button = qobject_cast<QPushButton*>(m_resetMapper->mapping(widget));
    QSlider* slider           = dynamic_cast<QSlider*>(widget);
    QSpinBox* spinbox         = dynamic_cast<QSpinBox*>(widget);
    if (button && slider)
    {
        int value = slider->property("defaultValue").toInt();
        slider->setValue(value);
    }
    else if (button && spinbox)
    {
        int value = spinbox->property("defaultValue").toInt();
        spinbox->setValue(value);
    }
}

//-----------------------------------------------------------------------------

void SParameters::onResetDoubleMapped(QWidget* widget)
{
    const QPushButton* button = qobject_cast<QPushButton*>(m_resetMapper->mapping(widget));
    QDoubleSpinBox* spinbox   = qobject_cast<QDoubleSpinBox*>(widget);

    if (button && spinbox)
    {
        const QString key  = spinbox->property("key").toString();
        const double value = spinbox->property("defaultValue").toDouble();
        const int count    = spinbox->property("count").toInt();
        SLM_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        QDoubleSpinBox* spin1 = spinbox->property("widget#0").value< QDoubleSpinBox*>();
        spin1->setValue(value);

        if(count == 1)
        {
            this->signal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG)->asyncEmit(value, key.toStdString());
        }
        else
        {
            QDoubleSpinBox* spin2 = spinbox->property("widget#1").value< QDoubleSpinBox*>();
            spin2->setValue(value);

            if(count == 2)
            {
                this->signal<Double2ChangedSignalType>(DOUBLE2_CHANGED_SIG)->asyncEmit(value, value, key.toStdString());
            }
            else
            {
                QDoubleSpinBox* spin3 = spinbox->property("widget#2").value< QDoubleSpinBox*>();
                spin3->setValue(value);

                this->signal<Double3ChangedSignalType>(DOUBLE3_CHANGED_SIG)->asyncEmit(value, value, value,
                                                                                       key.toStdString());
            }
        }
    }
}

//-----------------------------------------------------------------------------

QPushButton* SParameters::createResetButton()
{
    QPushButton* resetButton = new QPushButton( "R" );
    resetButton->setFocusPolicy(Qt::NoFocus);
    resetButton->setToolTip("Reset to the default value.");
    resetButton->setMaximumWidth(20);

    // Connect reset button to the slider
    QObject::connect(resetButton, SIGNAL(clicked()), m_resetMapper, SLOT(map()));

    return resetButton;
}

//-----------------------------------------------------------------------------

void SParameters::createBoolWidget(QGridLayout& layout, int row,
                                   const std::string& key,
                                   const std::string& defaultValue)
{
    QCheckBox* checkbox = new QCheckBox();
    checkbox->setTristate(false);

    if(defaultValue == "true")
    {
        checkbox->setCheckState(Qt::Checked);

        this->signal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG)->asyncEmit(true, key);
    }
    else
    {
        this->signal<BooleanChangedSignalType>(BOOLEAN_CHANGED_SIG)->asyncEmit(false, key);
    }

    checkbox->setProperty("key", QString(key.c_str()));
    checkbox->setProperty("defaultValue", checkbox->checkState());

    // Reset button
    QPushButton* resetButton = this->createResetButton();

    layout.addWidget(checkbox, row, 2);
    layout.addWidget(resetButton, row, 5);

    QObject::connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(onChangeBoolean(int)));

    // Connect reset button to the slider
    m_resetMapper->setMapping(resetButton, checkbox);
    connect(m_resetMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onResetBooleanMapped(QWidget*)));
}

//-----------------------------------------------------------------------------

void SParameters::createColorWidget(QGridLayout& layout, int row, const std::string& key,
                                    const std::string& defaultValue)
{
    // Reset button
    QPushButton* resetButton = this->createResetButton();

    QPushButton* colourButton = new QPushButton("Color");
    colourButton->setToolTip(tr("Selected color"));
    colourButton->setMinimumSize(120, 35);

    std::string colorStr = "#ffffffff";
    if(!defaultValue.empty())
    {
        std::uint8_t color[4];

        ::fwDataTools::Color::hexaStringToRGBA(defaultValue, color);

        const std::array<std::uint8_t, 4> newColor = {{ color[0], color[1], color[2], color[3] }};
        this->signal<ColorChangedSignalType>(COLOR_CHANGED_SIG)->asyncEmit(newColor, key);
        colorStr = defaultValue;
    }

    std::uint8_t color[4];
    ::fwDataTools::Color::hexaStringToRGBA(colorStr, color);

    const int iconSize = colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(iconSize, iconSize);

    QColor colorQt(color[0], color[1], color[2], color[3]);
    pix.fill(colorQt);

    colourButton->setIcon(QIcon(pix));

    colourButton->setProperty("key", QString(key.c_str()));
    colourButton->setProperty("defaultValue", colorQt);
    colourButton->setProperty("color", colorQt);

    layout.addWidget(colourButton, row, 2);
    layout.addWidget(resetButton, row, 5);

    QObject::connect(colourButton, SIGNAL(clicked()), this, SLOT(onColorButton()));

    // Connect reset button to the button
    m_resetMapper->setMapping(resetButton, colourButton);
    connect(m_resetMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onResetColorMapped(QWidget*)));
}

//-----------------------------------------------------------------------------

void SParameters::createDoubleWidget(QGridLayout& layout, int row, const std::string& key,
                                     double defaultValue, double min, double max, int count)
{
    // Reset button
    QPushButton* resetButton = this->createResetButton();

    layout.addWidget(resetButton, row, 5);

    // Connect reset button to the slider
    connect(m_resetMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onResetDoubleMapped(QWidget*)));

    QDoubleSpinBox* spinboxes[3];

    // Spinboxes
    for(int i = 0; i < count; ++i)
    {
        QDoubleSpinBox* spinbox = new QDoubleSpinBox();
        spinboxes[i] = spinbox;

        this->signal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG)->asyncEmit(defaultValue, key);

        auto countDecimals = [](double _num) -> int
                             {
                                 std::stringstream out;
                                 out << _num;
                                 const std::string s = out.str();
                                 const std::string t = s.substr(s.find(".") + 1);
                                 return static_cast<int>(t.length());
                             };

        spinbox->setDecimals(std::max( std::max(countDecimals(min), countDecimals(max)), 2));

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
        m_resetMapper->setMapping(resetButton, spinbox);
    }

    // Set a property with a pointer on each member of the group
    for(int i = 0; i < count; ++i)
    {
        for(int j = 0; j < count; ++j)
        {
            const std::string propName = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(propName.c_str(), QVariant::fromValue< QDoubleSpinBox*>(spinboxes[j]));
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::createDoubleSliderWidget(QGridLayout& layout, int row, const std::string& key,
                                           double defaultValue, double min, double max, std::uint8_t decimals)
{
    // Reset button
    QPushButton* resetButton = this->createResetButton();

    layout.addWidget(resetButton, row, 5);

    // Connect reset button to the slider
    QObject::connect(m_resetMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onResetDoubleMapped(QWidget*)));

    int maxSliderValue = 1;
    for(std::uint8_t i = 0; i < decimals; ++i)
    {
        maxSliderValue *= 10;
    }

    const double valueRange = max - min;
    maxSliderValue *= valueRange;

    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(0);
    slider->setMaximum(maxSliderValue);

    int defaultSliderValue = int(((defaultValue - min) / valueRange) * double(slider->maximum()));
    slider->setValue(defaultSliderValue);

    this->signal<DoubleChangedSignalType>(DOUBLE_CHANGED_SIG)->asyncEmit(defaultValue, key);

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

    layout.addWidget( minValueLabel, row, 1 );
    layout.addWidget( slider, row, 2 );
    layout.addWidget( maxValueLabel, row, 3 );
    layout.addWidget( valueLabel, row, 4);
    layout.addWidget( resetButton, row, 5);

    slider->setProperty("key", QString(key.c_str()));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", slider->value());
    slider->setProperty("decimals", decimals);
    slider->setProperty("min", min);
    slider->setProperty("max", max);

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeDoubleSlider(int)));

    // Connect slider value to the label
    QObject::connect(slider, SIGNAL(valueChanged(int)), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(slider, valueLabel);
    QObject::connect(m_signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onDoubleSliderMapped(QWidget*)));

    m_resetMapper->setMapping(resetButton, slider);
    QObject::connect(m_resetMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onResetIntegerMapped(QWidget*)));

    const std::string propName = std::string("widget#0");
    slider->setProperty(propName.c_str(), QVariant::fromValue< QSlider*>(slider));
}

//-----------------------------------------------------------------------------

void SParameters::createIntegerSliderWidget(QGridLayout& layout, int row, const std::string& key,
                                            int defaultValue, int min, int max)
{
    // Reset button
    QPushButton* resetButton = this->createResetButton();

    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(defaultValue);

    this->signal<IntegerChangedSignalType>(INTEGER_CHANGED_SIG)->asyncEmit(defaultValue, key);

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

    layout.addWidget( minValueLabel, row, 1 );
    layout.addWidget( slider, row, 2 );
    layout.addWidget( maxValueLabel, row, 3 );
    layout.addWidget( valueLabel, row, 4);
    layout.addWidget( resetButton, row, 5);

    slider->setProperty("key", QString(key.c_str()));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", slider->value());

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onChangeInteger(int)));

    // Connect slider value to the label
    QObject::connect(slider, SIGNAL(valueChanged(int)), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(slider, valueLabel);
    QObject::connect(m_signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onSliderMapped(QWidget*)));

    m_resetMapper->setMapping(resetButton, slider);
    QObject::connect(m_resetMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onResetIntegerMapped(QWidget*)));

    const std::string propName = std::string("widget#0");
    slider->setProperty(propName.c_str(), QVariant::fromValue< QSlider*>(slider));
}

//-----------------------------------------------------------------------------

void SParameters::createIntegerSpinWidget(QGridLayout& layout, int row, const std::string& key,
                                          int defaultValue, int min, int max, int count)
{
    // Reset button
    QPushButton* resetButton = this->createResetButton();

    layout.addWidget(resetButton, row, 5);

    QSpinBox* spinboxes[3];

    // Spinboxes
    for(int i = 0; i < count; ++i)
    {
        QSpinBox* spinbox = new QSpinBox();
        spinboxes[i] = spinbox;

        spinbox->setMinimum(min);
        spinbox->setMaximum(max);
        spinbox->setValue(defaultValue);

        this->signal<IntegerChangedSignalType>(INTEGER_CHANGED_SIG)->asyncEmit(defaultValue, key);

        spinbox->setProperty("key", QString(key.c_str()));
        spinbox->setProperty("count", count);
        spinbox->setProperty("defaultValue", spinbox->value());

        layout.addWidget(spinbox, row, i + 2);

        // Connect spinbox value with our editor
        QObject::connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onChangeInteger(int)));

        m_resetMapper->setMapping(resetButton, spinbox);
        connect(m_resetMapper, SIGNAL(mapped(QWidget*)), this, SLOT(onResetIntegerMapped(QWidget*)));
    }

    // Set a property with a pointer on each member of the group
    for(int i = 0; i < count; ++i)
    {
        for(int j = 0; j < count; ++j)
        {
            const std::string propName = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(propName.c_str(), QVariant::fromValue< QSpinBox*>(spinboxes[j]));
        }
    }
}

//-----------------------------------------------------------------------------

void SParameters::createEnumWidget(QGridLayout& layout, int row, const std::string& key,
                                   const std::string& defaultValue,
                                   const std::vector<std::string>& values,
                                   const std::vector<std::string>& data)
{
    QComboBox* menu = new QComboBox();
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

    const double doubleValue = (double(slider->value()) / slider->maximum()) * (max - min) + min;

    return doubleValue;
}

//-----------------------------------------------------------------------------

}   //namespace editor
}   //namespace guiQt
