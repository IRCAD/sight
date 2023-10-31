/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/qt/series/selector_model.hpp"

#include <core/runtime/path.hpp>
#include <core/tools/id.hpp>

#include <data/dicom/sop.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/series.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/math/special_functions/round.hpp>

#include <QFont>
#include <QPushButton>
#include <QStandardItem>
#include <QString>
#include <QTextCodec>
#include <QTreeView>

#include <regex>

namespace sight::ui::qt::series
{

//------------------------------------------------------------------------------

inline static bool is_date(const std::string& _date)
{
    if(_date.size() != 8)
    {
        return false;
    }

    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        if(std::isdigit(_date[i]) == 0)
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

inline static bool is_time(const std::string& _time)
{
    if(_time.size() < 6)
    {
        return false;
    }

    for(std::size_t i = 0 ; i < 6 ; ++i)
    {
        if(std::isdigit(_time[i]) == 0)
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

inline static std::string format_time(const std::string& _time)
{
    if(is_time(_time))
    {
        return _time.substr(0, 2) + ":" + _time.substr(2, 2) + ":" + _time.substr(4, 2);
    }

    return _time;
}

//------------------------------------------------------------------------------

inline static std::string format_date(const std::string& _date)
{
    if(is_date(_date))
    {
        return _date.substr(4, 2) + "/" + _date.substr(6, 2) + "/" + _date.substr(0, 4);
    }

    return _date;
}

enum class when
{
    study,
    series
};

struct column_display_information
{
    std::string header;
    std::function<QStandardItem* (data::series::csptr, when)> get_info;
};

//------------------------------------------------------------------------------

inline static QTextCodec* get_codec(data::series::csptr _series)
{
    QTextCodec* codec = QTextCodec::codecForName(_series->get_encoding().c_str());
    if(codec == nullptr)
    {
        return QTextCodec::codecForName("UTF-8");
    }

    return codec;
}

/* *INDENT-OFF* */
static const std::map<std::string, column_display_information> COLUMN_MAP {
    {"PatientName", {.header = "Name", .get_info =
        [](data::series::csptr _series, when _when)
        {
            if(_when == when::series)
            {
                return new QStandardItem;
            }

            QString res = get_codec(_series)->toUnicode(_series->get_patient_name().c_str());
            QString upper_patient_name = res.toUpper();
            if(upper_patient_name.isEmpty() || upper_patient_name.contains("ANONYMIZED") || upper_patient_name == "UNKNOWN"
                || upper_patient_name.contains("ANONYMOUS") || upper_patient_name == "NONE" || upper_patient_name == "NA")
            {
                // Automatically switch to Patient ID if the Patient Name is anonymized
                res = QString::fromStdString(_series->get_patient_id());
            }

            return new QStandardItem(res);
        }
     }
    },
    {"SeriesInstanceUID", {.header = "Name", .get_info =
        [](data::series::csptr _series, when _when)
        {
            if(_when == when::study)
            {
                return new QStandardItem;
            }

            if(std::string series_instance_uid = _series->get_series_instance_uid();
                !series_instance_uid.empty())
            {
                return new QStandardItem(QString::fromStdString(series_instance_uid));
            }

            return new QStandardItem(QString::fromStdString(_series->get_id()));
        }
     }
    },
    {"PatientName/SeriesInstanceUID", {.header = "Name", .get_info =
        [](data::series::csptr _series, when _when)
        {
            if(_when == when::study)
            {
                QString res = get_codec(_series)->toUnicode(_series->get_patient_name().c_str());
                QString upper_patient_name = res.toUpper();
                if(upper_patient_name.isEmpty() || upper_patient_name.contains("ANONYMIZED") || upper_patient_name == "UNKNOWN"
                    || upper_patient_name.contains("ANONYMOUS") || upper_patient_name == "NONE" || upper_patient_name == "NA")
                {
                    // Automatically switch to Patient ID if the Patient Name is anonymized
                    res = QString::fromStdString(_series->get_patient_id());
                }

                return new QStandardItem(res);
            }

            if(std::string series_instance_uid = _series->get_series_instance_uid(); !series_instance_uid.empty())
            {
                return new QStandardItem(QString::fromStdString(series_instance_uid));
            }

            return new QStandardItem(QString::fromStdString(_series->get_id()));
        }
     }
    },
    {"PatientSex", {.header = "Sex", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(_when == when::study ? _series->get_patient_sex() : ""));
        }
     }
    },
    {"PatientBirthDate", {.header = "Birthdate", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(
                QString::fromStdString(_when == when::study ? format_date(_series->get_patient_birth_date()) : ""));
        }
     }
    },
    {"Icon", {.header = "Icon", .get_info =
        [](data::series::csptr /*series*/, when _when)
        {
            if(_when == when::study)
            {
                return new QStandardItem;
            }

            auto* item = new QStandardItem;
            item->setData(true, selector_model::role::icon);
            return item;
        }
     }
    },
    {"PatientBirthDate/Icon", {.header = "Birthdate", .get_info =
        [](data::series::csptr _series, when _when)
        {
            if(_when == when::study)
            {
                return new QStandardItem(QString::fromStdString(format_date(_series->get_patient_birth_date())));
            }

            auto* item = new QStandardItem;
            item->setData(true, selector_model::role::icon);
            return item;
        }
     }
    },
    {"Modality", {.header = "Modality", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(_when == when::series ? _series->get_modality() : ""));
        }
     }
    },
    {"StudyDescription", {.header = "Description", .get_info =
        [](data::series::csptr _series, when _when)
        {
            if(_when == when::study)
            {
                return new QStandardItem(get_codec(_series)->toUnicode(_series->get_study_description().c_str()));
            }

            return new QStandardItem();
        }
     }
    },
    {"SeriesDescription", {.header = "Description", .get_info =
        [](data::series::csptr _series, when _when)
        {
            if(_when == when::study)
            {
                return new QStandardItem;
            }

            std::string infos(_series->get_sop_class_name());
            const auto& rows    = _series->get_rows();
            const auto& columns = _series->get_columns();
            const auto& frames  = _series->num_frames();
            if(rows && columns)
            {
                infos += " ( " + std::to_string(*rows)
                        + "x" + std::to_string(*columns)
                        + "x" + std::to_string(frames) + " )";
            }

            QString full_description = QString::fromStdString(infos);
            const auto& description = get_codec(_series)->toUnicode(_series->get_series_description().c_str());

            if(!description.isEmpty())
            {
                full_description += ": " + description;
            }

            return new QStandardItem(full_description);
        }
     }
    },
    {"StudyDescription/SeriesDescription", {.header = "Description", .get_info =
        [](data::series::csptr _series, when _when)
        {
            if(_when == when::study)
            {
                return new QStandardItem(get_codec(_series)->toUnicode(_series->get_description().c_str()));
            }

            std::string infos(_series->get_sop_class_name());
            const auto& rows = _series->get_rows();
            const auto& columns = _series->get_columns();
            const auto& frames = _series->num_frames();
            if(rows && columns)
            {
                infos += " ( " + std::to_string(*rows)
                        + "x" + std::to_string(*columns)
                        + "x" + std::to_string(frames) + " )";
            }

            QString full_description = QString::fromStdString(infos);
            const auto& description = get_codec(_series)->toUnicode(_series->get_series_description().c_str());

            if(!description.isEmpty())
            {
                full_description += ": " + description;
            }

            return new QStandardItem(full_description);
        }
     }
    },
    {"StudyDate", {.header = "Date", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(
                QString::fromStdString(_when == when::study ? format_date(_series->get_study_date()) : ""));
        }
     }
    },
    {"SeriesDate", {.header = "Date", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(
                QString::fromStdString(_when == when::series ? format_date(_series->get_series_date()) : ""));
        }
     }
    },
    {"StudyDate/SeriesDate", {.header = "Date", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(
                format_date(_when == when::study ? _series->get_study_date() : _series->get_series_date())));
        }
     }
    },
    {"StudyTime", {.header = "Time", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(
                QString::fromStdString(_when == when::study ? format_time(_series->get_study_time()) : ""));
        }
     }
    },
    {"SeriesTime", {.header = "Time", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(
                QString::fromStdString(_when == when::series ? format_time(_series->get_series_time()) : ""));
        }
     }
    },
    {"StudyTime/SeriesTime", {.header = "Time", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(
                format_time(_when == when::study ? _series->get_study_time() : _series->get_series_time())));
        }
     }
    },
    {"PatientAge", {.header = "Patient age", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(_when == when::study ? _series->get_patient_age() : ""));
        }
     }
    },
    {"BodyPartExamined", {.header = "Body part examined", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(
                _when == when::series && _series->get_dicom_type() == data::series::dicom_t::image
                    ? _series->get_body_part_examined()
                    : ""));
        }
     }
    },
    {"PatientPositionString", {.header = "Patient position", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(
                _when == when::series && _series->get_dicom_type() == data::series::dicom_t::image
                    ? _series->get_patient_position_string()
                    : ""));
        }
     }
    },
    {"ContrastBolusAgent", {.header = "Contrast agent", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(
                _when == when::series && _series->get_dicom_type() == data::series::dicom_t::image
                    ? _series->get_contrast_bolus_agent()
                    : ""));
        }
     }
    },
    {"AcquisitionTime", {.header = "Acquisition time", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(
                _when == when::series && _series->get_dicom_type() == data::series::dicom_t::image
                    ? format_time(_series->get_acquisition_time())
                    : ""));
        }
     }
    },
    {"ContrastBolusStartTime", {.header = "Contrast/bolus time", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(
                _when == when::series && _series->get_dicom_type() == data::series::dicom_t::image
                    ? format_time(_series->get_contrast_bolus_start_time())
                    : ""));
        }
     }
    },
    {"PatientID", {.header = "Patient ID", .get_info =
        [](data::series::csptr _series, when _when)
        {
            return new QStandardItem(QString::fromStdString(_when == when::study ? _series->get_patient_id() : ""));
        }
     }
    }
};
/* *INDENT-ON* */

