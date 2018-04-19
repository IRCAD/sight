/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/widget/ActivityDataView.hpp"

#include <fwActivities/IActivityValidator.hpp>
#include <fwActivities/IObjectValidator.hpp>
#include <fwActivities/IValidator.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwDataCamp/getObject.hpp>
#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwIO/ioTypes.hpp>

#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <QApplication>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPixmap>
#include <QPushButton>
#include <QStandardItem>
#include <QTableWidget>
#include <QVBoxLayout>

namespace uiMedDataQt
{
namespace widget
{

const int ActivityDataView::s_UID_ROLE = Qt::UserRole + 1;

//-----------------------------------------------------------------------------

ActivityDataView::ActivityDataView(QWidget* parent) :
    QTabWidget(parent)
{
}

//-----------------------------------------------------------------------------

ActivityDataView::~ActivityDataView()
{
}

//-----------------------------------------------------------------------------

void ActivityDataView::clear()
{
    m_importedObject.clear();
    m_treeWidgets.clear();
    QTabWidget::clear();
}

//-----------------------------------------------------------------------------

bool ActivityDataView::eventFilter(QObject* obj, QEvent* event)
{
    // get dropped data in tree widget
    if (event->type() == QEvent::Drop)
    {
        QDropEvent* dropEvent = static_cast<QDropEvent*>(event);

        size_t index = static_cast<size_t>(this->currentIndex());
        ::fwActivities::registry::ActivityRequirement requirement = m_activityInfo.requirements[index];
        QPointer<QTreeWidget> tree = m_treeWidgets[index];

        // get dropped item from event mimedata
        const QMimeData* qMimeData = dropEvent->mimeData();

        QByteArray encoded = qMimeData->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);

        QList<QTreeWidgetItem* > itemList;
        QTreeWidgetItem* item;

        // Get the dropped item
        while (!stream.atEnd())
        {
            int row, col;

            QMap<int,  QVariant> roleDataMap;

            stream >> row >> col >> roleDataMap;

            if(col == 0)
            {
                item = new QTreeWidgetItem();
                itemList.push_back(item);
            }

            QList<int> keys = roleDataMap.keys();
            for (int key : keys)
            {
                item->setData(col, key, roleDataMap[key]);
            }
        }

        // check if the limit number of data is exceeded
        int nbChild = tree->topLevelItemCount() + itemList.size();
        if (static_cast<unsigned int>(nbChild) > requirement.maxOccurs )
        {
            QMessageBox::warning(this, "Drop", "The maximum number of element is reached.\n"
                                 "You must remove one before adding another.");
        }
        else
        {
            // add the dropped item in the tree
            for (QTreeWidgetItem* itemToAdd: itemList)
            {
                itemToAdd->setFlags(itemToAdd->flags() & ~Qt::ItemIsDropEnabled);
                std::string uid = itemToAdd->data(int(ColumnType::NAME), s_UID_ROLE).toString().toStdString();
                if (!uid.empty())
                {
                    // insert the object if it is in the required type
                    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(uid));
                    if (obj && obj->isA(requirement.type))
                    {
                        // Insert the new object
                        tree->addTopLevelItem(itemToAdd);
                    }
                }
            }
        }
        return true;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Delete)
        {
            this->removeSelectedObjects();
            return true;
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);

}

//-----------------------------------------------------------------------------

