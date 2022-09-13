/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#pragma once

#include "modules/ui/qt/config.hpp"

#include <activity/extension/Activity.hpp>

#include <data/Activity.hpp>
#include <data/Composite.hpp>

#include <QEvent>
#include <QPointer>
#include <QTabWidget>
#include <QTreeWidget>

namespace sight::module::ui::qt::activity
{

/**
 * @brief This class displays a tab widget allowing to select the required data to create an activity.
 */
class MODULE_UI_QT_CLASS_API DataView : public QTabWidget
{
Q_OBJECT;

public:

    /// Identifier of the role UID in the series tree item.
    MODULE_UI_QT_API static const int s_UID_ROLE;

    typedef std::map<std::string, std::string> ObjectIconMapType;

    /// Defines common header columns used in the tree widget of all created tabs.
    enum class ColumnCommunType : int
    {
        ID = 0
    };

    /// Defines header columns used in the tree widget of all data.
    enum class ColumnObjectType : int
    {
        DESC = 1
    };

    /// Defines header columns used in the tree widget of all series.
    enum class ColumnSeriesType : int
    {
        NAME = 1,
        SEX,
        BIRTHDATE,
        MODALITY,
        MODALITY_DESC,
        STUDY_DESC,
        DATE,
        TIME,
        PATIENT_AGE
    };

    /// Defines header columns used in the tree widget of image series.
    enum ColumnImageSeriesType : int
    {
        BODY_PART_EXAMINED = 10,
        PATIENT_POSITION,
        CONTRAST_AGENT,
        ACQUISITION_TIME,
        CONTRAST_BOLUS_START_TIME
    };

    /**
     * @brief Creates the tabbed widget.
     * @param _parent the parent of this widget.
     */
    MODULE_UI_QT_API DataView(QWidget* _parent = nullptr);

    /// Destroys the tabbed widget.
    MODULE_UI_QT_API ~DataView() override;

    /**
     * @brief Creates all tabs from an activity information.
     *
     * One tab will be added for each activity parameter, if the type of the data is a @ref data::Series,
     * more informations will be displayed in the tree widget.
     *
     * @param _info the struct containing the activity configuration.
     */
    MODULE_UI_QT_API void fillInformation(const sight::activity::extension::ActivityInfo& _info);

    /**
     * @brief Creates all tabs from an activity.
     * @param _activity the activity.
     */
    MODULE_UI_QT_API void fillInformation(const data::Activity::sptr& _activity);

    /**
     * @brief Checks if all the required data are selected and fill activity data composite.
     *
     * It checks if all required data are present in the widget, and create the activity data composite.
     * Then it checks if the activity parameters are accessible from the composite of data (it checks if the object are
     * accessible from the object paths.
     *
     * @return True if data are correct, else false and errorMsg contains detailled error.
     * @param _activity activity to fill with the selected data.
     * @param[out] _errorMsg it will contain the error information if the data are not correct.
     */
    MODULE_UI_QT_API bool checkAndComputeData(
        const data::Activity::sptr& _activity,
        std::string& _errorMsg
    );

    /**
     * @brief Checks if the current required data are selected And return this data.
     * @param _index index of the requirement to check (this index also correspond to a tab).
     * @param[out] _errorMsg it will contain the error information if the data are not correct.
     * @return return the data if it is correct, else return a nullptr and errorMsg contains detailled error.
     */
    MODULE_UI_QT_API data::Object::sptr checkData(std::size_t _index, std::string& _errorMsg);

    /**
     * @brief Sets the IO selector configuration used to import data.
     * @param _ioSelectorSrvConfig the service configuration used for the IO selector.
     */
    inline void setIOSelectorConfig(const std::string& _ioSelectorSrvConfig)
    {
        m_ioSelectorSrvConfig = _ioSelectorSrvConfig;
    }

    /**
     * @brief Sets the IO selector configuration used to import data from a series set.
     * @param _ioSelectorSrvConfig the service configuration used for the IO selector.
     */
    inline void setSDBIOSelectorConfig(const std::string& _ioSelectorSrvConfig)
    {
        m_sdbIoSelectorSrvConfig = _ioSelectorSrvConfig;
    }

    /// Removes all the widget tabs.
    MODULE_UI_QT_API virtual void clear();

    /**
     * @brief Sets the map that defines the icon associated to an object.
     * @param _objectIcons the map associating an object type to an icon.
     */
    void setObjectIconAssociation(const ObjectIconMapType& _objectIcons)
    {
        m_objectIcons = _objectIcons;
    }

protected:

    /// Catch KeyEvent and DropEvent from tree widgets
    bool eventFilter(QObject* _obj, QEvent* _event) override;

private:

    typedef std::vector<data::Object::csptr> ImportedObjectType;
    typedef std::vector<QPointer<QTreeWidget> > TreeContainerType;

    /// Imports an object required for the selected tab.
    void importObject();

    /// Imports a SeriesSet and then extract the object required for the selected tab.
    void importObjectFromSDB();

    /// Creates a new object for the selected tab (just use data::factory::New(type)).
    void createNewObject();

    /**
     * @brief Calls @ref module::ui::base::editor::SSelector service to read the object. Return true if the object is
     * properly
     * read.
     * @param _classname the class name of the object to read.
     * @param _ioSelectorSrvConfig the IO selector configuration.
     * @return the object read by the selector.
     */
    static data::Object::sptr readObject(const std::string& _classname, const std::string& _ioSelectorSrvConfig);

    /// Removes the selected object in the current tree.
    void removeSelectedObjects();

    /// Removes all objects in the current tree.
    void clearTree();

    /**
     * @brief Allows to edit the current data.
     * @note Currently it is only available for simple types (@see data::String).
     */
    void onTreeItemDoubleClicked(QTreeWidgetItem* _item, int _column);

    /**
     * @brief Adds the current item in the tree.
     * @param index index used to find the associated tree widget.
     * @param _obj object to add in the tree.
     */
    void addObjectItem(std::size_t index, const data::Object::csptr& _obj);

    /// Sets the activity information
    sight::activity::extension::ActivityInfo m_activityInfo;

    /// Defines the IO selector config.
    std::string m_ioSelectorSrvConfig;

    /// Dewfines the IO selector config to import data from a SeriesSet.
    std::string m_sdbIoSelectorSrvConfig;

    /// Stores references on the imported object before to add them in the activity.
    ImportedObjectType m_importedObject;

    /// Stores the tree widget of each tab.
    TreeContainerType m_treeWidgets;

    /// Associates an icon to an object type.
    ObjectIconMapType m_objectIcons;
};

} // namespace sight::module::ui::qt::activity