//-----------------------------------------------------------------------------

selector_model::selector_model(const std::string& _display_columns, QWidget* _parent, bool _allow_remove) :
    QStandardItemModel(_parent),
    m_remove_allowed(_allow_remove)
{
    boost::split(m_display_columns, _display_columns, boost::is_any_of(","));
    this->init();
}

//-----------------------------------------------------------------------------

void selector_model::init()
{
    m_study_row_count = 0;
    m_items.clear();

    QStringList headers;
    for(const std::string& display_column : m_display_columns)
    {
        SIGHT_ASSERT("Display column '" << display_column << "' unknown.", COLUMN_MAP.contains(display_column));
        headers << QString::fromStdString(COLUMN_MAP.at(display_column).header);
    }

    if(m_remove_allowed)
    {
        headers << "Remove";
    }

    this->setHorizontalHeaderLabels(headers);
}

//-----------------------------------------------------------------------------

selector_model::item_t selector_model::get_item_type(const QModelIndex& _index)
{
    QModelIndex idx     = this->createIndex(_index.row(), 0, _index.internalPointer());
    QStandardItem* item = this->itemFromIndex(idx);
    return (selector_model::item_t) item->data(role::item_type).toInt();
}

//-----------------------------------------------------------------------------

void selector_model::clear()
{
    this->QStandardItemModel::clear();
    this->init();
}