void ActivityDataView::fillInformation(const ::fwActivities::registry::ActivityInfo& info)
{
    namespace ActReg = ::fwActivities::registry;

    m_activityInfo = info;
    this->clear();

    ActReg::ActivityInfo::RequirementsType reqVect = m_activityInfo.requirements;
    for(const ActReg::ActivityRequirement& req :  reqVect)
    {
        QVBoxLayout* layout = new QVBoxLayout();
        QWidget* widget     = new QWidget();
        widget->setLayout(layout);

        QHBoxLayout* infoLayout = new QHBoxLayout();
        layout->addLayout(infoLayout);

        QVBoxLayout* typeLayout = new QVBoxLayout();
        QVBoxLayout* txtLayout  = new QVBoxLayout();
        infoLayout->addLayout(typeLayout);
        infoLayout->addSpacerItem(new QSpacerItem(20, 0));
        infoLayout->addLayout(txtLayout, 1);

        ObjectIconMapType::iterator iter = m_objectIcons.find(req.type);
        if (iter != m_objectIcons.end())
        {
            QString filename = QString::fromStdString(iter->second);

            this->addTab(widget, QIcon(filename), QString::fromStdString(req.name));
            QLabel* icon = new QLabel();
            icon->setAlignment(Qt::AlignHCenter);
            QPixmap pixmap(filename);
            icon->setPixmap(pixmap.scaled(100, 100));
            typeLayout->addWidget(icon);
        }
        else
        {
            this->addTab(widget, QString::fromStdString(req.name));
        }

        QLabel* type = new QLabel(QString("<small>%1</small>").arg(QString::fromStdString(req.type)));
        type->setAlignment(Qt::AlignHCenter);
        typeLayout->addWidget(type);

        QLabel* name = new QLabel(QString("<h2>%1</h2>").arg(QString::fromStdString(req.name)));
        name->setStyleSheet("QLabel { font: bold; }");
        txtLayout->addWidget(name);

        QLabel* description = new QLabel(QString::fromStdString(req.description));
        description->setStyleSheet("QLabel { font: italic; }");
        txtLayout->addWidget(description);

        txtLayout->addStretch();

        QLabel* nb = new QLabel();
        nb->setStyleSheet("QLabel { font: bold; }");
        layout->addWidget(nb);

        QPointer<QTreeWidget> tree = new QTreeWidget();
        m_treeWidgets.push_back(tree);
        if (req.maxOccurs == 0)
        {
            nb->setText("No object is required, it will be automatically created.");
            tree->setEnabled(false);
        }
        else if (req.minOccurs == 1 && req.maxOccurs == 1)
        {
            nb->setText("One object is required: ");
        }
        else
        {
            QString nbObj("Number of required object");
            if (req.maxOccurs == std::numeric_limits<unsigned int>::max())
            {
                nbObj.append(QString(" >= %1").arg(req.minOccurs));
            }
            else
            {
                nbObj.append(QString(": [%1-%2]").arg(req.minOccurs).arg(req.maxOccurs));
            }
            nb->setText(nbObj);
        }

        QHBoxLayout* treeLayout   = new QHBoxLayout();
        QVBoxLayout* buttonLayout = new QVBoxLayout();
        if (req.type == "::fwData::String" || req.type == "::fwData::Boolean"
            || req.type == "::fwData::Integer" || req.type == "::fwData::Float"
            || req.type == "::fwData::TransformationMatrix3D")
        {
            QPushButton* buttonNew = new QPushButton("New");
            buttonNew->setToolTip("Create a new empty object");
            buttonLayout->addWidget(buttonNew);
            QObject::connect(buttonNew, &QPushButton::clicked, this, &ActivityDataView::createNewObject);
        }

        QPushButton* buttonAdd    = new QPushButton("Import");
        QPushButton* buttonRemove = new QPushButton("Remove");
        QPushButton* buttonClear  = new QPushButton("Clear");
        buttonLayout->addWidget(buttonAdd);
        buttonAdd->setToolTip(QString("Import a new object of type '%1'").arg(QString::fromStdString(req.type)));

        // If type is a series, we use add a button to import the data from a SeriesDB
        ::fwData::Object::sptr newObject = ::fwData::factory::New(req.type);
        if (newObject && ::fwMedData::Series::dynamicCast(newObject))
        {
            QPushButton* buttonAddFromSDB = new QPushButton("Import from SeriesDB");
            buttonLayout->addWidget(buttonAddFromSDB);
            buttonAddFromSDB->setToolTip(QString("Import a SeriesDB an extract the object of type '%1'").
                                         arg(QString::fromStdString(req.type)));
            QObject::connect(buttonAddFromSDB, &QPushButton::clicked, this, &ActivityDataView::importObjectFromSDB);
        }

        buttonLayout->addWidget(buttonRemove);
        buttonRemove->setToolTip(QString("Remove the selected objects"));
        buttonLayout->addWidget(buttonClear);
        buttonClear->setToolTip(QString("Remove all the objects"));
        buttonLayout->addStretch();
        QObject::connect(buttonAdd, &QPushButton::clicked, this, &ActivityDataView::importObject);
        QObject::connect(buttonRemove, &QPushButton::clicked, this, &ActivityDataView::removeSelectedObjects);
        QObject::connect(buttonClear, &QPushButton::clicked, this, &ActivityDataView::clearTree);
        treeLayout->addLayout(buttonLayout);

        QStringList headers;
        headers << "" << "object type" << "description" <<  "patient name" << "study description" << "" << "" << ""
                << "" << "" << "" << "";
        tree->setHeaderLabels(headers);

        treeLayout->addWidget(tree, 1);
        tree->setAlternatingRowColors(true);
        tree->setAcceptDrops(true);
        tree->setDragDropMode(QAbstractItemView::DropOnly);
        tree->viewport()->installEventFilter(this);
        tree->installEventFilter(this);

        QObject::connect(tree.data(), &QTreeWidget::itemDoubleClicked, this,
                         &ActivityDataView::onTreeItemDoubleClicked);
        layout->addLayout(treeLayout, 1);

        //TODO better management of composite container
    }

    for (int i = 1; i < this->count(); ++i)
    {
        this->setTabEnabled(i, false);
    }
}

