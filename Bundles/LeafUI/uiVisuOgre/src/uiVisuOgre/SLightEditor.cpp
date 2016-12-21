#include "uiVisuOgre/SLightEditor.hpp"

#include "uiVisuOgre/helper/Utils.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SLightEditor, ::fwData::Composite );

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SLightEditor::s_EDIT_LIGHT_SLOT = "editLight";

//------------------------------------------------------------------------------

SLightEditor::SLightEditor() throw()
{
    newSlot(s_EDIT_LIGHT_SLOT, &SLightEditor::editLight, this);
}

//------------------------------------------------------------------------------

SLightEditor::~SLightEditor() throw()
{
}

//------------------------------------------------------------------------------

void SLightEditor::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SLightEditor::starting() throw( ::fwTools::Failed )
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instantiated", container);

    m_lightNameLbl = new QLabel("No light selected", container);
    m_lightNameLbl->setAlignment(::Qt::AlignHCenter);

    m_lightTypeBox = new QComboBox(container);
    m_lightTypeBox->addItems(QStringList() <<
                             ::fwRenderOgre::ILight::s_POINT_LIGHT.c_str() <<
                             ::fwRenderOgre::ILight::s_DIRECTIONAL_LIGHT.c_str() <<
                             ::fwRenderOgre::ILight::s_SPOT_LIGHT.c_str());

    //TODO: enable the light type combo box when other light types are implemented.
    m_lightTypeBox->setEnabled(false);

    m_diffuseColorBtn = new QPushButton("Diffuse color", container);
    m_diffuseColorBtn->setEnabled(false);

    m_specularColorBtn = new QPushButton("Specular color", container);
    m_specularColorBtn->setEnabled(false);

    m_removeLightBtn = new QPushButton("Remove light", container);
    m_removeLightBtn->setEnabled(false);

    m_thetaSlider = new QSlider(::Qt::Horizontal, container);
    m_thetaSlider->setMinimum(0);
    m_thetaSlider->setMaximum(::fwRenderOgre::ILight::s_OFFSET_RANGE);
    m_thetaSlider->setEnabled(false);

    m_phiSlider = new QSlider(::Qt::Horizontal, container);
    m_phiSlider->setMinimum(0);
    m_phiSlider->setMaximum(::fwRenderOgre::ILight::s_OFFSET_RANGE);
    m_phiSlider->setEnabled(false);

    // Name of the selected light and its type
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_lightNameLbl);
    layout->addWidget(m_lightTypeBox);

    // Diffuse and specular colors
    QHBoxLayout* colorLayout = new QHBoxLayout();
    colorLayout->addWidget(m_diffuseColorBtn);
    colorLayout->addWidget(m_specularColorBtn);
    layout->addLayout(colorLayout);

    // Theta offset
    QHBoxLayout* thetaLayout = new QHBoxLayout();
    thetaLayout->addWidget(new QLabel("Theta offset"));
    thetaLayout->addWidget(m_thetaSlider);
    layout->addLayout(thetaLayout);

    // Phi offset
    QHBoxLayout* phiLayout = new QHBoxLayout();
    phiLayout->addWidget(new QLabel("Phi offset"));
    phiLayout->addWidget(m_phiSlider);
    layout->addLayout(phiLayout);

    layout->addWidget(m_removeLightBtn);

    container->setLayout(layout);

    QObject::connect(m_diffuseColorBtn, SIGNAL(clicked(bool)), this, SLOT(onEditDiffuseColor(bool)));
    QObject::connect(m_specularColorBtn, SIGNAL(clicked(bool)), this, SLOT(onEditSpecularColor(bool)));

    QObject::connect(m_thetaSlider, SIGNAL(valueChanged(int)), this, SLOT(onEditThetaOffset(int)));
    QObject::connect(m_phiSlider, SIGNAL(valueChanged(int)), this, SLOT(onEditPhiOffset(int)));

    QObject::connect(m_removeLightBtn, SIGNAL(clicked(bool)), this, SLOT(onRemoveLight(bool)));
}

//------------------------------------------------------------------------------

void SLightEditor::stopping() throw( ::fwTools::Failed )
{
    QObject::disconnect(m_diffuseColorBtn, SIGNAL(clicked(bool)), this, SLOT(onEditDiffuseColor(bool)));
    QObject::disconnect(m_specularColorBtn, SIGNAL(clicked(bool)), this, SLOT(onEditSpecularColor(bool)));

    QObject::disconnect(m_thetaSlider, SIGNAL(valueChanged(int)), this, SLOT(onEditThetaOffset(int)));
    QObject::disconnect(m_phiSlider, SIGNAL(valueChanged(int)), this, SLOT(onEditPhiOffset(int)));

    QObject::disconnect(m_removeLightBtn, SIGNAL(clicked(bool)), this, SLOT(onRemoveLight(bool)));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SLightEditor::updating() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void SLightEditor::onEditDiffuseColor(bool _checked)
{
    ::Ogre::ColourValue newDiffuseColor = this->editColor(m_currentLight->getDiffuseColor(),
                                                          "Light diffuse color");

    m_currentLight->setDiffuseColor(newDiffuseColor);
}

//------------------------------------------------------------------------------

void SLightEditor::onEditSpecularColor(bool _checked)
{
    ::Ogre::ColourValue newSpecularColor = this->editColor(m_currentLight->getSpecularColor(),
                                                           "Light specular color");

    m_currentLight->setSpecularColor(newSpecularColor);
}

//------------------------------------------------------------------------------

void SLightEditor::onEditThetaOffset(int _value)
{
    m_currentLight->setThetaOffset(static_cast<float>(_value));
}

//------------------------------------------------------------------------------

void SLightEditor::onEditPhiOffset(int _value)
{
    m_currentLight->setPhiOffset(static_cast<float>(_value));
}

//------------------------------------------------------------------------------

void SLightEditor::onRemoveLight(bool _checked)
{
    //TODO: Implement light removing method.
}

//------------------------------------------------------------------------------

void SLightEditor::editLight(::fwRenderOgre::ILight::sptr _lightAdaptor)
{
    SLM_ASSERT("The selected light adaptor doesn't exist.", _lightAdaptor);
    m_currentLight = _lightAdaptor;

    m_lightNameLbl->setText(m_currentLight->getName().c_str());
    m_lightTypeBox->setCurrentIndex(static_cast<int>(m_currentLight->getType()));

    m_diffuseColorBtn->setEnabled(true);
    m_specularColorBtn->setEnabled(true);

    if(!m_currentLight->isOrphanNode())
    {
        m_thetaSlider->setValue(static_cast<int>(m_currentLight->getThetaOffset()));
        m_thetaSlider->setEnabled(true);

        m_phiSlider->setValue(static_cast<int>(m_currentLight->getPhiOffset()));
        m_phiSlider->setEnabled(true);
    }
    else
    {
        m_thetaSlider->setEnabled(false);
        m_phiSlider->setEnabled(false);
    }

    m_removeLightBtn->setEnabled(true);
}

//------------------------------------------------------------------------------

::Ogre::ColourValue SLightEditor::editColor(const ::Ogre::ColourValue& _currentColor, const std::string& _title)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    QColor qColor = QColorDialog::getColor(::uiVisuOgre::helper::Utils::converOgreColorToQColor(_currentColor),
                                           container,
                                           _title.c_str());

    return ::uiVisuOgre::helper::Utils::convertQColorToOgreColor(qColor);
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
