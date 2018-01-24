/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SMaterialSelector.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Material.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/helper/Field.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <material/Plugin.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreResourceManager.h>
#include <OGRE/OgreTechnique.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuOgre::SMaterialSelector, ::fwData::Reconstruction);

const ::fwCom::Signals::SignalKeyType SMaterialSelector::s_SELECTED_SIG = "selected";

//------------------------------------------------------------------------------
SMaterialSelector::SMaterialSelector() noexcept
{
    newSignal< SelectedSignalType >( s_SELECTED_SIG );
}

//------------------------------------------------------------------------------

SMaterialSelector::~SMaterialSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SMaterialSelector::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer() );

    // Selection
    QLabel* currentMaterial = new QLabel();
    currentMaterial->setText("Current material : ");

    m_materialBox = new QComboBox();

    std::pair <std::string, std::string> elt;
    ::Ogre::ResourceManager::ResourceMapIterator iter = ::Ogre::MaterialManager::getSingleton().getResourceIterator();
    while(iter.hasMoreElements())
    {
        ::Ogre::ResourcePtr mat = iter.getNext();
        if (mat->getGroup() == ::material::s_MATERIAL_RESOURCEGROUP_NAME)
        {
            m_materialBox->addItem(QString::fromStdString(mat->getName()));
        }
    }
    m_materialBox->setCurrentIndex(0);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->addWidget( currentMaterial);
    labelLayout->addWidget( m_materialBox);

    // Reload
    m_reloadButton = new QPushButton("Reload");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout( labelLayout );
    layout->addWidget( m_reloadButton );

    qtContainer->setLayout( layout );

    this->updating();

    QObject::connect(m_materialBox, SIGNAL(activated(const QString&)), this,
                     SLOT(onSelectedModeItem(const QString&)));
    QObject::connect(m_reloadButton, SIGNAL(clicked()), this, SLOT(onReloadMaterial()));
}

//------------------------------------------------------------------------------

void SMaterialSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SMaterialSelector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SMaterialSelector::updating()
{
    this->updateMaterial();
}

//------------------------------------------------------------------------------

void SMaterialSelector::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SMaterialSelector::updateMaterial()
{
    ::fwData::Reconstruction::sptr reconst = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material      = reconst->getMaterial();
    ::fwData::Object::sptr fieldObj        = material->getField("ogreMaterial");
    if(fieldObj != nullptr)
    {
        ::fwData::String::sptr field = ::fwData::String::dynamicCast(fieldObj);
        m_materialBox->setCurrentText(field->value().c_str());
    }
}

//------------------------------------------------------------------------------

void SMaterialSelector::onSelectedModeItem(const QString& text)
{
    ::fwData::Reconstruction::sptr reconst = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material      = reconst->getMaterial();
    ::fwData::String::sptr string          = ::fwData::String::New();
    string->setValue(text.toStdString());

    ::fwDataTools::helper::Field helper(material);
    helper.setField("ogreMaterial", string);
    helper.notify();

    auto sig = this->signal<SelectedSignalType>(s_SELECTED_SIG);
    sig->asyncEmit(text.toStdString());
}

//------------------------------------------------------------------------------

void SMaterialSelector::onReloadMaterial()
{
    auto materialName = m_materialBox->currentText().toStdString();
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(materialName);

    if(!material)
    {
        OSLM_ERROR("Could not find material" << materialName);
    }

    material->reload();

    const ::Ogre::Material::Techniques& techniques = material->getTechniques();

    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        const ::Ogre::Technique::Passes& passes = tech->getPasses();

        for(const auto pass : passes)
        {
            SLM_ASSERT("No pass found", pass);

            if(!pass->getVertexProgramName().empty())
            {
                pass->getVertexProgram()->reload();
            }
            if(!pass->getGeometryProgramName().empty())
            {
                pass->getGeometryProgram()->reload();
            }
            if(!pass->getFragmentProgramName().empty())
            {
                pass->getFragmentProgram()->reload();
            }
        }

    }

}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
