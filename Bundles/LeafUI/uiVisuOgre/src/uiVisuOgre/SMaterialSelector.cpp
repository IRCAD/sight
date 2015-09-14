/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SMaterialSelector.hpp"

#include <fwData/Material.hpp>
#include <fwData/String.hpp>

#include <fwCom/Signal.hxx>

#include <fwComEd/helper/Field.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/Base.hpp>


#include <fwRenderOgre/Utils.hpp>

#include <material/Plugin.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreResourceManager.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SMaterialSelector, ::fwData::Reconstruction);

//------------------------------------------------------------------------------
SMaterialSelector::SMaterialSelector() throw()
{
}

//------------------------------------------------------------------------------

SMaterialSelector::~SMaterialSelector() throw()
{
}

//------------------------------------------------------------------------------

void SMaterialSelector::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instantiated", container);

    m_materialBox = new QComboBox(container);
    QLabel* currentMaterial = new QLabel(container);
    m_materialText = new QLabel(container);

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

    currentMaterial->setText("Current material : ");
    m_materialText->setText("/");

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->addWidget( currentMaterial);
    labelLayout->addWidget( m_materialText);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(labelLayout);
    layout->addWidget( m_materialBox);

    container->setLayout( layout );

    QObject::connect(m_materialBox, SIGNAL(activated(const QString &)), this,
                     SLOT(onSelectedModeItem(const QString &)));

    this->updating();
}

//------------------------------------------------------------------------------

void SMaterialSelector::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_materialBox, SIGNAL(activated(const QString &)), this,
                        SLOT(onSelectedModeItem(const QString &)));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SMaterialSelector::configuring() throw(::fwTools::Failed)
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SMaterialSelector::updating() throw(::fwTools::Failed)
{
    this->updateMaterial();
}

//------------------------------------------------------------------------------

void SMaterialSelector::swapping() throw(::fwTools::Failed)
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
        m_materialText->setText(field->value().c_str());
    }
    else
    {
        m_materialText->setText("/");
    }
}

//------------------------------------------------------------------------------

void SMaterialSelector::onSelectedModeItem(const QString& text)
{
    ::fwData::Reconstruction::sptr reconst = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material      = reconst->getMaterial();
    ::fwData::String::sptr string          = ::fwData::String::New();
    string->setValue(text.toStdString());

    m_materialText->setText(text);

    ::fwComEd::helper::Field helper(material);
    helper.setField("ogreMaterial", string);
    helper.notify();
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
