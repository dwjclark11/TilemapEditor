add_subdirectory(imgui)
add_subdirectory(nfd)

set_target_properties(imgui nfd PROPERTIES FOLDER "dependencies")
