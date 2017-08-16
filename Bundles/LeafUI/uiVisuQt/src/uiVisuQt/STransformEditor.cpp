/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuQt/STransformEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/mat4x4.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

namespace uiVisu
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisu::STransformEditor, ::fwData::TransformationMatrix3D);

//------------------------------------------------------------------------------

STransformEditor::STransformEditor() noexcept :
    m_rotation("xyz"),
    m_translation("xyz")
{
    m_translationRange[0] = -300;
    m_translationRange[1] = +300;
    m_rotationRange[0]    = -180;
    m_rotationRange[1]    = 180;
}

//------------------------------------------------------------------------------

STransformEditor::~STransformEditor() noexcept
{

}

//------------------------------------------------------------------------------

void STransformEditor::configuring()
{
    static const ::boost::regex s_REGEX("[xyz][xyz]?[xyz]?");

    this->initialize();

    ::fwRuntime::ConfigurationElement::sptr cfgRotation = m_configuration->findConfigurationElement("rotation");
    if(cfgRotation)
    {
        std::string rotation = cfgRotation->getAttributeValue("enabled");
        if (!rotation.empty())
        {
            if(rotation == "no")
            {
                m_rotation = "";
            }
            else if(rotation == "yes")
            {
                m_rotation = "xyz";
            }
            else if(boost::regex_match(rotation, s_REGEX))
            {
                m_rotation = rotation;
            }
            else
            {
                SLM_ERROR("Attribute 'rotation' can only has value 'yes', 'no' or a combination of [xyz]");
            }
        }

        std::string min = cfgRotation->getAttributeValue("min");
        if (!min.empty())
        {
            m_rotationRange[0] = std::stoi(min);
        }

        std::string max = cfgRotation->getAttributeValue("max");
        if (!max.empty())
        {
            m_rotationRange[1] = std::stoi(max);
        }

    }

    ::fwRuntime::ConfigurationElement::sptr cfgTranslation = m_configuration->findConfigurationElement("translation");
    if(cfgTranslation)
    {
        std::string translation = cfgTranslation->getAttributeValue("enabled");
        if (!translation.empty())
        {
            if(translation == "no")
            {
                m_translation = "";
            }
            else if(translation == "yes")
            {
                m_translation = "xyz";
            }
            else if(boost::regex_match(m_translation, s_REGEX))
            {
                m_translation = translation;
            }
            else
            {
                SLM_ERROR("Attribute 'translation' can only has value 'yes', 'no' or a combination of [xyz]");
            }
        }

        std::string min = cfgTranslation->getAttributeValue("min");
        if (!min.empty())
        {
            m_translationRange[0] = std::stoi(min);
        }

        std::string max = cfgTranslation->getAttributeValue("max");
        if (!max.empty())
        {
            m_translationRange[1] = std::stoi(max);
        }
    }
}

//------------------------------------------------------------------------------

void STransformEditor::starting()
{
    const char* description[] = {"Translation X", "Translation Y", "Translation Z",
                                 "Rotation X", "Rotation Y", "Rotation Z"};

    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();

    qtContainer->setLayout( layout );

    for (unsigned int i = 0; i < MAX_SLIDER_INDEX; i++)
    {
        QHBoxLayout* sliderLayout = new QHBoxLayout();

        m_sliders[i].m_sliderValue     = new QLineEdit();
        m_sliders[i].m_labelMin        = new QLabel();
        m_sliders[i].m_labelMax        = new QLabel();
        m_sliders[i].m_labelDefinition = new QLabel();

        m_sliders[i].m_slider = new QSlider(Qt::Horizontal);
        m_sliders[i].m_slider->setTickInterval(1);

        m_sliders[i].m_labelDefinition->setText(description[i]);

        sliderLayout->addWidget( m_sliders[i].m_labelDefinition, 0);
        sliderLayout->addWidget( m_sliders[i].m_labelMin, 0);
        sliderLayout->addWidget( m_sliders[i].m_slider, 3);
        sliderLayout->addWidget( m_sliders[i].m_labelMax, 0);
        sliderLayout->addWidget( m_sliders[i].m_sliderValue, 1);

        layout->addLayout(sliderLayout, 0);
        QObject::connect(m_sliders[i].m_slider, SIGNAL(valueChanged(int )), this, SLOT(onSliderChanged(int)));
        QObject::connect(m_sliders[i].m_sliderValue, SIGNAL(editingFinished()), this, SLOT(onTextChanged()));
    }

    for (unsigned int i = POSITION_X; i <= POSITION_Z; i++)
    {
        m_sliders[i].m_labelMin->setText(std::to_string(m_translationRange[0]).c_str());
        m_sliders[i].m_labelMax->setText(std::to_string(m_translationRange[1]).c_str());
        m_sliders[i].m_slider->setRange(m_translationRange[0], m_translationRange[1]);
    }

    const char axes[] = {"xyzxyz"};

    for (unsigned int i = POSITION_X; i <= POSITION_Z; i++)
    {
        const bool visible = m_translation.find(axes[i]) != std::string::npos;
        m_sliders[i].m_sliderValue->setVisible( visible );
        m_sliders[i].m_labelMin->setVisible( visible );
        m_sliders[i].m_labelMax->setVisible( visible );
        m_sliders[i].m_labelDefinition->setVisible( visible );
        m_sliders[i].m_slider->setVisible( visible );
    }

    for (unsigned int i = ROTATION_X; i <= ROTATION_Z; i++)
    {
        m_sliders[i].m_labelMin->setText(std::to_string(m_rotationRange[0]).c_str());
        m_sliders[i].m_labelMax->setText(std::to_string(m_rotationRange[1]).c_str());
        m_sliders[i].m_slider->setRange(m_rotationRange[0], m_rotationRange[1]);
    }

    for (unsigned int i = ROTATION_X; i <= ROTATION_Z; i++)
    {
        const bool visible = m_rotation.find(axes[i]) != std::string::npos;
        m_sliders[i].m_sliderValue->setVisible( visible );
        m_sliders[i].m_labelMin->setVisible( visible );
        m_sliders[i].m_labelMax->setVisible( visible );
        m_sliders[i].m_labelDefinition->setVisible( visible );
        m_sliders[i].m_slider->setVisible( visible );
    }

    this->updateFromMatrix();
}