//-----------------------------------------------------------------------------

void ActivityDataView::fillInformation(const ::fwMedData::ActivitySeries::sptr& activitySeries)
{
    namespace ActReg = ::fwActivities::registry;
    ::fwActivities::registry::ActivityInfo info;
    info           = ::fwActivities::registry::Activities::getDefault()->getInfo(activitySeries->getActivityConfigId());
    m_activityInfo = info;

    ::fwData::Composite::sptr data = activitySeries->getData();

    this->fillInformation(info);

    for (size_t i = 0; i < m_activityInfo.requirements.size(); ++i)
    {
        ::fwActivities::registry::ActivityRequirement req = m_activityInfo.requirements[i];

        ::fwData::Object::sptr obj = data->at< ::fwData::Object >(req.name);
        if (obj)
        {
            if ((req.minOccurs == 0 && req.maxOccurs == 0) ||
                (req.minOccurs == 1 && req.maxOccurs == 1) ||
                req.create)
            {
                this->addObjectItem(i, obj);
            }
            else
            {
                if (req.container == "vector")
                {
                    ::fwData::Vector::sptr vector = ::fwData::Vector::dynamicCast(obj);
                    if (vector)
                    {
                        for (auto subObj : vector->getContainer())
                        {
                            this->addObjectItem(i, subObj);
                        }
                    }
                    else
                    {
                        SLM_ERROR("Object param '" + req.name + "' must be a '::fwData::Vector'");
                    }
                }
                else // container == composite
                {
                    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast(obj);
                    if (composite)
                    {
                        for (auto subObj : composite->getContainer())
                        {
                            this->addObjectItem(i, subObj.second);
                        }
                    }
                    else
                    {
                        SLM_ERROR("Object param '" + req.name + "' must be a '::fwData::Composite'");
                    }
                }
            }
        }
        this->setTabEnabled(int(i), true);
    }
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr ActivityDataView::checkData(size_t index, std::string& errorMsg)
{
    ::fwData::Object::sptr data;

    ::fwActivities::registry::ActivityRequirement req = m_activityInfo.requirements[index];
    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    bool ok = true;
    if ((req.minOccurs == 1 && req.maxOccurs == 1) ||
        (req.minOccurs == 0 && req.maxOccurs == 0) ||
        req.create)      // One object is required
    {
        if (tree->topLevelItemCount() == 1)
        {
            QTreeWidgetItem* item = tree->topLevelItem(0);

            std::string uid =
                item->data(int(ColumnType::NAME), ActivityDataView::s_UID_ROLE).toString().toStdString();

            ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(uid));
            if (obj && obj->isA(req.type))
            {
                data = obj;
            }
            else
            {
                ok        = false;
                errorMsg += "\n - The parameter '" + req.name + "' must be a '" + req.type + "'.";
            }
        }
        else
        {
            if ((req.minOccurs == 0 && req.maxOccurs == 0) || req.create)
            {
                data = ::fwData::factory::New(req.type);
            }
            else
            {
                ok        = false;
                errorMsg += "\n - The parameter '" + req.name + "' is required but is not defined.";
            }
        }
    }
    else // optional object or several objects
    {
        unsigned int nbObj = static_cast<unsigned int>(tree->topLevelItemCount());

        if (nbObj < req.minOccurs)
        {
            ok        = false;
            errorMsg += "\n - The parameter '" + req.name + "' must contain at least " +
                        std::to_string(req.minOccurs) + " elements.";
        }
        else if (nbObj > req.maxOccurs)
        {
            ok        = false;
            errorMsg += "\n - The parameter '" + req.name + "' must contain at most " +
                        std::to_string(req.maxOccurs) + " elements.";
        }
        else
        {
            if (req.container == "vector")
            {
                ::fwData::Vector::sptr vector = ::fwData::Vector::New();

                for (unsigned int i = 0; i < nbObj; ++i)
                {
                    QTreeWidgetItem* itemData = tree->topLevelItem(int(i));
                    std::string uid           =
                        itemData->data(int(ColumnType::NAME), s_UID_ROLE).toString().toStdString();

                    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(uid));
                    if (obj  && obj->isA(req.type))
                    {
                        vector->getContainer().push_back(obj);
                    }
                    else
                    {
                        ok        = false;
                        errorMsg += "\n - The parameter '" + req.name + "' must be a " + req.type + ".";
                    }
                }
                if (ok)
                {
                    data = vector;
                }
            }
            else // container == composite
            {
                ::fwData::Composite::sptr composite = ::fwData::Composite::New();

                for (unsigned int i = 0; i < nbObj; ++i)
                {
                    QTreeWidgetItem* itemData = tree->topLevelItem(int(i));
                    std::string uid           =
                        itemData->data(int(ColumnType::NAME), s_UID_ROLE).toString().toStdString();

                    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(uid));
                    if (obj  && obj->isA(req.type))
                    {
                        std::string key  = req.keys[i].key;
                        std::string path = req.keys[i].path;
                        if(path.empty())
                        {
                            (*composite)[key] = obj;
                        }
                        else
                        {
                            (*composite)[key] = ::fwDataCamp::getObject( obj, path );
                        }
                    }
                    else
                    {
                        ok        = false;
                        errorMsg += "\n - The parameter '" + req.name + "' must be a " + req.type + ".";
                    }
                }
                if (ok)
                {
                    data = composite;
                }

            }
        }
    }

    if (data && !req.validator.empty())
    {
        /// Process object validator
        ::fwActivities::IValidator::sptr validator           = ::fwActivities::validator::factory::New(req.validator);
        ::fwActivities::IObjectValidator::sptr dataValidator = ::fwActivities::IObjectValidator::dynamicCast(validator);
        SLM_ASSERT("Validator '" + req.validator + "' instantiation failed", dataValidator);

        ::fwActivities::IValidator::ValidationType validation = dataValidator->validate(data);
        if(!validation.first)
        {
            errorMsg += "\n" + validation.second;
            data      = nullptr;
        }
    }

    return data;
}

