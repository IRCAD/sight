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

#include "modules/ui/qt/image/WindowLevel.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/runtime/operations.hpp>

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>
#include <data/tools/helper/Composite.hpp>
#include <data/TransferFunction.hpp>

#include <service/macros.hpp>

#include <boost/math/special_functions/fpclassify.hpp>

#include <QApplication>
#include <QComboBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSignalMapper>
#include <QToolButton>
#include <QWidget>

#include <ui/qt/container/QtContainer.hpp>
#include <ui/qt/widget/QRangeSlider.hpp>

#include <functional>

namespace sight::modules::ui::qt::image
{

fwServicesRegisterMacro( ::sight::ui::base::IEditor, ::sight::modules::ui::qt::image::WindowLevel,
                         ::sight::data::Image)

static const service::IService::KeyType s_IMAGE_INOUT = "image";
static const service::IService::KeyType s_TF_INOUT = "tf";

static const std::string s_AUTO_WINDOWING_CONFIG   = "autoWindowing";
static const std::string s_ENABLE_SQUARE_TF_CONFIG = "enableSquareTF";

//------------------------------------------------------------------------------

WindowLevel::WindowLevel() noexcept :
    m_widgetDynamicRangeMin(-1024),
    m_widgetDynamicRangeWidth(4000),
    m_autoWindowing(false),
    m_enableSquareTF(true),
    m_helperTF(std::bind(&WindowLevel::updateTF, this))
{
}

//------------------------------------------------------------------------------

WindowLevel::~WindowLevel() noexcept
{
}

//------------------------------------------------------------------------------

void WindowLevel::configuring()
{
    this->initialize();

    const ConfigType srvConfig = this->getConfigTree();

    if (srvConfig.count("config.<xmlattr>"))
    {
        const ConfigType config = srvConfig.get_child("config.<xmlattr>");

        const std::string autoWindowing = config.get(s_AUTO_WINDOWING_CONFIG, "no");
        SLM_ASSERT("Bad value for 'autoWindowing' attribute. It must be 'yes' or 'no'!",
                   autoWindowing == "yes" || autoWindowing == "no");
        m_autoWindowing = (autoWindowing == "yes");

        const std::string enableSquareTF = config.get(s_ENABLE_SQUARE_TF_CONFIG, "yes");
        SLM_ASSERT("Bad value for 'enableSquareTF' attribute. It must be 'yes' or 'no'!",
                   enableSquareTF == "yes" || enableSquareTF == "no");
        m_enableSquareTF = (enableSquareTF == "yes");
    }
}

//------------------------------------------------------------------------------

void WindowLevel::starting()
{
    const data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist.", image);

    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QGridLayout* const layout = new QGridLayout();

    m_valueTextMin = new QLineEdit();
    QDoubleValidator* const minValidator = new QDoubleValidator(m_valueTextMin);
    m_valueTextMin->setValidator(minValidator);

    m_valueTextMax = new QLineEdit();
    QDoubleValidator* const maxValidator = new QDoubleValidator(m_valueTextMax);
    m_valueTextMax->setValidator(maxValidator);

    m_rangeSlider = new sight::ui::qt::widget::QRangeSlider();

    m_toggleTFButton = new QToolButton();
    QIcon ico;
    std::string squareIcon(core::runtime::getModuleResourceFilePath("module_ui_qt", "square.png").string());
    std::string rampIcon(core::runtime::getModuleResourceFilePath("module_ui_qt", "ramp.png").string());
    ico.addPixmap(QPixmap(QString::fromStdString(squareIcon)), QIcon::Normal, QIcon::On);
    ico.addPixmap(QPixmap(QString::fromStdString(rampIcon)), QIcon::Normal, QIcon::Off);
    m_toggleTFButton->setIcon(ico);
    m_toggleTFButton->setCheckable(true);
    m_toggleTFButton->setToolTip("Function style");

    m_toggleAutoButton = new QToolButton();
    QIcon icon;
    std::string windo(core::runtime::getModuleResourceFilePath("module_ui_qt", "windowing.svg").string());
    icon.addFile(QString::fromStdString(windo), QSize(), QIcon::Normal, QIcon::On);
    std::string nowindo(core::runtime::getModuleResourceFilePath("module_ui_qt", "nowindowing.svg").string());
    icon.addFile(QString::fromStdString(nowindo), QSize(), QIcon::Normal, QIcon::Off);
    m_toggleAutoButton->setIcon(icon);
    m_toggleAutoButton->setToolTip("Automatic Windowing");
    m_toggleAutoButton->setCheckable(true);
    m_toggleAutoButton->setChecked(m_autoWindowing);

    m_dynamicRangeSelection = new QToolButton();
    m_dynamicRangeSelection->setPopupMode(QToolButton::InstantPopup);

    m_dynamicRangeMenu = new QMenu(m_dynamicRangeSelection);
    QAction* const action1 = m_dynamicRangeMenu->addAction( "-1024; 1023" );
    QAction* const action2 = m_dynamicRangeMenu->addAction( "-100; 300" );
    QAction* const action3 = m_dynamicRangeMenu->addAction( "Fit W/L" );
    QAction* const action4 = m_dynamicRangeMenu->addAction( "Fit Data" ); // TODO
    //QAction *action5 = m_dynamicRangeMenu->addAction( "Custom ..." ); // TODO
    m_dynamicRangeSelection->setMenu(m_dynamicRangeMenu);

    action1->setData(QVariant(1));
    action2->setData(QVariant(2));
    action3->setData(QVariant(3));
    action4->setData(QVariant(4));
    //action5->setData(QVariant(5));

    layout->addWidget( m_rangeSlider,  0, 0, 1, -1 );
    layout->addWidget( m_valueTextMin, 1, 0 );
    layout->addWidget( m_toggleTFButton, 1, 1 );
    layout->addWidget( m_toggleAutoButton, 1, 2 );
    layout->addWidget( m_dynamicRangeSelection, 1, 3 );
    layout->addWidget( m_valueTextMax, 1, 4 );

    qtContainer->setLayout( layout );

    m_dynamicRangeSignalMapper = new QSignalMapper(this);

    // Set the visibility after the layout is created so it doesn't open its own window.
    m_toggleTFButton->setVisible(m_enableSquareTF);

    QObject::connect(m_valueTextMin, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);
    QObject::connect(m_valueTextMax, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);
    QObject::connect(m_rangeSlider, SIGNAL(sliderRangeEdited(double,double)), this,
                     SLOT(onWindowLevelWidgetChanged(double,double)));
    QObject::connect(m_toggleTFButton, &::QToolButton::toggled, this, &WindowLevel::onToggleTF);
    QObject::connect(m_toggleAutoButton, &::QToolButton::toggled, this, &WindowLevel::onToggleAutoWL);
    QObject::connect(m_dynamicRangeSelection, &::QToolButton::triggered, this,
                     &WindowLevel::onDynamicRangeSelectionChanged);

    const data::TransferFunction::sptr tf = this->getInOut< data::TransferFunction>(s_TF_INOUT);
    m_helperTF.setOrCreateTF(tf, image);

    this->updating();
}

//------------------------------------------------------------------------------

void WindowLevel::updating()
{
    const data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("'" + s_IMAGE_INOUT + "' does not exist.", image);
    const data::mt::ObjectReadLock imgLock(image);

    const bool imageIsValid = data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);
    this->setEnabled(imageIsValid);

