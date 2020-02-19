/************************************************************************
 *
 * Copyright (C) 2016-2020 IRCAD France
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

#include "uiMedDataQt/config.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwData/Composite.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <QEvent>
#include <QPointer>
#include <QTabWidget>
#include <QTreeWidget>

namespace uiMedDataQt
{
namespace widget
{

/**
 * @brief This class displays a tab widget allowing to select the required data to create an activity.
 */
class UIMEDDATAQT_CLASS_API ActivityDataView : public QTabWidget
{

Q_OBJECT;

public:

    /// Identifier of the role UID in the series tree item.
    UIMEDDATAQT_API static const int s_UID_ROLE;

    typedef std::map< std::string, std::string > ObjectIconMapType;

    /// Defines comun header columns used in the tree widget of all created tabs.
    enum class ColumnCommunType : int
    {
        ID = 0
    };

    /// Defines header columns used in the tree widget of all data.
    enum class ColumnObjectType : int
    {
        DESC = 1,
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

    /**
     * @brief Creates the tabbed widget.
     * @param _parent the parent of this widget.
     */
    UIMEDDATAQT_API ActivityDataView(QWidget* _parent = 0);

    /// Destroys the tabbed widget.
    UIMEDDATAQT_API virtual ~ActivityDataView();

    /**
     * @brief Creates all tabs from an activity information.
     *
     * One tab will be added for each activity parameter, if the type of the data is a @ref ::fwMedData::Series,
     * more informations will be displayed in the tree widget.
     *
     * @param _info the struct containing the activity configuration.
     */
    UIMEDDATAQT_API void fillInformation(const ::fwActivities::registry::ActivityInfo& _info);

    /**
     * @brief Creates all tabs from an activity series.
     * @param _actSeries the series containing activities.
     */
    UIMEDDATAQT_API void fillInformation(const ::fwMedData::ActivitySeries::sptr& _actSeries);

    /**
     * @brief Checks if all the required data are selected and fill activity data composite.
     *
     * It checks if all required data are present in the widget, and create the activity data composite.
     * Then it checks if the activity parameters are accessible from the composite of data (it checks if the object are
     * accessible from the sesh@ paths.
     *
     * @return True if data are correct, else false and errorMsg contains detailled error.
     * @param _actSeries activitySeries to fill with the selected data.
     * @param[out] _errorMsg it will contain the error information if the data are not correct.
     */
    UIMEDDATAQT_API bool checkAndComputeData(const ::fwMedData::ActivitySeries::sptr& _actSeries,
                                             std::string& _errorMsg);

    /**
     * @brief Checks if the current required data are selected And return this data.
     * @param _index index of the requirement to check (this index also correspond to a tab).
     * @param[out] _errorMsg it will contain the error information if the data are not correct.
     * @return return the data if it is correct, else return a nullptr and errorMsg contains detailled error.
     */
    UIMEDDATAQT_API ::fwData::Object::sptr checkData(size_t _index, std::string& _errorMsg);

    /**
     * @brief Sets the IO selector configuration used to import data.
     * @param _ioSelectorSrvConfig the service confiuration used for the IO selector.
     */
    inline void setIOSelectorConfig(const std::string& _ioSelectorSrvConfig)
    {
        m_ioSelectorSrvConfig = _ioSelectorSrvConfig;
    }

    /**
     * @brief Sets the IO selector configuration used to import data from a series DB.
     * @param _ioSelectorSrvConfig the service confiuration used for the IO selector.
     */
    inline void setSDBIOSelectorConfig(const std::string& _ioSelectorSrvConfig)
    {
        m_sdbIoSelectorSrvConfig = _ioSelectorSrvConfig;
    }

    /// Removes all the widget tabs.
    UIMEDDATAQT_API virtual void clear();

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

    typedef std::vector< ::fwData::Object::csptr > ImportedObjectType;
    typedef std::vector<QPointer<QTreeWidget> > TreeContainerType;

    /// Imports an object required for the selected tab.
    void importObject();

    /// Imports a SeriesDB and then extract the object required for the selected tab.
    void importObjectFromSDB();

    /// Creates a new object for the selected tab (just use ::fwData::factory::New(type)).
    void createNewObject();

    /**
     * @brief Calls @ref ::uiIO::editor::SIOSelector service to read the object. Return true if the object is properly
     *read.
     * @param _classname the type name of the object to read.
     * @param _ioSelectorSrvConfig the IO selector configuration.
     * @return the object read by the selector.
     */
    fwData::Object::sptr readObject(const std::string& _classname, const std::string& _ioSelectorSrvConfig);

    /// Removes the selected object in the current tree.
    void removeSelectedObjects();

    /// Removes all objects in the current tree.
    void clearTree();

    /**
     * @brief Allows to edit the current data.
     * @note Currently it is only available for simple types (@see ::fwData::String).
     */
    void onTreeItemDoubleClicked(QTreeWidgetItem* _item, int _column);

    /**
     * @brief Adds the current item in the tree.
     * @param index index used to find the associated tree widget.
     * @param _obj object to add in the tree.
     */
    void addObjectItem(size_t index, const ::fwData::Object::csptr& _obj);

    /// Sets the activity information
    ::fwActivities::registry::ActivityInfo m_activityInfo;

    /// Defines the IO selector config.
    std::string m_ioSelectorSrvConfig;

    /// Dewfines the IO selector config to import data from a SeriesDB.
    std::string m_sdbIoSelectorSrvConfig;

    /// Stores references on the imported object before to add them in the activity series.
    ImportedObjectType m_importedObject;

    /// Stores the tree widget of each tab.
    TreeContainerType m_treeWidgets;

    /// Associates an icon to an object type.
    ObjectIconMapType m_objectIcons;
};

} // namespace widget
} // namespace uiMedDataQt