//-----------------------------------------------------------------------------

bool ActivityDataView::checkAndComputeData(const ::fwMedData::ActivitySeries::sptr& actSeries, std::string& errorMsg)
{
    namespace ActReg = ::fwActivities::registry;

    ::fwData::Composite::sptr data = actSeries->getData();

    bool ok = true;
    errorMsg += "The required data are not correct:";

    // Check if all required data are present
    for (size_t i = 0; i < m_activityInfo.requirements.size(); ++i)
    {
        ::fwActivities::registry::ActivityRequirement req = m_activityInfo.requirements[i];
        std::string msg;
        ::fwData::Object::sptr obj = this->checkData(i, msg);
        if (obj)
        {
            (*data)[req.name] = obj;
        }
        else
        {
            ok        = false;
            errorMsg += msg;
        }
    }

    for (std::string validatotImpl : m_activityInfo.validatorsImpl)
    {
        /// Process activity validator
        ::fwActivities::IValidator::sptr validator = ::fwActivities::validator::factory::New(validatotImpl);

        ::fwActivities::IActivityValidator::sptr activityValidator =
            ::fwActivities::IActivityValidator::dynamicCast(validator);
        SLM_ASSERT("Validator '" + validatotImpl + "' instantiation failed", activityValidator);

        ::fwActivities::IValidator::ValidationType validation = activityValidator->validate(actSeries);
        if(!validation.first)
        {
            ok        = false;
            errorMsg += "\n" + validation.second;
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------

void ActivityDataView::removeSelectedObjects()
{
    size_t tabIndex               = static_cast<size_t>(this->currentIndex());
    QPointer<QTreeWidget> tree    = m_treeWidgets[tabIndex];
    QList<QTreeWidgetItem*> items = tree->selectedItems();
    for (QTreeWidgetItem* item: items)
    {
        if (item)
        {
            int itemIndex                 = tree->indexOfTopLevelItem(item);
            QTreeWidgetItem* itemToRemove = tree->takeTopLevelItem(itemIndex);
            if (itemToRemove)
            {
                delete itemToRemove;
                this->update();
            }
        }
    }
}

//-----------------------------------------------------------------------------

void ActivityDataView::clearTree()
{
    size_t tabIndex            = static_cast<size_t>(this->currentIndex());
    QPointer<QTreeWidget> tree = m_treeWidgets[tabIndex];
    tree->clear();
}

//-----------------------------------------------------------------------------

void ActivityDataView::createNewObject()
{
    size_t index = static_cast<size_t>(this->currentIndex());

    ::fwActivities::registry::ActivityRequirement req = m_activityInfo.requirements[index];

    std::string type = req.type;

    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    unsigned int nbItems = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nbItems >= req.maxOccurs)
    {
        QMessageBox::warning(this, "Import",
                             "The maximum number of element is reached.\n"
                             "You must remove one before adding another.");
        return;
    }
    ::fwData::Object::sptr newObject = ::fwData::factory::New(type);

    m_importedObject.push_back(newObject);
    this->addObjectItem(index, newObject);
}

//-----------------------------------------------------------------------------

void ActivityDataView::importObject()
{
    size_t index = static_cast<size_t>(this->currentIndex());

    ::fwActivities::registry::ActivityRequirement req = m_activityInfo.requirements[index];

    std::string type = req.type;

    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    unsigned int nbItems = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nbItems >= req.maxOccurs)
    {
        QMessageBox::warning(this, "Import",
                             "The maximum number of element is reached.\n"
                             "You must remove one before adding another.");
        return;
    }

    ::fwData::Object::sptr newObject = ::fwData::factory::New(type);
    if (newObject)
    {
        if (this->readObject(newObject, m_ioSelectorSrvConfig))
        {
            m_importedObject.push_back(newObject);

            this->addObjectItem(index, newObject);
        }
    }
    else
    {
        std::string msg = "Can not create object '" + type + "'";
        OSLM_ERROR(msg);
        QMessageBox messageBox(QMessageBox::Warning, "Error", QString::fromStdString(msg), QMessageBox::Ok);
    }
}

//-----------------------------------------------------------------------------

void ActivityDataView::importObjectFromSDB()
{
    size_t index = static_cast<size_t>(this->currentIndex());

    ::fwActivities::registry::ActivityRequirement req = m_activityInfo.requirements[index];

    std::string type = req.type;

    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    unsigned int nbItems = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nbItems >= req.maxOccurs)
    {
        QMessageBox::warning(this, "Import",
                             "The maximum number of element is reached.\n"
                             "You must remove one before adding another.");
        return;
    }

    ::fwData::Object::sptr newObject = ::fwData::factory::New(type);
    if (newObject)
    {
        OSLM_ERROR_IF("Imported object must inherit from 'Series'.", !::fwMedData::Series::dynamicCast(newObject));

        // We use the SeriesDB reader and then extract the object of this type.
        ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();
        if (this->readObject(seriesDB, m_sdbIoSelectorSrvConfig))
        {
            unsigned int nbImportedObj = 0;
            for (const ::fwMedData::Series::sptr& series : *seriesDB)
            {
                if (series->isA(type))
                {
                    ++nbImportedObj;
                    m_importedObject.push_back(series);

                    this->addObjectItem(index, series);

                    if (nbImportedObj >= req.maxOccurs)
                    {
                        break;
                    }
                }
            }
        }
    }
    else
    {
        std::string msg = "Can not create object '" + type + "'";
        OSLM_ERROR(msg);
        QMessageBox messageBox(QMessageBox::Warning, "Error", QString::fromStdString(msg), QMessageBox::Ok);
    }
}

