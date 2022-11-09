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

#include "modules/ui/qt/image/WindowLevel.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/path.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>
#include <ui/qt/widget/QRangeSlider.hpp>

#include <boost/math/special_functions/fpclassify.hpp>

#include <QApplication>
#include <QComboBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QWidget>

#include <cmath>
#include <functional>

namespace sight::module::ui::qt::image
{

static const core::com::Slots::SlotKeyType s_UPDATE_TF_SLOT = "updateTF";

static const std::string s_AUTO_WINDOWING_CONFIG   = "autoWindowing";
static const std::string s_ENABLE_SQUARE_TF_CONFIG = "enableSquareTF";

//------------------------------------------------------------------------------

WindowLevel::WindowLevel() noexcept
{
    newSlot(s_UPDATE_TF_SLOT, &WindowLevel::updateTF, this);
}

//------------------------------------------------------------------------------

WindowLevel::~WindowLevel() noexcept =
    default;

//------------------------------------------------------------------------------

void WindowLevel::configuring()
{
    this->initialize();

    const ConfigType srvConfig = this->getConfiguration();

    if(srvConfig.count("config.<xmlattr>") != 0U)
    {
        const ConfigType config = srvConfig.get_child("config.<xmlattr>");

        const std::string autoWindowing = config.get(s_AUTO_WINDOWING_CONFIG, "false");
        SIGHT_ASSERT(
            "Bad value for 'autoWindowing' attribute. It must be 'true' or 'false'!",
            autoWindowing == "true" || autoWindowing == "false"
        );
        m_autoWindowing = (autoWindowing == "true");

        const std::string enableSquareTF = config.get(s_ENABLE_SQUARE_TF_CONFIG, "true");
        SIGHT_ASSERT(
            "Bad value for 'enableSquareTF' attribute. It must be 'true' or 'false'!",
            enableSquareTF == "true" || enableSquareTF == "false"
        );
        m_enableSquareTF = (enableSquareTF == "true");
    }
}

//------------------------------------------------------------------------------

void WindowLevel::starting()
{
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);