//-----------------------------------------------------------------------------

data::image::spacing_t round_spacing(const data::image::spacing_t& _spacing)
{
    data::image::spacing_t round_spacing;
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        data::image::spacing_t::value_type round_val = boost::math::round(_spacing[i] * 100.) / 100.;
        round_spacing[i] = round_val;
    }

    return round_spacing;
}

//------------------------------------------------------------------------------

void selector_model::add_series(data::series::sptr _series)
{
    const auto study_instance_uid  = _series->get_study_instance_uid();
    auto itr                       = m_items.find(study_instance_uid);
    QStandardItem* study_root_item = nullptr;

    if(itr != m_items.end())
    {
        study_root_item = itr->second;
    }
    else
    {
        std::size_t i = 0;
        for(const std::string& display_column : m_display_columns)
        {
            setItem(
                m_study_row_count,
                static_cast<int>(i),
                COLUMN_MAP.at(display_column).get_info(_series, when::study)
            );
            i++;
        }

        item(m_study_row_count)->setData(item_t::study, role::item_type);
        item(m_study_row_count)->setData(QString::fromStdString(study_instance_uid), role::uid);

        // Add a remove button to each studies.
        if(m_remove_allowed && !m_remove_study_icon.empty())
        {
            this->setItem(m_study_row_count, static_cast<int>(i), new QStandardItem());

            auto* const selector = static_cast<QTreeView*>(this->parent());
            SIGHT_ASSERT("The QTreeView parent must be given to the constructor", selector);

            auto* const remove_button = new QPushButton(QIcon(m_remove_study_icon.string().c_str()), "");
            selector->setIndexWidget(this->index(m_study_row_count, static_cast<int>(i)), remove_button);

            // When the remove button is clicked, emit a signal with the study UID.
            QObject::connect(
                remove_button,
                &QPushButton::clicked,
                this,
                [study_instance_uid, this]()
                {
                    Q_EMIT remove_study_instance_uid(study_instance_uid);
                });
        }

        study_root_item = item(m_study_row_count);
        m_study_row_count++;
        m_items[study_instance_uid] = study_root_item;
    }

    const std::string& series_id = _series->get_id();

    const int nb_row = study_root_item->rowCount();

    QStandardItem* series_icon = nullptr;
    std::size_t i              = 0;
    for(const std::string& display_column : m_display_columns)
    {
        QStandardItem* item = COLUMN_MAP.at(display_column).get_info(_series, when::series);
        study_root_item->setChild(nb_row, static_cast<int>(i), item);
        if(item->data(role::icon).toBool())
        {
            series_icon = item;
        }

        i++;
    }

    study_root_item->child(nb_row)->setData(item_t::series, role::item_type);
    study_root_item->child(nb_row)->setData(QString::fromStdString(series_id), role::uid);

    // Add a remove button to each series.
    if(m_remove_allowed && !m_remove_series_icon.empty())
    {
        auto* const remove_item = new QStandardItem();
        study_root_item->setChild(nb_row, static_cast<int>(i), remove_item);

        auto* const selector = static_cast<QTreeView*>(this->parent());
        SIGHT_ASSERT("The QTreeView parent must be given to the constructor", selector);

        auto* const remove_button = new QPushButton(QIcon(m_remove_series_icon.string().c_str()), "");
        selector->setIndexWidget(this->indexFromItem(remove_item), remove_button);

        // When the remove button is clicked, emit a signal with the study UID.
        QObject::connect(
            remove_button,
            &QPushButton::clicked,
            this,
            [series_id, this]()
            {
                Q_EMIT remove_series_id(series_id);
            });
    }

    if(m_insert)
    {
        const int nb_columns = study_root_item->columnCount();
        for(int j = 0 ; j < nb_columns ; ++j)
        {
            QStandardItem* item = study_root_item->child(nb_row, j);
            if(item == nullptr)
            {
                study_root_item->setChild(nb_row, j, new QStandardItem());
                item = study_root_item->child(nb_row, j);
            }

            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        }
    }

    if(series_icon != nullptr)
    {
        this->add_series_icon(_series, series_icon);
    }
}

