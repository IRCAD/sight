#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Acquisition.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwCore/spyLog.hpp>
#include "opSofa/SofaMeshEditorSrv.hpp"
#include "fwData/Integer.hpp"
#include "fwData/Vector.hpp"
#include <QVBoxLayout>
#include <QSlider>
#include <QString>

#include "opSofa/KeyEvent.hpp"


namespace opSofa
{

REGISTER_SERVICE(::gui::editor::IEditor, ::opSofa::SofaMeshEditorSrv, ::fwData::Acquisition);

/**
 * @brief Constructor
 */
SofaMeshEditorSrv::SofaMeshEditorSrv() throw()
{
}


/**
 * @brief Destructor
 */
SofaMeshEditorSrv::~SofaMeshEditorSrv() throw()
{
}


/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void SofaMeshEditorSrv::configuring() throw ( ::fwTools::Failed )
{
    this->initialize();
}


/**
 * @brief Used to launch the service.
 */
void SofaMeshEditorSrv::starting() throw ( ::fwTools::Failed )
{
    //this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    assert(container);

    QSlider *slider = new QSlider(Qt::Horizontal, container);
    slider->setToolTip(tr("Select strength"));
    slider->setRange(0, 2000);
    slider->setTickInterval(500);
    slider->setTickPosition(QSlider::TicksBelow);

    KeyEvent *e = new KeyEvent();
    e->setParent(container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->addWidget(slider, 0);
    layout->addWidget(e, 1);

    container->setLayout(layout);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onStrengthSlider(int)));
    QObject::connect(e, SIGNAL(keyPress(QKeyEvent*)), this, SLOT(moveOrgan(QKeyEvent*)));
}


/**
 * @brief Used to stop the service.
 */
void SofaMeshEditorSrv::stopping() throw ( ::fwTools::Failed )
{
    this->getContainer()->clean();
    //this->destroy();
}


/**
 * @brief Called to do an action on the data associated to the service.
 */
void SofaMeshEditorSrv::updating() throw ( ::fwTools::Failed )
{
}


/**
 * @brief Called by a data to notify a service.
 *
 * @param _pMsg : Incoming message
 */
void SofaMeshEditorSrv::updating( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
    if (msg->hasEvent("NEW_RECONSTRUCTION_SELECTED")) {
        idReconstruction = ::fwData::String::dynamicConstCast(msg->getDataInfo("NEW_RECONSTRUCTION_SELECTED"));
    }
}

void SofaMeshEditorSrv::onStrengthSlider(int value)
{
    if (idReconstruction) {
        // Get acquisition
        ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
        SLM_ASSERT("Associated object is not an Acquisition", acq);

        ::fwData::Vector::NewSptr data;
        ::fwData::String::NewSptr v1(idReconstruction->value());
        ::fwData::Integer::NewSptr v2(value);
        data->push_back(v1);
        data->push_back(v2);
       
        // Notification
        ::fwServices::ObjectMsg::NewSptr msg;
        msg->addEvent("EDITOR_MESH_SOFA", data);
        ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);
    }
}

void SofaMeshEditorSrv::moveOrgan(QKeyEvent* event)
{
    // Get acquisition
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    SLM_ASSERT("Associated object is not an Acquisition", acq);

    static int x = 0;
    static int y = 0;
    static int z = 0;

    if (event->key() == Qt::Key_Up) y += 1;
    if (event->key() == Qt::Key_Down) y -= 1;
    if (event->key() == Qt::Key_Left) x -= 1;
    if (event->key() == Qt::Key_Right) x += 1;
    if (event->key() == Qt::Key_A) z += 1;
    if (event->key() == Qt::Key_Q) z -= 1;

    ::fwData::Vector::NewSptr data;
    ::fwData::String::NewSptr v1(idReconstruction->value());
    ::fwData::Integer::NewSptr v2(x);
    ::fwData::Integer::NewSptr v3(y);
    ::fwData::Integer::NewSptr v4(z);
    data->push_back(v1);
    data->push_back(v2);
    data->push_back(v3);
    data->push_back(v4);
   
    // Notification
    ::fwServices::ObjectMsg::NewSptr msg;
    msg->addEvent("MOVE_MESH_SOFA", data);
    ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);
}

}