        this->create();
        auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
            this->getContainer()
        );

        auto* const layout = new QGridLayout();

        m_valueTextMin = new QLineEdit();
        auto* const minValidator = new QDoubleValidator(m_valueTextMin);
        m_valueTextMin->setValidator(minValidator);

        m_valueTextMax = new QLineEdit();
        auto* const maxValidator = new QDoubleValidator(m_valueTextMax);
        m_valueTextMax->setValidator(maxValidator);

        m_rangeSlider = new sight::ui::qt::widget::QRangeSlider();

        m_toggleTFButton = new QToolButton();
        QIcon ico;
        std::string squareIcon(core::runtime::getModuleResourceFilePath(
                                   "sight::module::ui::qt",
                                   "square.png"
        ).string());
        std::string rampIcon(core::runtime::getModuleResourceFilePath("sight::module::ui::qt", "ramp.png").string());
        ico.addPixmap(QPixmap(QString::fromStdString(squareIcon)), QIcon::Normal, QIcon::On);
        ico.addPixmap(QPixmap(QString::fromStdString(rampIcon)), QIcon::Normal, QIcon::Off);
        m_toggleTFButton->setIcon(ico);
        m_toggleTFButton->setCheckable(true);
        m_toggleTFButton->setToolTip("Function style");

        m_toggleAutoButton = new QToolButton();
        QIcon icon;
        std::string win(core::runtime::getModuleResourceFilePath("sight::module::ui::qt", "windowing.svg").string());
        icon.addFile(QString::fromStdString(win), QSize(), QIcon::Normal, QIcon::On);
        std::string nowindo(core::runtime::getModuleResourceFilePath(
                                "sight::module::ui::qt",
                                "nowindowing.svg"
        ).string());
        icon.addFile(QString::fromStdString(nowindo), QSize(), QIcon::Normal, QIcon::Off);
        m_toggleAutoButton->setIcon(icon);
        m_toggleAutoButton->setToolTip("Automatic Windowing");
        m_toggleAutoButton->setCheckable(true);
        m_toggleAutoButton->setChecked(m_autoWindowing);

        m_dynamicRangeSelection = new QToolButton();
        m_dynamicRangeSelection->setPopupMode(QToolButton::InstantPopup);

        m_dynamicRangeMenu = new QMenu(m_dynamicRangeSelection);
        QAction* const action1 = m_dynamicRangeMenu->addAction("-1024; 1023");
        QAction* const action2 = m_dynamicRangeMenu->addAction("-100; 300");
        QAction* const action3 = m_dynamicRangeMenu->addAction("Fit W/L");
        QAction* const action4 = m_dynamicRangeMenu->addAction("Fit Data"); // TODO
        //QAction *action5 = m_dynamicRangeMenu->addAction( "Custom ..." ); // TODO
        m_dynamicRangeSelection->setMenu(m_dynamicRangeMenu);

        action1->setData(QVariant(1));
        action2->setData(QVariant(2));
        action3->setData(QVariant(3));
        action4->setData(QVariant(4));
        //action5->setData(QVariant(5));

        layout->addWidget(m_rangeSlider, 0, 0, 1, -1);
        layout->addWidget(m_valueTextMin, 1, 0);
        layout->addWidget(m_toggleTFButton, 1, 1);
        layout->addWidget(m_toggleAutoButton, 1, 2);
        layout->addWidget(m_dynamicRangeSelection, 1, 3);
        layout->addWidget(m_valueTextMax, 1, 4);

        qtContainer->setLayout(layout);

        // Set the visibility after the layout is created so it doesn't open its own window.
        m_toggleTFButton->setVisible(m_enableSquareTF);

        QObject::connect(m_valueTextMin, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);
        QObject::connect(m_valueTextMax, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);
        QObject::connect(
            m_rangeSlider,
            SIGNAL(sliderRangeEdited(double,double)),
            this,
            SLOT(onWindowLevelWidgetChanged(double,double))
        );
        QObject::connect(m_toggleTFButton, &::QToolButton::toggled, this, &WindowLevel::onToggleTF);
        QObject::connect(m_toggleAutoButton, &::QToolButton::toggled, this, &WindowLevel::onToggleAutoWL);
        QObject::connect(
            m_dynamicRangeSelection,
            &::QToolButton::triggered,
            this,
            &WindowLevel::onDynamicRangeSelectionChanged
        );
    }
    this->updating();
}

//------------------------------------------------------------------------------

void WindowLevel::updating()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);

    const bool imageIsValid = data::helper::MedicalImage::checkImageValidity(image.get_shared());
    this->setEnabled(imageIsValid);

    if(imageIsValid)
    {
        if(m_autoWindowing)
        {
            double min = NAN;
            double max = NAN;
            data::helper::MedicalImage::getMinMax(image.get_shared(), min, max);
            this->updateImageWindowLevel(min, max);
        }

        const auto tf                            = m_tf.const_lock();
        data::TransferFunction::min_max_t minMax = tf->windowMinMax();
        this->onImageWindowLevelChanged(minMax.first, minMax.second);
    }
}

//------------------------------------------------------------------------------

void WindowLevel::stopping()
{
    QObject::disconnect(
        m_dynamicRangeSelection,
        &::QToolButton::triggered,
        this,
        &WindowLevel::onDynamicRangeSelectionChanged
    );
    QObject::disconnect(m_toggleAutoButton, &::QToolButton::toggled, this, &WindowLevel::onToggleAutoWL);
    QObject::disconnect(m_toggleTFButton, &::QToolButton::toggled, this, &WindowLevel::onToggleTF);
    QObject::disconnect(
        m_rangeSlider,
        SIGNAL(sliderRangeEdited(double,double)),
        this,
        SLOT(onWindowLevelWidgetChanged(double,double))
    );
    QObject::disconnect(m_valueTextMax, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);
    QObject::disconnect(m_valueTextMin, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);

    this->destroy();
}