//-----------------------------------------------------------------------------

void selector_model::add_series_icon(data::series::sptr _series, QStandardItem* _item)
{
    auto iter = m_series_icons.find(_series->get_classname());
    if(iter != m_series_icons.end())
    {
        _item->setIcon(QIcon(QString::fromStdString(iter->second)));
    }
    else
    {
        std::filesystem::path icon_path;

        if(const auto& type = _series->get_dicom_type(); type == data::series::dicom_t::image)
        {
            icon_path = core::runtime::get_module_resource_file_path("sight::module::ui::icons", "ImageSeries.svg");
        }
        else if(type == data::series::dicom_t::model)
        {
            icon_path = core::runtime::get_module_resource_file_path("sight::module::ui::icons", "ModelSeries.svg");
        }

        if(!icon_path.empty())
        {
            _item->setIcon(QIcon(QString::fromStdString(icon_path.string())));
        }
    }
}

//-----------------------------------------------------------------------------

void selector_model::remove_series(data::series::sptr _series)
{
    QStandardItem* series_item = this->find_series_item(_series);

    if(series_item != nullptr)
    {
        this->remove_series_item(series_item);
    }
}

//-----------------------------------------------------------------------------

QModelIndex selector_model::get_index(const QModelIndex& _index, int _column)
{
    QModelIndex idx = this->createIndex(_index.row(), _column, _index.internalPointer());
    return idx;
}

