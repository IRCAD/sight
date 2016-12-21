#include "uiVisuOgre/SLightEditor.hpp"

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

const std::string SLightEditor::s_POINT_LIGHT       = "Point";
const std::string SLightEditor::s_DIRECTIONAL_LIGHT = "Directional";
const std::string SLightEditor::s_SPOT_LIGHT        = "Spot";

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

    m_lightTypeBox = new QComboBox(container);
    m_lightTypeBox->addItems(QStringList() <<
                             SLightEditor::s_POINT_LIGHT.c_str() <<
                             SLightEditor::s_DIRECTIONAL_LIGHT.c_str() <<
                             SLightEditor::s_SPOT_LIGHT.c_str());

    //TODO: enable the light type combo box when other light types are implemented.
    m_lightTypeBox->setEnabled(false);

    m_diffuseColorBtn  = new QPushButton("Diffuse color", container);
    m_specularColorBtn = new QPushButton("Specular color", container);
    m_thetaSlider      = new QSlider(::Qt::Horizontal, container);
    m_phiSlider        = new QSlider(::Qt::Horizontal, container);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_lightTypeBox);

    QHBoxLayout* colorLayout = new QHBoxLayout();
    colorLayout->addWidget(m_diffuseColorBtn);
    colorLayout->addWidget(m_specularColorBtn);

    layout->addLayout(colorLayout);
    layout->addWidget(m_thetaSlider);
    layout->addWidget(m_phiSlider);

    container->setLayout(layout);
}

//------------------------------------------------------------------------------

void SLightEditor::stopping() throw( ::fwTools::Failed )
{
    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SLightEditor::updating() throw( ::fwTools::Failed )
{

}

void SLightEditor::editLight(::fwRenderOgre::ILight::sptr _lightAdaptor)
{
    m_lightTypeBox->setCurrentIndex(static_cast<int>(_lightAdaptor->getType()));
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
