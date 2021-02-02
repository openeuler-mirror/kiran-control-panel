#翻译文件
#find_package(Qt5 COMPONENTS LinguistTools)
#file(GLOB TS_FILES "*.ts")
#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} "*.ts")
file(GLOB QM_FILES "./*.qm")
install(FILES ${QM_FILES} DESTINATION ${TRANSLATE_PATH})
