set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Designer REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5UiTools REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_executable(linguist
   batchtranslationdialog.cpp
   batchtranslation.ui
   configdialog.cpp
   configdialog.ui
   configuration.cpp
   errorsview.cpp
   finddialog.cpp
   finddialog.ui
   formpreviewview.cpp
   globals.cpp
   linguist.qrc
   main.cpp
   mainwindow.cpp
   mainwindow.ui
   messageeditor.cpp
   messageeditorwidgets.cpp
   messagehighlighter.cpp
   messagemodel.cpp
   phrase.cpp
   phrasebookbox.cpp
   phrasebookbox.ui
   phrasemodel.cpp
   phraseview.cpp
   printout.cpp
   recentfiles.cpp
   sourcecodeview.cpp
   statistics.cpp
   statistics.ui
   translatedialog.cpp
   translatedialog.ui
   translationsettings.ui
   translationsettingsdialog.cpp
   # Formats
   ../shared/po.cpp
   ../shared/qm.cpp
   ../shared/qph.cpp
   ../shared/ts.cpp
   ../shared/xliff.cpp
   # Other Shared Code
   ../shared/numerus.cpp
   ../shared/simtexth.cpp
   ../shared/translator.cpp
   ../shared/translatormessage.cpp
   ../shared/xmlparser.cpp
)

set_target_properties(linguist PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST Info_mac.plist
)

target_compile_definitions(linguist PRIVATE
    QFORMINTERNAL_NAMESPACE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
)

target_link_libraries(linguist
    Qt5::Designer
    Qt5::PrintSupport
    Qt5::UiTools
    Qt5::Widgets
)

target_include_directories(linguist PRIVATE
    ../shared
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5Designer_PRIVATE_INCLUDE_DIRS}
    ${Qt5UiTools_PRIVATE_INCLUDE_DIRS}
)
