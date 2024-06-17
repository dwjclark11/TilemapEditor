add_library(imgui
  "src/imconfig.h"
  "src/imgui.cpp"
  "src/imgui.h"
  "src/imgui_demo.cpp"
  "src/imgui_draw.cpp"
  "src/imgui_internal.h"
  "src/imgui_stdlib.cpp"
  "src/imgui_stdlib.h"
  "src/imgui_widgets.cpp"
  "src/imstb_rectpack.h"
  "src/imstb_textedit.h"
  "src/imstb_truetype.h"

  "src/backends/imgui_impl_sdl.cpp"
  "src/backends/imgui_impl_sdl.h"
  "src/backends/imgui_sdl.cpp"
  "src/backends/imgui_sdl.h"
  )

target_include_directories(
  imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(imgui PUBLIC SDL2::SDL2)