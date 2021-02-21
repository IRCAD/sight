/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "DataView.hpp"

#include <activity/IActivityValidator.hpp>
#include <activity/IObjectValidator.hpp>
#include <activity/IValidator.hpp>

#include <core/runtime/Convert.hpp>

#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/Matrix4.hpp>
#include <data/Patient.hpp>
#include <data/reflection/getObject.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>
#include <data/Study.hpp>
#include <data/Vector.hpp>

#include <service/IService.hpp>
#include <service/op/Add.hpp>
#include <service/registry/ObjectService.hpp>
#include <service/extension/Config.hpp>

#include <io/base/service/ioTypes.hpp>

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

#include <algorithm>
#include <iomanip>

namespace sight::module::ui::qt
{
namespace activity
{

const int DataView::s_UID_ROLE = Qt::UserRole + 1;

using sight::activity::extension::ActivityInfo;
using sight::activity::extension::ActivityRequirement;

//-----------------------------------------------------------------------------

DataView::DataView(QWidget* _parent) :
    QTabWidget(_parent)
{
}

//-----------------------------------------------------------------------------

DataView::~DataView()
{
}

//-----------------------------------------------------------------------------

void DataView::clear()
{
    m_importedObject.clear();
    m_treeWidgets.clear();
    QTabWidget::clear();
}

//-----------------------------------------------------------------------------

bool DataView::eventFilter(QObject* _obj, QEvent* _event)
{
    // get dropped data in tree widget
    if (_event->type() == QEvent::Drop)
    {
        QDropEvent* dropEvent = static_cast<QDropEvent*>(_event);

        size_t index                    = static_cast<size_t>(this->currentIndex());
        ActivityRequirement requirement = m_activityInfo.requirements[index];
        QPointer<QTreeWidget> tree      = m_treeWidgets[index];

        // get dropped item from event mimedata
        const QMimeData* qMimeData = dropEvent->mimeData();

        QByteArray encoded = qMimeData->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);

        QList<QTreeWidgetItem* > itemList;
        QTreeWidgetItem* item = nullptr;

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
                std::string uid = itemToAdd->data(int(ColumnCommunType::ID), s_UID_ROLE).toString().toStdString();
                if (!uid.empty())
                {
                    // insert the object if it is in the required type
                    data::Object::sptr obj = data::Object::dynamicCast(core::tools::fwID::getObject(uid));
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
    else if (_event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(_event);
        if (keyEvent->key() == Qt::Key_Delete)
        {
            this->removeSelectedObjects();
            return true;
        }
    }

    // standard event processing
    return QObject::eventFilter(_obj, _event);

}

//-----------------------------------------------------------------------------

void DataView::fillInformation(const ActivityInfo& _info)
{
    m_activityInfo = _info;
    this->clear();

    const ActivityInfo::RequirementsType& reqVect = m_activityInfo.requirements;
    for(const auto req : reqVect)
    {
        QVBoxLayout* const layout = new QVBoxLayout();
        QWidget* const widget     = new QWidget();
        widget->setLayout(layout);

        QHBoxLayout* const infoLayout = new QHBoxLayout();
        layout->addLayout(infoLayout);

        QVBoxLayout* const typeLayout = new QVBoxLayout();
        QVBoxLayout* const txtLayout  = new QVBoxLayout();
        infoLayout->addLayout(typeLayout);
        infoLayout->addSpacerItem(new QSpacerItem(20, 0));
        infoLayout->addLayout(txtLayout, 1);

        ObjectIconMapType::iterator iter = m_objectIcons.find(req.type);
        if (iter != m_objectIcons.end())
        {
            const QString filename = QString::fromStdString(iter->second);

            this->addTab(widget, QIcon(filename), QString::fromStdString(req.name));
            QLabel* const icon = new QLabel();
            icon->setAlignment(Qt::AlignHCenter);
            QPixmap pixmap(filename);
            icon->setPixmap(pixmap.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            typeLayout->addWidget(icon);
        }
        else
        {
            this->addTab(widget, QString::fromStdString(req.name));
        }

        QLabel* const type = new QLabel(QString("<small>%1</small>").arg(QString::fromStdString(req.type)));
        type->setAlignment(Qt::AlignHCenter);
        typeLayout->addWidget(type);

        QLabel* const name = new QLabel(QString("<h2>%1</h2>").arg(QString::fromStdString(req.name)));
        name->setStyleSheet("QLabel { font: bold; }");
        txtLayout->addWidget(name);

        QLabel* const description = new QLabel(QString::fromStdString(req.description));
        description->setStyleSheet("QLabel { font: italic; }");
        txtLayout->addWidget(description);

        txtLayout->addStretch();

        QLabel* const nb = new QLabel();
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

        QStringList headers;
        headers << "" << "Description";

        QHBoxLayout* const treeLayout   = new QHBoxLayout();
        QVBoxLayout* const buttonLayout = new QVBoxLayout();
        if (req.type == "::sight::data::String" || req.type == "::sight::data::Boolean"
            || req.type == "::sight::data::Integer" || req.type == "::sight::data::Float"
            || req.type == "::sight::data::Matrix4")
        {
            QPushButton* const buttonNew = new QPushButton("New");
            buttonNew->setToolTip("Create a new empty object");
            buttonLayout->addWidget(buttonNew);
            QObject::connect(buttonNew, &QPushButton::clicked, this, &DataView::createNewObject);
        }

        QPushButton* const buttonAdd    = new QPushButton("Load");
        QPushButton* const buttonRemove = new QPushButton("Remove");
        QPushButton* const buttonClear  = new QPushButton("Clear");
        buttonLayout->addWidget(buttonAdd);
        buttonAdd->setToolTip(QString("Load an object of type '%1'.").arg(QString::fromStdString(req.type)));

        // If the type is a Series, we add a button to import the data from a SeriesDB,
        // we also improve the tree header by adding more informations.
        data::Object::sptr newObject = data::factory::New(req.type);
        if (newObject && data::Series::dynamicCast(newObject))
        {
            QPushButton* const buttonAddFromSDB = new QPushButton("Import");
            buttonLayout->addWidget(buttonAddFromSDB);
            buttonAddFromSDB->setToolTip(QString("Import a SeriesDB and extract the N first objects of type '%1', with "
                                                 "N the maximum number of required objects.").
                                         arg(QString::fromStdString(req.type)));
            QObject::connect(buttonAddFromSDB, &QPushButton::clicked, this, &DataView::importObjectFromSDB);

            headers.clear();
            headers << "" << "Name" << "Sex" << "Birthdate"
                    << "Modality" << "Description"
                    << "Study description" << "Date" << "Time"
                    << "Patient age";

            if(data::ImageSeries::dynamicCast(newObject))
            {
                headers << "Body part examined" << "Patient position" << "Contrast agent" << "Acquisition time" <<
                    "Contrast/bolus time";
            }
        }

        buttonLayout->addWidget(buttonRemove);
        buttonRemove->setToolTip(QString("Remove the selected objects"));
        buttonLayout->addWidget(buttonClear);
        buttonClear->setToolTip(QString("Remove all the objects"));
        buttonLayout->addStretch();
        QObject::connect(buttonAdd, &QPushButton::clicked, this, &DataView::importObject);
        QObject::connect(buttonRemove, &QPushButton::clicked, this, &DataView::removeSelectedObjects);
        QObject::connect(buttonClear, &QPushButton::clicked, this, &DataView::clearTree);

        treeLayout->addLayout(buttonLayout);
        treeLayout->addWidget(tree, 1);

        tree->setHeaderLabels(headers);
        tree->setAlternatingRowColors(true);
        tree->setAcceptDrops(true);
        tree->setDragDropMode(QAbstractItemView::DropOnly);
        tree->viewport()->installEventFilter(this);
        tree->installEventFilter(this);

        QObject::connect(tree.data(), &QTreeWidget::itemDoubleClicked, this,
                         &DataView::onTreeItemDoubleClicked);
        layout->addLayout(treeLayout, 1);

        //TODO better management of composite container
    }

    for (int i = 1; i < this->count(); ++i)
    {
        this->setTabEnabled(i, false);
    }
}

//-----------------------------------------------------------------------------

void DataView::fillInformation(const data::ActivitySeries::sptr& _activitySeries)
{
    ActivityInfo info = sight::activity::extension::Activity::getDefault()->getInfo(
        _activitySeries->getActivityConfigId());
    m_activityInfo = info;

    data::Composite::sptr activitySeriesData = _activitySeries->getData();

    this->fillInformation(info);

    for (size_t i = 0; i < m_activityInfo.requirements.size(); ++i)
    {
        ActivityRequirement req = m_activityInfo.requirements[i];

        data::Object::sptr obj = activitySeriesData->at< data::Object >(req.name);
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
                    data::Vector::sptr vector = data::Vector::dynamicCast(obj);
                    if (vector)
                    {
                        for (auto subObj : vector->getContainer())
                        {
                            this->addObjectItem(i, subObj);
                        }
                    }
                    else
                    {
                        SLM_ERROR("Object param '" + req.name + "' must be a 'data::Vector'");
                    }
                }
                else // container == composite
                {
                    data::Composite::sptr composite = data::Composite::dynamicCast(obj);
                    if (composite)
                    {
                        for (auto subObj : composite->getContainer())
                        {
                            this->addObjectItem(i, subObj.second);
                        }
                    }
                    else
                    {
                        SLM_ERROR("Object param '" + req.name + "' must be a 'data::Composite'");
                    }
                }
            }
        }
        this->setTabEnabled(int(i), true);
    }
}

//-----------------------------------------------------------------------------

data::Object::sptr DataView::checkData(size_t _index, std::string& _errorMsg)
{
    data::Object::sptr object;

    ActivityRequirement req    = m_activityInfo.requirements[_index];
    QPointer<QTreeWidget> tree = m_treeWidgets[_index];

    bool ok = true;
    if ((req.minOccurs == 1 && req.maxOccurs == 1) ||
        (req.minOccurs == 0 && req.maxOccurs == 0) ||
        req.create)      // One object is required
    {
        if (tree->topLevelItemCount() == 1)
        {
            QTreeWidgetItem* item = tree->topLevelItem(0);

            std::string uid =
                item->data(int(ColumnCommunType::ID), DataView::s_UID_ROLE).toString().toStdString();

            data::Object::sptr obj = data::Object::dynamicCast(core::tools::fwID::getObject(uid));
            if (obj && obj->isA(req.type))
            {
                object = obj;
            }
            else
            {
                ok         = false;
                _errorMsg += "\n - The parameter '" + req.name + "' must be a '" + req.type + "'.";
            }
        }
        else
        {
            if ((req.minOccurs == 0 && req.maxOccurs == 0) || req.create)
            {
                object = data::factory::New(req.type);
            }
            else
            {
                ok         = false;
                _errorMsg += "\n - The parameter '" + req.name + "' is required but is not defined.";
            }
        }
    }
    else // optional object or several objects
    {
        unsigned int nbObj = static_cast<unsigned int>(tree->topLevelItemCount());

        if (nbObj < req.minOccurs)
        {
            ok         = false;
            _errorMsg += "\n - The parameter '" + req.name + "' must contain at least " +
                         std::to_string(req.minOccurs) + " elements.";
        }
        else if (nbObj > req.maxOccurs)
        {
            ok         = false;
            _errorMsg += "\n - The parameter '" + req.name + "' must contain at most " +
                         std::to_string(req.maxOccurs) + " elements.";
        }
        else
        {
            if (req.container == "vector")
            {
                data::Vector::sptr vector = data::Vector::New();

                for (unsigned int i = 0; i < nbObj; ++i)
                {
                    QTreeWidgetItem* itemData = tree->topLevelItem(int(i));
                    std::string uid           =
                        itemData->data(int(ColumnCommunType::ID), s_UID_ROLE).toString().toStdString();

                    data::Object::sptr obj = data::Object::dynamicCast(core::tools::fwID::getObject(uid));
                    if (obj  && obj->isA(req.type))
                    {
                        vector->getContainer().push_back(obj);
                    }
                    else
                    {
                        ok         = false;
                        _errorMsg += "\n - The parameter '" + req.name + "' must be a " + req.type + ".";
                    }
                }
                if (ok)
                {
                    object = vector;
                }
            }
            else // container == composite
            {
                data::Composite::sptr composite = data::Composite::New();

                for (unsigned int i = 0; i < nbObj; ++i)
                {
                    QTreeWidgetItem* itemData = tree->topLevelItem(int(i));
                    std::string uid           =
                        itemData->data(int(ColumnCommunType::ID), s_UID_ROLE).toString().toStdString();

                    data::Object::sptr obj = data::Object::dynamicCast(core::tools::fwID::getObject(uid));
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
                            (*composite)[key] = data::reflection::getObject( obj, path );
                        }
                    }
                    else
                    {
                        ok         = false;
                        _errorMsg += "\n - The parameter '" + req.name + "' must be a " + req.type + ".";
                    }
                }
                if (ok)
                {
                    object = composite;
                }

            }
        }
    }

    if (object && !req.validator.empty())
    {
        /// Process object validator
        auto validator     = sight::activity::validator::factory::New(req.validator);
        auto dataValidator = sight::activity::IObjectValidator::dynamicCast(validator);
        SLM_ASSERT("Validator '" + req.validator + "' instantiation failed", dataValidator);

        sight::activity::IValidator::ValidationType validation = dataValidator->validate(object);
        if(!validation.first)
        {
            _errorMsg += "\n" + validation.second;
            object     = nullptr;
        }
    }

    return object;
}