//-----------------------------------------------------------------------------

bool ActivityDataView::readObject(const fwData::Object::sptr& obj, const std::string& ioSelectorSrvConfig)
{
    ::fwServices::IService::sptr ioSelectorSrv;
    ioSelectorSrv = ::fwServices::add("::uiIO::editor::SIOSelector");
    ioSelectorSrv->registerInOut(obj, ::fwIO::s_DATA_KEY);

    ::fwRuntime::ConfigurationElement::csptr ioCfg;
    ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(ioSelectorSrvConfig,
                                                                                  "::uiIO::editor::SIOSelector");

    bool isRead = true;
    try
    {
        ioSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(ioCfg) );
        ioSelectorSrv->configure();
        ioSelectorSrv->start();
        ioSelectorSrv->update();
        ioSelectorSrv->stop();
        ::fwServices::OSR::unregisterService( ioSelectorSrv );

        // check if object is properly read.
        // TODO : improve this test
        {
            ::fwData::Object::sptr tmpObject = ::fwData::factory::New(obj->getClassname());
            ::fwDataCamp::visitor::CompareObjects visitor;
            visitor.compare(tmpObject, obj);
            SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();

            if (props->empty())
            {
                SLM_WARN("Object of type '" + obj->getClassname() + "' is not read.");
                isRead = false;
            }
        }
    }
    catch(std::exception& e)
    {
        std::stringstream msg;
        msg << "The object can not be imported: " << e.what();
        SLM_ERROR(msg.str());

        QMessageBox messageBox(QMessageBox::Warning, "Error", QString::fromStdString(msg.str()), QMessageBox::Ok);
        if (ioSelectorSrv->isStarted())
        {
            ioSelectorSrv->stop();
        }
        ::fwServices::OSR::unregisterService( ioSelectorSrv );
        isRead = false;
    }
    return isRead;
}