//------------------------------------------------------------------------------

void STransformEditor::stopping()
{
    for (unsigned int i = 0; i < MAX_SLIDER_INDEX; i++)
    {
        QObject::disconnect(m_sliders[i].m_slider, SIGNAL(valueChanged(int )), this, SLOT(onSliderChanged(int)));
        QObject::disconnect(m_sliders[i].m_sliderValue, SIGNAL(editingFinished()), this, SLOT(onTextChanged()));
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void STransformEditor::updating()
{
    this->updateFromMatrix();
}

//------------------------------------------------------------------------------

void STransformEditor::onSliderChanged(int value)
{
    ::fwData::TransformationMatrix3D::sptr matrix;
    if (this->isVersion2())
    {
        matrix = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");
    }
    else
    {
        matrix = this->getObject< ::fwData::TransformationMatrix3D >();
    }

    double rx = ::glm::radians<double>(m_sliders[ROTATION_X].m_slider->value());
    double ry = ::glm::radians<double>(m_sliders[ROTATION_Y].m_slider->value());
    double rz = ::glm::radians<double>(m_sliders[ROTATION_Z].m_slider->value());

    double tx = m_sliders[POSITION_X].m_slider->value();
    double ty = m_sliders[POSITION_Y].m_slider->value();
    double tz = m_sliders[POSITION_Z].m_slider->value();

    ::glm::dmat4x4 mat;
    mat                      = ::glm::translate(mat, ::glm::dvec3(tx, ty, tz));
    ::glm::dvec4 translation = mat[3];
    mat                      = ::glm::yawPitchRoll(ry, rx, rz) * mat;

    translation = mat[3];

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(matrix, mat);

    for (unsigned int i = 0; i < MAX_SLIDER_INDEX; i++)
    {
        m_sliders[i].m_sliderValue->setText(QString("%1").arg(m_sliders[i].m_slider->value()));
    }

    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void STransformEditor::onTextChanged()
{
    for (unsigned int i = 0; i < MAX_SLIDER_INDEX; i++)
    {
        QString string = m_sliders[i].m_sliderValue->text();
        m_sliders[i].m_slider->setValue(string.toInt());
    }
}

//------------------------------------------------------------------------------

void STransformEditor::updateFromMatrix()
{
    ::fwData::TransformationMatrix3D::sptr matrix;
    if (this->isVersion2())
    {
        matrix = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");
    }
    else
    {
        matrix = this->getObject< ::fwData::TransformationMatrix3D >();
    }
    SLM_ASSERT("Unable to get matrix", matrix);
    ::glm::dmat4x4 mat = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(matrix);

    ::glm::dquat quat(mat);
    ::glm::dvec3 angles = ::glm::eulerAngles(quat);

    ::glm::dvec4 translation = mat[3];

    // Block
    for (unsigned int i = 0; i < MAX_SLIDER_INDEX; i++)
    {
        QObject::disconnect(m_sliders[i].m_slider, SIGNAL(valueChanged(int )), this, SLOT(onSliderChanged(int)));
        QObject::disconnect(m_sliders[i].m_sliderValue, SIGNAL(editingFinished()), this, SLOT(onTextChanged()));
    }

    for (::glm::length_t i = POSITION_X, j = 0; i <= POSITION_Z; i++, ++j)
    {
        m_sliders[i].m_slider->setValue(static_cast<int>(translation[j]));
    }

    for (::glm::length_t i = ROTATION_X, j = 0; i <= ROTATION_Z; i++, ++j)
    {
        m_sliders[i].m_slider->setValue(static_cast<int>(::glm::degrees<double>(angles[j])));
    }

    for (unsigned int i = 0; i < MAX_SLIDER_INDEX; i++)
    {
        m_sliders[i].m_sliderValue->setText(QString("%1").arg(m_sliders[i].m_slider->value()));
    }

    for (unsigned int i = 0; i < MAX_SLIDER_INDEX; i++)
    {
        QObject::connect(m_sliders[i].m_slider, SIGNAL(valueChanged(int )), this, SLOT(onSliderChanged(int)));
        QObject::connect(m_sliders[i].m_sliderValue, SIGNAL(editingFinished()), this, SLOT(onTextChanged()));

    }
}

//------------------------------------------------------------------------------

} // namespace uiVisu
