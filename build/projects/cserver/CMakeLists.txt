j_project(cserver src/cserver)

j_aux_add_source(source_files)
include_directories(${j_project_root}/include )

add_definitions(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING -DBOOST_COROUTINE_NO_DEPRECATION_WARNING -DBOOST_LOG_DYN_LINK)
add_executable(cserver ${source_files})
j_set_target_default(cserver)
target_link_libraries(cserver boost_system boost_coroutine boost_log_setup boost_log boost_thread ssl crypto pthread)


