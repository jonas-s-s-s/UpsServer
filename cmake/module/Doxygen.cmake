function(Doxygen input output)
    find_package(Doxygen)
    if (NOT DOXYGEN_FOUND)
        add_custom_target(doxygen COMMAND false
                COMMENT "Doxygen not found")
        message(WARNING "Doxygen is not installed. Please download doxygen (and graphviz) in order to generate documentation.")
        return()
    endif ()

    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_HTML_OUTPUT ${PROJECT_BINARY_DIR}/${output})

    UseDoxygenAwesomeCss()

    doxygen_add_docs(doxygen ${PROJECT_SOURCE_DIR}/${input}
            COMMENT "Generate HTML documentation"
    )
endfunction()

macro(UseDoxygenAwesomeCss)
    include(FetchContent)
    FetchContent_Declare(doxygen-awesome-css
            GIT_REPOSITORY
            https://github.com/jothepro/doxygen-awesome-css.git
            GIT_TAG
            v2.2.1
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_DOT_TRANSPARENT YES)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET
            ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css)
endmacro()
