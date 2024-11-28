# Lägg till exekverbara filer
add_executable(server
    src/server_main.cpp
    # Lägg till andra källfiler här om det behövs
)

# Definiera Windows-version
add_definitions(-D_WIN32_WINNT=0x0A00)

# Länka FTXUI-biblioteken
target_link_libraries(server PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# Om du har egna inkluderingsfiler i 'includes'-katalogen
target_include_directories(server PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/includes"
)