    if(imageIsValid)
    {
        if(m_autoWindowing)
        {
            double min, max;
            data::tools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            this->updateImageWindowLevel(min, max);
        }

        const data::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
        SLM_ASSERT("TransferFunction null pointer", tf);
        const data::mt::ObjectReadLock tfLock(tf);
        data::TransferFunction::TFValuePairType minMax = tf->getWLMinMax();
        this->onImageWindowLevelChanged( minMax.first, minMax.second );
    }
}

//------------------------------------------------------------------------------

void WindowLevel::stopping()
{
    m_helperTF.removeTFConnections();

    QObject::disconnect(m_dynamicRangeSelection, &::QToolButton::triggered, this,
                        &WindowLevel::onDynamicRangeSelectionChanged);
    QObject::disconnect(m_toggleAutoButton, &::QToolButton::toggled, this, &WindowLevel::onToggleAutoWL);
    QObject::disconnect(m_toggleTFButton, &::QToolButton::toggled, this, &WindowLevel::onToggleTF);
    QObject::disconnect(m_rangeSlider, SIGNAL(sliderRangeEdited(double,double)), this,
                        SLOT(onWindowLevelWidgetChanged(double,double)));
    QObject::disconnect(m_valueTextMax, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);
    QObject::disconnect(m_valueTextMin, &::QLineEdit::editingFinished, this, &WindowLevel::onTextEditingFinished);

    this->destroy();
}

//------------------------------------------------------------------------------

void WindowLevel::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        {
            const data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
            SLM_ASSERT("'" + s_IMAGE_INOUT + "' does not exist.", image);

            const data::TransferFunction::sptr tf = this->getInOut< data::TransferFunction>(s_TF_INOUT);
            m_helperTF.setOrCreateTF(tf, image);
        }
        this->updating();
    }
}

