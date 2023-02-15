cmake_minimum_required (VERSION 2.6)


if (PLUGIN_IPERF)
    file (GLOB src_file "*.c*")

    add_library (plugin_iperf MODULE ${src_file})
    target_link_libraries (plugin_iperf gfal2 gfal2_transfer uuid)


    set_target_properties(plugin_iperf   PROPERTIES
                                LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/src
								CLEAN_DIRECT_OUTPUT 1
								OUTPUT_NAME "gfal_plugin_iperf"
								LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

    install(TARGETS		plugin_iperf
	        LIBRARY		DESTINATION ${PLUGIN_INSTALL_DIR})

    install(FILES		"README_PLUGIN_IPERF"
                DESTINATION ${DOC_INSTALL_DIR})

    # install iperf configuration files
    LIST(APPEND iperf_conf_file "${CMAKE_SOURCE_DIR}/dist/etc/gfal2.d/iperf_plugin.conf")
    install(FILES ${iperf_conf_file}
                        DESTINATION ${SYSCONF_INSTALL_DIR}/gfal2.d/)

endif (PLUGIN_IPERF)

