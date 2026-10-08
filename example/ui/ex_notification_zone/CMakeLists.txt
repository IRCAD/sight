sight_add_target(ex_notification_zone TYPE APP REQUIRE_ADMIN OFF)

add_dependencies(${SIGHT_TARGET} module_app module_service module_ui module_ui_qt module_ui_icons)

target_link_libraries(${SIGHT_TARGET} PRIVATE service)

module_param(
    module_ui_qt PARAM_LIST resource stylesheet touch_friendly PARAM_VALUES sight::module::ui::qt/noctura.rcc
                                                                            sight::module::ui::qt/noctura.qss true
)

# Launch configuration: point module_app to the example config id
module_param(module_app PARAM_LIST config PARAM_VALUES ex_notification_zone_app_cfg)