//------------------------------------------------------------------------------

void WindowLevel::updateTF()
{
    this->updating();
}

//------------------------------------------------------------------------------

void WindowLevel::info( std::ostream& _sstream )
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------

WindowLevel::WindowLevelMinMaxType WindowLevel::getImageWindowMinMax()
{
    const data::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    SLM_ASSERT("TransferFunction null pointer", tf);
    const data::mt::ObjectReadLock tfLock(tf);
    return tf->getWLMinMax();
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

double WindowLevel::toWindowLevel(double _val)
{
    return m_widgetDynamicRangeMin + m_widgetDynamicRangeWidth * _val;
}

//------------------------------------------------------------------------------

void WindowLevel::updateImageWindowLevel(double _imageMin, double _imageMax)
{
    const data::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    const data::mt::ObjectWriteLock tfLock(tf);
    tf->setWLMinMax( data::TransferFunction::TFValuePairType(_imageMin,
                                                             _imageMax) );
    auto sig = tf->signal< data::TransferFunction::WindowingModifiedSignalType >(
        data::TransferFunction::s_WINDOWING_MODIFIED_SIG);
    {
        const core::com::Connection::Blocker block(m_helperTF.getTFWindowingConnection());
        sig->asyncEmit( tf->getWindow(), tf->getLevel());
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

    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is not defined.", image);

    switch (index)
    {
        case 0:
            break;
        case 1:         // -1024; 1023
            min = -1024;
            max = 1023;
            break;
        case 2:         // -100; 300
            min = -100;
            max = 300;
            break;
        case 3:         // Fit Window/Level
            min = std::min(wl.first, wl.second);
            max = std::max(wl.first, wl.second);
            break;
        case 4:         // Fit Image Range
        {
            const data::mt::ObjectReadLock imgLock(image);
            data::tools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
        }
        break;
        case 5:         // Custom : TODO
            break;
        default:
            SLM_ASSERT("Unknown range selector index", 0);
    }

    this->setWidgetDynamicRange(min, max);
    this->updateWidgetMinMax(wl.first, wl.second);
}

//------------------------------------------------------------------------------

void WindowLevel::onImageWindowLevelChanged(double _imageMin, double _imageMax)
{
    this->updateWidgetMinMax( _imageMin, _imageMax );
    this->updateTextWindowLevel( _imageMin, _imageMax );
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
    data::TransferFunction::sptr currentTF = m_helperTF.getTransferFunction();
    data::mt::ObjectWriteLock tfLock(currentTF);

    data::TransferFunction::sptr newTF;

    if( squareTF )
    {
        newTF = data::TransferFunction::New();
        data::TransferFunction::TFColor color(1., 1., 1., 1.);
        newTF->setName("SquareTF");
        newTF->addTFColor(0.0, color);
        newTF->addTFColor(1.0, color);
        newTF->setIsClamped(true);
    }
    else
    {
        if( m_previousTF )
        {
            newTF = m_previousTF;
        }
        else
        {
            newTF = data::TransferFunction::createDefaultTF();
        }
    }

    newTF->setWindow( currentTF->getWindow() );
    newTF->setLevel( currentTF->getLevel() );

    m_previousTF = data::Object::copy(currentTF);

    currentTF->deepCopy(newTF);

    // Send signal
    auto sig = currentTF->signal< data::TransferFunction::PointsModifiedSignalType >(
        data::TransferFunction::s_POINTS_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(m_helperTF.getTFUpdateConnection());
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void WindowLevel::onToggleAutoWL(bool autoWL)
{
    m_autoWindowing = autoWL;

    if (m_autoWindowing)
    {
        data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is not defined.", image);
        double min, max;
        data::tools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
        this->updateImageWindowLevel(min, max);
        this->onImageWindowLevelChanged(min, max);
    }
}

//------------------------------------------------------------------------------

void WindowLevel::onTextEditingFinished()
{
    double min, max;
    if(this->getWidgetDoubleValue(m_valueTextMin, min) && this->getWidgetDoubleValue(m_valueTextMax, max))
    {
        this->updateWidgetMinMax( min, max );
        this->updateImageWindowLevel(min, max);
    }
}

//------------------------------------------------------------------------------

bool WindowLevel::getWidgetDoubleValue(QLineEdit* widget, double& val)
{
    bool ok = false;
    val = widget->text().toDouble(&ok);

    QPalette palette;
    if (!ok)
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
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

}