//-----------------------------------------------------------------------------

void selector_model::removeRows(const QModelIndexList _indexes)
{
    QList<QStandardItem*> series_items;
    QList<QStandardItem*> study_items;

    for(QModelIndex index : _indexes)
    {
        SIGHT_ASSERT("Index must be in the first column.", index.column() == 0);
        QStandardItem* item = this->itemFromIndex(index);
        if(item->data(role::item_type) == item_t::study)
        {
            study_items.append(item);
        }
        else if(item->data(role::item_type) == item_t::series)
        {
            series_items.append(item);
        }
    }

    // Remove series items from selector
    for(QStandardItem* item : series_items)
    {
        QStandardItem* study_item = item->parent();

        // Remove series item if it is not included in a study which will be remove.
        if(std::find(study_items.begin(), study_items.end(), study_item) == study_items.end())
        {
            this->remove_series_item(item);
        }
    }

    // Remove study items from selector
    for(QStandardItem* item : study_items)
    {
        this->remove_study_item(item);
    }
}

//-----------------------------------------------------------------------------

bool selector_model::remove_study_item(QStandardItem* _item)
{
    bool is_removed = false;
    SIGHT_ASSERT("Index must represent a study.", _item->data(role::item_type) == item_t::study);
    QString uid                      = _item->data(role::uid).toString();
    data::dicom_value_t instance_uid = uid.toStdString();

    is_removed = this->QStandardItemModel::removeRow(_item->row());
    SIGHT_ASSERT("Remove can not be done!", is_removed);
    m_items.erase(instance_uid);
    --m_study_row_count;

    return is_removed;
}

//-----------------------------------------------------------------------------

bool selector_model::remove_series_item(QStandardItem* _item)
{
    bool is_removed = false;

    SIGHT_ASSERT("Index must represent series", _item->data(role::item_type) == item_t::series);
    QStandardItem* parent = _item->parent();
    is_removed = this->QStandardItemModel::removeRow(_item->row(), this->indexFromItem(parent));
    SIGHT_ASSERT("Remove can not be done!", is_removed);
    if(parent != nullptr && parent->rowCount() == 0)
    {
        this->remove_study_item(parent);
    }

    return is_removed;
}

//-----------------------------------------------------------------------------

QStandardItem* selector_model::find_series_item(data::series::sptr _series)
{
    QStandardItem* series_item = nullptr;
    QStandardItem* study_item  = this->find_study_item(_series);

    if(study_item != nullptr)
    {
        int nb_row = study_item->rowCount();
        for(int row = 0 ; row < nb_row ; ++row)
        {
            QStandardItem* child  = study_item->child(row, 0);
            std::string series_id = child->data(role::uid).toString().toStdString();
            if(series_id == _series->get_id())
            {
                series_item = child;
                break;
            }
        }
    }

    return series_item;
}

//-----------------------------------------------------------------------------

QStandardItem* selector_model::find_study_item(data::series::sptr _series)
{
    data::dicom_value_t study_instance_uid = _series->get_study_instance_uid();

    QStandardItem* study_item = m_items[study_instance_uid];
    return study_item;
}

//-----------------------------------------------------------------------------

void selector_model::set_series_icons(const series_icon_t& _series_icons)
{
    m_series_icons = _series_icons;
}

} // namespace sight::ui::qt::series