//------------------------------------------------------------------------------

void WindowLevel::updateTF()
{
    this->updating();
}

//------------------------------------------------------------------------------

void WindowLevel::info(std::ostream& _sstream)
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------

WindowLevel::WindowLevelMinMaxType WindowLevel::getImageWindowMinMax()
{
    const auto tf = m_tf.const_lock();
    SIGHT_ASSERT("TransferFunction null pointer", tf);
    return tf->windowMinMax();
}

//------------------------------------------------------------------------------
void WindowLevel::updateWidgetMinMax(double _imageMin, double _imageMax)
{
    const double rangeMin = this->fromWindowLevel(_imageMin);
    const double rangeMax = this->fromWindowLevel(_imageMax);

    m_rangeSlider->setPos(rangeMin, rangeMax);
}

//------------------------------------------------------------------------------

double WindowLevel::fromWindowLevel(double val)
{
    double valMin = m_widgetDynamicRangeMin;
    double valMax = valMin + m_widgetDynamicRangeWidth;

    valMin = std::min(val, valMin);
    valMax = std::max(val, valMax);

    this->setWidgetDynamicRange(valMin, valMax);

    const double res = (val - m_widgetDynamicRangeMin) / m_widgetDynamicRangeWidth;
    return res;
}

//------------------------------------------------------------------------------

double WindowLevel::toWindowLevel(double _val) const
{
    return m_widgetDynamicRangeMin + m_widgetDynamicRangeWidth * _val;
}

//------------------------------------------------------------------------------

void WindowLevel::updateImageWindowLevel(double _imageMin, double _imageMax)
{
    const auto tf = m_tf.lock();
    tf->setWindowMinMax(
        data::TransferFunction::min_max_t(
            _imageMin,
            _imageMax
        )
    );
    auto sig = tf->signal<data::TransferFunction::WindowingModifiedSignalType>(
        data::TransferFunction::s_WINDOWING_MODIFIED_SIG
    );
    {
        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_TF_SLOT)));
        sig->asyncEmit(tf->window(), tf->level());
    }
}

//------------------------------------------------------------------------------

void WindowLevel::onWindowLevelWidgetChanged(double _min, double _max)
{
    const double imageMin = this->toWindowLevel(_min);
    const double imageMax = this->toWindowLevel(_max);
    this->updateImageWindowLevel(imageMin, imageMax);
    this->updateTextWindowLevel(imageMin, imageMax);
}

//------------------------------------------------------------------------------

void WindowLevel::onDynamicRangeSelectionChanged(QAction* action)
{
    WindowLevelMinMaxType wl = this->getImageWindowMinMax();
    double min               = m_widgetDynamicRangeMin;
    double max               = m_widgetDynamicRangeWidth + min;
    int index                = action->data().toInt();

    const auto image = m_image.lock();
    SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);

    switch(index)
    {
        case 0:
            break;

        case 1: // -1024; 1023
            min = -1024;
            max = 1023;
            break;

        case 2: // -100; 300
            min = -100;
            max = 300;
            break;

        case 3: // Fit Window/Level
            min = std::min(wl.first, wl.second);
            max = std::max(wl.first, wl.second);
            break;

        case 4: // Fit Image Range
            data::helper::MedicalImage::getMinMax(image.get_shared(), min, max);
            break;

        case 5: // Custom : TODO
            break;

        default:
            SIGHT_ASSERT("Unknown range selector index", 0);
    }

    this->setWidgetDynamicRange(min, max);
    this->updateWidgetMinMax(wl.first, wl.second);
}

//------------------------------------------------------------------------------

