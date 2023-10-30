/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include <activity/extension/activity.hpp>

#include <data/activity.hpp>
#include <data/composite.hpp>

#include <QEvent>
#include <QPointer>
#include <QTabWidget>
#include <QTreeWidget>

namespace sight::module::ui::qt::activity
{

/**
 * @brief This class displays a tab widget allowing to select the required data to create an activity.
 */
class MODULE_UI_QT_CLASS_API data_view : public QTabWidget
{
Q_OBJECT;

public:

    /// Identifier of the role UID in the series tree item.
    MODULE_UI_QT_API static const int UID_ROLE;

    using object_icon_map_t = std::map<std::string, std::string>;

    /// Defines common header columns used in the tree widget of all created tabs.
    enum class column_commun_t : int
    {
        id = 0
    };

    /// Defines header columns used in the tree widget of all data.
    enum class column_object_t : int
    {
        desc = 1
    };

    /// Defines header columns used in the tree widget of all series.
    enum class column_series_t : int
    {
        name = 1,
        sex,
        birthdate,
        modality,
        modality_desc,
        study_desc,
        date,
        time,
        patient_age
    };

    /// Defines header columns used in the tree widget of image series.
    enum column_image_series_t : int
    {
        body_part_examined = 10,
        patient_position,
        contrast_agent,
        acquisition_time,
        contrast_bolus_start_time
    };

    /**
     * @brief Creates the tabbed widget.
     * @param _parent the parent of this widget.
     */
    MODULE_UI_QT_API data_view(QWidget* _parent = nullptr);

    /// Destroys the tabbed widget.
    MODULE_UI_QT_API ~data_view() override;

    /**
     * @brief Creates all tabs from an activity information.
     *
     * One tab will be added for each activity parameter, if the type of the data is a @ref data::series,
     * more informations will be displayed in the tree widget.
     *
     * @param _info the struct containing the activity configuration.
     */
    MODULE_UI_QT_API void fill_information(const sight::activity::extension::activity_info& _info);

    /**
     * @brief Creates all tabs from an activity.
     * @param _activity the activity.
     */
    MODULE_UI_QT_API void fill_information(const data::activity::sptr& _activity);

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
    MODULE_UI_QT_API bool check_and_compute_data(
        const data::activity::sptr& _activity,
        std::string& _error_msg
    );

    /**
     * @brief Checks if the current required data are selected And return this data.
     * @param _index index of the requirement to check (this index also correspond to a tab).
     * @param[out] _errorMsg it will contain the error information if the data are not correct.
     * @return return the data if it is correct, else return a nullptr and errorMsg contains detailled error.
     */
    MODULE_UI_QT_API data::object::sptr check_data(std::size_t _index, std::string& _error_msg);

    /**
     * @brief Sets the IO selector configuration used to import data.
     * @param _ioSelectorSrvConfig the service configuration used for the IO selector.
     */
    inline void set_io_selector_config(const std::string& _io_selector_srv_config)
    {
        m_io_selector_srv_config = _io_selector_srv_config;
    }

    /**
     * @brief Sets the IO selector configuration used to import data from a series set.
     * @param _ioSelectorSrvConfig the service configuration used for the IO selector.
     */
    inline void set_sdbio_selector_config(const std::string& _io_selector_srv_config)
    {
        m_sdb_io_selector_srv_config = _io_selector_srv_config;
    }

    /// Removes all the widget tabs.
    MODULE_UI_QT_API virtual void clear();

    /**
     * @brief Sets the map that defines the icon associated to an object.
     * @param _objectIcons the map associating an object type to an icon.
     */
    void set_object_icon_association(const object_icon_map_t& _object_icons)
    {
        m_object_icons = _object_icons;
    }

protected:

    /// Catch KeyEvent and DropEvent from tree widgets
    bool eventFilter(QObject* _obj, QEvent* _event) override;

private:

    using imported_object_t = std::vector<data::object::csptr>;
    using tree_container_t  = std::vector<QPointer<QTreeWidget> >;

    /// Imports an object required for the selected tab.
    void import_object();

    /// Imports a series_set and then extract the object required for the selected tab.
    void import_object_from_sdb();

    /// Creates a new object for the selected tab (just use data::factory::make(type)).
    void create_new_object();

    /**
     * @brief Calls @ref module::ui::editor::selector service to read the object. Return true if the object is
     * properly
     * read.
     * @param _classname the class name of the object to read.
     * @param _ioSelectorSrvConfig the IO selector configuration.
     * @return the object read by the selector.
     */
    static data::object::sptr read_object(const std::string& _classname, const std::string& _io_selector_srv_config);

    /// Removes the selected object in the current tree.
    void remove_selected_objects();

    /// Removes all objects in the current tree.
    void clear_tree();

    /**
     * @brief Allows to edit the current data.
     * @note Currently it is only available for simple types (@see data::string).
     */
    void on_tree_item_double_clicked(QTreeWidgetItem* _item, int _column);

    /**
     * @brief Adds the current item in the tree.
     * @param index index used to find the associated tree widget.
     * @param _obj object to add in the tree.
     */
    void add_object_item(std::size_t _index, const data::object::csptr& _obj);

    /// Sets the activity information
    sight::activity::extension::activity_info m_activity_info;

    /// Defines the IO selector config.
    std::string m_io_selector_srv_config;

    /// Dewfines the IO selector config to import data from a series_set.
    std::string m_sdb_io_selector_srv_config;

    /// Stores references on the imported object before to add them in the activity.
    imported_object_t m_imported_object;

    /// Stores the tree widget of each tab.
    tree_container_t m_tree_widgets;

    /// Associates an icon to an object type.
    object_icon_map_t m_object_icons;
};

} // namespace sight::module::ui::qt::activity