//-----------------------------------------------------------------------------

bool DataView::checkAndComputeData(const data::ActivitySeries::sptr& actSeries, std::string& errorMsg)
{
    data::Composite::sptr composite = actSeries->getData();

    bool ok = true;
    errorMsg += "The required data are not correct:";

    // Check if all required data are present
    for (size_t i = 0; i < m_activityInfo.requirements.size(); ++i)
    {
        ActivityRequirement req = m_activityInfo.requirements[i];
        std::string msg;
        data::Object::sptr obj = this->checkData(i, msg);
        if (obj)
        {
            (*composite)[req.name] = obj;
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
        auto validator         = sight::activity::validator::factory::New(validatotImpl);
        auto activityValidator = sight::activity::IActivityValidator::dynamicCast(validator);

        SLM_ASSERT("Validator '" + validatotImpl + "' instantiation failed", activityValidator);

        sight::activity::IValidator::ValidationType validation = activityValidator->validate(actSeries);
        if(!validation.first)
        {
            ok        = false;
            errorMsg += "\n" + validation.second;
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------

void DataView::removeSelectedObjects()
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

void DataView::clearTree()
{
    size_t tabIndex            = static_cast<size_t>(this->currentIndex());
    QPointer<QTreeWidget> tree = m_treeWidgets[tabIndex];
    tree->clear();
}

//-----------------------------------------------------------------------------

void DataView::createNewObject()
{
    size_t index = static_cast<size_t>(this->currentIndex());

    ActivityRequirement req = m_activityInfo.requirements[index];

    std::string type = req.type;

    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    unsigned int nbItems = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nbItems >= req.maxOccurs)
    {
        const QString message("Can't create more '"+ QString::fromStdString(type) +
                              "', please remove one to create another.");
        QMessageBox::warning(this, "New", message );
        return;
    }
    data::Object::sptr newObject = data::factory::New(type);

    m_importedObject.push_back(newObject);
    this->addObjectItem(index, newObject);
}

//-----------------------------------------------------------------------------

void DataView::importObject()
{
    const size_t index = static_cast<size_t>(this->currentIndex());

    ActivityRequirement req = m_activityInfo.requirements[index];

    const std::string type = req.type;

    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    const unsigned int nbItems = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nbItems >= req.maxOccurs)
    {
        const QString message("Can't load more '"+ QString::fromStdString(type) +
                              "', please remove one to load another.");
        QMessageBox::warning(this, "Import", message );
        return;
    }

    auto obj = this->readObject(type, m_ioSelectorSrvConfig);
    if (obj)
    {
        m_importedObject.push_back(obj);

        this->addObjectItem(index, obj);
    }
}

//-----------------------------------------------------------------------------

void DataView::importObjectFromSDB()
{
    const size_t index = static_cast<size_t>(this->currentIndex());

    ActivityRequirement req = m_activityInfo.requirements[index];

    const std::string type = req.type;

    QPointer<QTreeWidget> tree = m_treeWidgets[index];

    const unsigned int nbItems = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nbItems >= req.maxOccurs)
    {
        const QString message("Can't load more '"+ QString::fromStdString(type) +
                              "', please remove one to load another.");
        QMessageBox::warning(this, "Import from SeriesDB", message );
        return;
    }

    data::Object::sptr newObject = data::factory::New(type);
    if (newObject)
    {
        SLM_ERROR_IF("Imported object must inherit from 'Series'.", !data::Series::dynamicCast(newObject));

        // We use the SeriesDB reader and then extract the object of this type.
        auto obj      = this->readObject("::sight::data::SeriesDB", m_sdbIoSelectorSrvConfig);
        auto seriesDB = data::SeriesDB::dynamicCast(obj);
        if (seriesDB)
        {
            unsigned int nbImportedObj = 0;
            for (const data::Series::sptr& series : *seriesDB)
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
        SLM_ERROR(msg);
        QMessageBox messageBox(QMessageBox::Warning, "Error", QString::fromStdString(msg), QMessageBox::Ok);
    }
}

//-----------------------------------------------------------------------------

data::Object::sptr DataView::readObject(const std::string& _classname,
                                        const std::string& _ioSelectorSrvConfig)
{
    data::Object::sptr obj;
    service::IService::sptr ioSelectorSrv;
    ioSelectorSrv = service::add("::sight::module::ui::base::io::SSelector");

    core::runtime::ConfigurationElement::csptr ioCfg;
    ioCfg = service::extension::Config::getDefault()->getServiceConfig(_ioSelectorSrvConfig,
                                                                             "::sight::module::ui::base::io::SSelector");

    auto ioConfig  = core::runtime::Convert::toPropertyTree(ioCfg);
    auto srvConfig = ioConfig.get_child("config");
    srvConfig.add("type.<xmlattr>.class", _classname); // add the class of the output object

    try
    {
        ioSelectorSrv->setConfiguration(srvConfig);
        ioSelectorSrv->configure();
        ioSelectorSrv->setObjectId(io::base::service::s_DATA_KEY, "objRead");
        ioSelectorSrv->start();
        ioSelectorSrv->update();
        obj = ioSelectorSrv->getOutput< data::Object >(io::base::service::s_DATA_KEY);
        ioSelectorSrv->stop();
        service::OSR::unregisterService( ioSelectorSrv );
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
        service::OSR::unregisterService( ioSelectorSrv );
    }
    return obj;
}

//-----------------------------------------------------------------------------

void DataView::addObjectItem(size_t _index, const data::Object::csptr& _obj)
{
    QPointer<QTreeWidget> tree = m_treeWidgets[_index];

    QTreeWidgetItem* const newItem = new QTreeWidgetItem();
    newItem->setFlags(newItem->flags() & ~Qt::ItemIsDropEnabled);
    newItem->setData(int(ColumnCommunType::ID), s_UID_ROLE, QVariant(QString::fromStdString(_obj->getID())));

    const data::Series::csptr series   = data::Series::dynamicCast(_obj);
    const data::String::csptr strObj   = data::String::dynamicCast(_obj);
    const data::Integer::csptr intObj  = data::Integer::dynamicCast(_obj);
    const data::Float::csptr floatObj  = data::Float::dynamicCast(_obj);
    const data::Boolean::csptr boolObj = data::Boolean::dynamicCast(_obj);
    const data::Matrix4::csptr trf     = data::Matrix4::dynamicCast(_obj);
    if (series)
    {
        newItem->setText(int(ColumnSeriesType::NAME), QString::fromStdString(series->getPatient()->getName()));
        newItem->setText(int(ColumnSeriesType::SEX), QString::fromStdString(series->getPatient()->getSex()));
        std::string birthdate = series->getPatient()->getBirthdate();
        if(!birthdate.empty() && birthdate != "unknown")
        {
            birthdate.insert(4, "-");
            birthdate.insert(7, "-");
        }
        newItem->setText(int(ColumnSeriesType::BIRTHDATE), QString::fromStdString(birthdate));

        newItem->setText(int(ColumnSeriesType::MODALITY), QString::fromStdString(series->getModality()));
        newItem->setText(int(ColumnSeriesType::MODALITY_DESC), QString::fromStdString(series->getDescription()));

        newItem->setText(int(ColumnSeriesType::STUDY_DESC),
                         QString::fromStdString(series->getStudy()->getDescription()));
        std::string date = series->getDate();
        if(!date.empty())
        {
            date.insert(4, "/");
            date.insert(7, "/");
        }
        newItem->setText(int(ColumnSeriesType::DATE), QString::fromStdString(date));

        std::string time = series->getTime();
        if(!time.empty())
        {
            time.insert(2, ":");
            time.insert(5, ":");
        }
        newItem->setText(int(ColumnSeriesType::TIME), QString::fromStdString(time.substr(0, 8)));

        std::string patientAge = series->getStudy()->getPatientAge();
        if(!patientAge.empty())
        {
            patientAge.insert(3, " ");
            if(patientAge[0] == '0')
            {
                patientAge.erase(0, 1);
            }
        }
        newItem->setText(int(ColumnSeriesType::PATIENT_AGE), QString::fromStdString(patientAge));

        const data::ImageSeries::csptr imageSeries = data::ImageSeries::dynamicCast(_obj);
        if(imageSeries)
        {
            newItem->setText(int(ColumnImageSeriesType::BODY_PART_EXAMINED),
                             QString::fromStdString(imageSeries->getBodyPartExamined()));
            std::string patientPosition = imageSeries->getPatientPosition();
            if(!patientPosition.empty())
            {
                // Code string can contains leading or trailing spaces, we removed it frist.
                const std::string::const_iterator forward
                    = std::remove_if(patientPosition.begin(), patientPosition.end(), [&](unsigned char _c)
                        {
                            return _c == ' ';
                        });
                patientPosition.erase(forward, patientPosition.end());
                if(patientPosition.compare("HFP") == 0)
                {
                    patientPosition = "Head First-Prone";
                }
                else if(patientPosition.compare("HFS") == 0)
                {
                    patientPosition = "Head First-Supine";
                }
                else if(patientPosition.compare("HFDR") == 0)
                {
                    patientPosition = "Head First-Decubitus Right";
                }
                else if(patientPosition.compare("HFDL") == 0)
                {
                    patientPosition = "Head First-Decubitus Left";
                }
                else if(patientPosition.compare("FFDR") == 0)
                {
                    patientPosition = "Feet First-Decubitus Right";
                }
                else if(patientPosition.compare("FFDL") == 0)
                {
                    patientPosition = "Feet First-Decubitus Left";
                }
                else if(patientPosition.compare("FFP") == 0)
                {
                    patientPosition = "Feet First-Prone";
                }
                else if(patientPosition.compare("FFS") == 0)
                {
                    patientPosition = "Feet First-Supine";
                }
                else if(patientPosition.compare("LFP") == 0)
                {
                    patientPosition = "Left First-Prone";
                }
                else if(patientPosition.compare("LFS") == 0)
                {
                    patientPosition = "Left First-Supine";
                }
                else if(patientPosition.compare("RFP") == 0)
                {
                    patientPosition = "Right First-Prone";
                }
                else if(patientPosition.compare("RFS") == 0)
                {
                    patientPosition = "Right First-Supine";
                }
                else if(patientPosition.compare("AFDR") == 0)
                {
                    patientPosition = "Anterior First-Decubitus Right";
                }
                else if(patientPosition.compare("AFDL") == 0)
                {
                    patientPosition = "Anterior First-Decubitus Left";
                }
                else if(patientPosition.compare("PFDR") == 0)
                {
                    patientPosition = "Posterior First-Decubitus Right";
                }
                else if(patientPosition.compare("PFDL") == 0)
                {
                    patientPosition = "Posterior First-Decubitus Left";
                }
            }
            newItem->setText(int(ColumnImageSeriesType::PATIENT_POSITION),
                             QString::fromStdString(patientPosition));
            newItem->setText(int(ColumnImageSeriesType::CONTRAST_AGENT),
                             QString::fromStdString(imageSeries->getContrastAgent()));
            std::string acquisitionTime = imageSeries->getAcquisitionTime();
            if(!acquisitionTime.empty())
            {
                acquisitionTime.insert(2, ":");
                acquisitionTime.insert(5, ":");
            }
            newItem->setText(int(ColumnImageSeriesType::ACQUISITION_TIME),
                             QString::fromStdString(acquisitionTime.substr(0, 8)));
            std::string contrastTime = imageSeries->getContrastStartTime();
            if(!contrastTime.empty())
            {
                contrastTime.insert(2, ":");
                contrastTime.insert(5, ":");
            }
            newItem->setText(int(ColumnImageSeriesType::CONTRAST_BOLUS_START_TIME),
                             QString::fromStdString(contrastTime.substr(0, 8)));
        }
    }
    else if (strObj)
    {
        std::string description = strObj->value();
        if(description.empty())
        {
            description = _obj->getClassname();
        }
        newItem->setText(int(ColumnObjectType::DESC), QString::fromStdString(description));
    }
    else if (intObj)
    {
        newItem->setText(int(ColumnObjectType::DESC), QString("%1").arg(intObj->value()));
    }
    else if (floatObj)
    {
        newItem->setText(int(ColumnObjectType::DESC), QString("%1").arg(floatObj->value()));
    }
    else if (boolObj)
    {
        newItem->setText(int(ColumnObjectType::DESC), boolObj->value() ? "true" : "false");
    }
    else if (trf)
    {
        std::stringstream str;
        str << *trf;
        newItem->setText(int(ColumnObjectType::DESC), QString::fromStdString(str.str()));
    }
    else
    {
        newItem->setText(int(ColumnObjectType::DESC), QString::fromStdString(_obj->getClassname()));
    }

    // set icon
    ObjectIconMapType::iterator iter = m_objectIcons.find(_obj->getClassname());
    if (iter != m_objectIcons.end())
    {
        newItem->setIcon(int(ColumnCommunType::ID), QIcon(QString::fromStdString(iter->second)));
    }

    tree->addTopLevelItem(newItem);
    for (int i = 0; i < tree->columnCount(); ++i)
    {
        tree->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void DataView::onTreeItemDoubleClicked(QTreeWidgetItem* _item, int)
{
    if (_item)
    {
        std::string uid = _item->data(int(ColumnCommunType::ID), s_UID_ROLE).toString().toStdString();
        if (!uid.empty())
        {
            data::Object::sptr obj = data::Object::dynamicCast(core::tools::fwID::getObject(uid));
            if (obj)
            {
                if (obj->isA("::sight::data::String"))
                {
                    data::String::sptr str = data::String::dynamicCast(obj);
                    bool isOkClicked       = false;
                    QString value          = QInputDialog::getText(
                        this, "Edition", "Enter the String value:",
                        QLineEdit::Normal, QString::fromStdString(str->value()), &isOkClicked);

                    if ( isOkClicked)
                    {
                        str->value() = value.toStdString();
                        _item->setText(int(ColumnObjectType::DESC), value);
                    }
                }
                else if (obj->isA("::sight::data::Integer"))
                {
                    data::Integer::sptr intObj = data::Integer::dynamicCast(obj);

                    bool isOkClicked = false;
                    int value        = QInputDialog::getInt(
                        this, "Edition", "Enter the Integer value:", intObj->value(), std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max(), 1, &isOkClicked);
                    if (isOkClicked)
                    {
                        intObj->value() = value;
                        _item->setText(int(ColumnObjectType::DESC), QString("%1").arg(value));
                    }
                }
                else if (obj->isA("::sight::data::Float"))
                {
                    data::Float::sptr floatObj = data::Float::dynamicCast(obj);

                    bool isOkClicked = false;
                    double value     = QInputDialog::getDouble(
                        this, "Edition", "Enter the Integer value:", floatObj->value(), std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max(), 3, &isOkClicked);
                    if (isOkClicked)
                    {
                        floatObj->value() = static_cast<float>(value);
                        _item->setText(int(ColumnObjectType::DESC), QString("%1").arg(value));
                    }
                }
                else if (obj->isA("::sight::data::Boolean"))
                {
                    data::Boolean::sptr boolObj        = data::Boolean::dynamicCast(obj);
                    QMessageBox::StandardButton button = QMessageBox::question(
                        this, "Edition", "Defines the Boolean value");
                    boolObj->value() = (button == QMessageBox::Yes);
                    _item->setText(int(ColumnObjectType::DESC), boolObj->value() ? "true" : "false" );
                }
                else if (obj->isA("::sight::data::Matrix4"))
                {
                    data::Matrix4::sptr trf = data::Matrix4::dynamicCast(obj);
                    std::stringstream str;
                    str << *trf;

                    bool isOkClicked = false;
                    QString value    = QInputDialog::getMultiLineText(
                        this, "Edition", "Enter the Matrix coefficient (separated by a space):",
                        QString::fromStdString(str.str()), &isOkClicked);

                    QStringList coeffList = value.trimmed().split(QRegularExpression("\\s+"));
                    if (isOkClicked && coeffList.size() == 16)
                    {
                        data::Matrix4::TMCoefArray coeffs;

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
                        _item->setText(int(ColumnObjectType::DESC), value.trimmed() );
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

} // namespace activity
} // namespace sight::module::ui::qt