void WindowLevel::onImageWindowLevelChanged(double _imageMin, double _imageMax)
{
    this->updateWidgetMinMax(_imageMin, _imageMax);
    this->updateTextWindowLevel(_imageMin, _imageMax);
}

//------------------------------------------------------------------------------

void WindowLevel::updateTextWindowLevel(double _imageMin, double _imageMax)
{
    m_valueTextMin->setText(QString("%1").arg(_imageMin));
    m_valueTextMax->setText(QString("%1").arg(_imageMax));
}

//------------------------------------------------------------------------------

void WindowLevel::onToggleTF(bool squareTF)
{
    const auto currentTF = m_tf.lock();

    data::TransferFunction::sptr newTF;

    if(squareTF)
    {
        newTF = data::TransferFunction::New();
        data::TransferFunction::color_t color(1., 1., 1., 1.);
        newTF->setName("SquareTF");

        auto tfData = newTF->pieces().emplace_back(data::TransferFunctionPiece::New());
        tfData->insert({0.0, color});
        tfData->insert({1.0, color});
        tfData->setClamped(true);
    }
    else
    {
        if(m_previousTF)
        {
            newTF = m_previousTF;
        }
        else
        {
            newTF = data::TransferFunction::createDefaultTF();
        }
    }

    newTF->setWindow(currentTF->window());
    newTF->setLevel(currentTF->level());

    m_previousTF = data::Object::copy(currentTF.get_shared());

    currentTF->deepCopy(newTF);

    // Send signal
    auto sig = currentTF->signal<data::TransferFunction::PointsModifiedSignalType>(
        data::TransferFunction::s_POINTS_MODIFIED_SIG
    );
    {
        const core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_TF_SLOT)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void WindowLevel::onToggleAutoWL(bool autoWL)
{
    m_autoWindowing = autoWL;

    if(m_autoWindowing)
    {
        const auto image = m_image.lock();
        SIGHT_ASSERT("inout '" << s_IMAGE << "' does not exist.", image);
        double min = NAN;
        double max = NAN;
        data::helper::MedicalImage::getMinMax(image.get_shared(), min, max);
        this->updateImageWindowLevel(min, max);
        this->onImageWindowLevelChanged(min, max);
    }
}

//------------------------------------------------------------------------------

void WindowLevel::onTextEditingFinished()
{
    double min = NAN;
    double max = NAN;
    if(sight::module::ui::qt::image::WindowLevel::getWidgetDoubleValue(
           m_valueTextMin,
           min
       ) && sight::module::ui::qt::image::WindowLevel::getWidgetDoubleValue(m_valueTextMax, max))
    {
        this->updateWidgetMinMax(min, max);
        this->updateImageWindowLevel(min, max);
    }
}

//------------------------------------------------------------------------------

bool WindowLevel::getWidgetDoubleValue(QLineEdit* widget, double& val)
{
    bool ok = false;
    val = widget->text().toDouble(&ok);

    QPalette palette;
    if(!ok)
    {
        palette.setBrush(QPalette::Base, QBrush(Qt::red));
    }
    else
    {
        palette.setBrush(QPalette::Base, QApplication::palette().brush(QPalette::Base));
    }

    widget->setPalette(palette);
    return ok;
}

//------------------------------------------------------------------------------

void WindowLevel::setWidgetDynamicRange(double min, double max)
{
    if(fabs(max - min) < 1.e-05)
    {
        max = min + 1.e-05;
    }

    m_widgetDynamicRangeMin   = min;
    m_widgetDynamicRangeWidth = max - min;

    m_dynamicRangeSelection->setText(QString("%1, %2 ").arg(min).arg(max));
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap WindowLevel::getAutoConnections() const
{
    return {
        {s_IMAGE, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_IMAGE, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_TF, data::TransferFunction::s_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF, data::TransferFunction::s_POINTS_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF, data::TransferFunction::s_WINDOWING_MODIFIED_SIG, s_UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
