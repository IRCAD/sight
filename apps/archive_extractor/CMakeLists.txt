sight_add_target(archive_extractor TYPE APP)

add_dependencies(
    archive_extractor
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_ui_icons
    module_io_zip
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ArchiveExtractor_AppCfg)

sight_generate_profile(archive_extractor)