//-----------------------------------------------------------------------------

void ActivityDataView::addObjectItem(size_t index, const ::fwData::Object::sptr& obj)
{
    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setFlags(newItem->flags() & ~Qt::ItemIsDropEnabled);
    newItem->setData(int(ColumnType::NAME), s_UID_ROLE, QVariant(QString::fromStdString(obj->getID())));
    newItem->setText(int(ColumnType::TYPE), QString::fromStdString(obj->getClassname()));

    // TODO add more information about object
    ::fwMedData::Series::sptr series           = ::fwMedData::Series::dynamicCast(obj);
    ::fwData::String::sptr strObj              = ::fwData::String::dynamicCast(obj);
    ::fwData::Integer::sptr intObj             = ::fwData::Integer::dynamicCast(obj);
    ::fwData::Float::sptr floatObj             = ::fwData::Float::dynamicCast(obj);
    ::fwData::Boolean::sptr boolObj            = ::fwData::Boolean::dynamicCast(obj);
    ::fwData::TransformationMatrix3D::sptr trf = ::fwData::TransformationMatrix3D::dynamicCast(obj);
    if (series)
    {
        newItem->setText(int(ColumnType::NAME), QString::fromStdString(series->getModality()));
        newItem->setText(int(ColumnType::DESC), QString::fromStdString(series->getDescription()));
        newItem->setText(int(ColumnType::PATIENT), QString::fromStdString(series->getPatient()->getName()));
        newItem->setText(int(ColumnType::STUDY), QString::fromStdString(series->getStudy()->getDescription()));
    }
    else if (strObj)
    {
        newItem->setText(int(ColumnType::DESC), QString::fromStdString(strObj->value()));
    }
    else if (intObj)
    {
        newItem->setText(int(ColumnType::DESC), QString("%1").arg(intObj->value()));
    }
    else if (floatObj)
    {
        newItem->setText(int(ColumnType::DESC), QString("%1").arg(floatObj->value()));
    }
    else if (boolObj)
    {
        newItem->setText(int(ColumnType::DESC), boolObj->value() ? "true" : "false");
    }
    else if (trf)
    {
        std::stringstream str;
        str << *trf;
        newItem->setText(int(ColumnType::DESC), QString::fromStdString(str.str()));
    }

    // set icon
    ObjectIconMapType::iterator iter = m_objectIcons.find(obj->getClassname());
    if (iter != m_objectIcons.end())
    {
        newItem->setIcon(int(ColumnType::NAME), QIcon(QString::fromStdString(iter->second)));
    }

    tree->addTopLevelItem(newItem);
    for (int i = 0; i < tree->columnCount(); ++i)
    {
        tree->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void ActivityDataView::onTreeItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (item)
    {
        std::string uid = item->data(int(ColumnType::NAME), s_UID_ROLE).toString().toStdString();
        if (!uid.empty())
        {
            ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(uid));
            if (obj)
            {
                if (obj->isA("::fwData::String"))
                {
                    ::fwData::String::sptr str = ::fwData::String::dynamicCast(obj);
                    bool isOkClicked = false;
                    QString value    = QInputDialog::getText(
                        this, "Edition", "Enter the String value:",
                        QLineEdit::Normal, QString::fromStdString(str->value()), &isOkClicked);

                    if ( isOkClicked)
                    {
                        str->value() = value.toStdString();
                        item->setText(int(ColumnType::DESC), value);
                    }
                }
                else if (obj->isA("::fwData::Integer"))
                {
                    ::fwData::Integer::sptr intObj = ::fwData::Integer::dynamicCast(obj);

                    bool isOkClicked = false;
                    int value        = QInputDialog::getInt(
                        this, "Edition", "Enter the Integer value:", intObj->value(), std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max(), 1, &isOkClicked);
                    if (isOkClicked)
                    {
                        intObj->value() = value;
                        item->setText(int(ColumnType::DESC), QString("%1").arg(value));
                    }
                }
                else if (obj->isA("::fwData::Float"))
                {
                    ::fwData::Float::sptr floatObj = ::fwData::Float::dynamicCast(obj);

                    bool isOkClicked = false;
                    double value     = QInputDialog::getDouble(
                        this, "Edition", "Enter the Integer value:", floatObj->value(), std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max(), 3, &isOkClicked);
                    if (isOkClicked)
                    {
                        floatObj->value() = static_cast<float>(value);
                        item->setText(int(ColumnType::DESC), QString("%1").arg(value));
                    }
                }
                else if (obj->isA("::fwData::Boolean"))
                {
                    ::fwData::Boolean::sptr boolObj = ::fwData::Boolean::dynamicCast(obj);
                    QMessageBox::StandardButton button = QMessageBox::question(
                        this, "Edition", "Defines the Boolean value");
                    boolObj->value() = (button == QMessageBox::Yes);
                    item->setText(int(ColumnType::DESC), boolObj->value() ? "true" : "false" );
                }
                else if (obj->isA("::fwData::TransformationMatrix3D"))
                {
                    ::fwData::TransformationMatrix3D::sptr trf = ::fwData::TransformationMatrix3D::dynamicCast(obj);
                    std::stringstream str;
                    str << *trf;

                    bool isOkClicked = false;
                    QString value    = QInputDialog::getMultiLineText(
                        this, "Edition", "Enter the Matrix coefficient (separated by a space):",
                        QString::fromStdString(str.str()), &isOkClicked);

                    QStringList coeffList = value.trimmed().split(QRegularExpression("\\s+"));
                    if (isOkClicked && coeffList.size() == 16)
                    {
                        ::fwData::TransformationMatrix3D::TMCoefArray coeffs;

                        bool conversionOK = false;
                        for (int i = 0; i < 16; ++i)
                        {
                            coeffs[size_t(i)] = coeffList[i].toDouble(&conversionOK);
                            if (!conversionOK)
                            {
                                QMessageBox::warning(this, "ERROR",
                                                     "This values cannot be converted to matrix coefficients");
                                return;
                            }
                        }
                        trf->setCoefficients(coeffs);
                        item->setText(int(ColumnType::DESC), value.trimmed() );
                    }
                    else if (isOkClicked)
                    {
                        QMessageBox::warning(this, "ERROR",
                                             "This values cannot be converted to matrix coefficients. It must contain "
                                             "16 values");
                        return;
                    }
                }
                else
                {
                    SLM_DEBUG("Object of type '" + obj->classname() + "' can not yet be editted");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedDataQt
