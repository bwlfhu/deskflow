# SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
# SPDX-License-Identifier: MIT

# Test the real method bodies without installing hooks, moving the user's
# pointer or sharing the user's clipboard. A production edit regenerates the
# fixture, so reverting the fix cannot leave a separate implementation green.
set(mouse_source "${CMAKE_SOURCE_DIR}/src/lib/platform/MSWindowsComputer.cpp")
set(hook_source "${CMAKE_SOURCE_DIR}/src/lib/platform/MSWindowsHook.h")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${mouse_source}" "${hook_source}")
file(READ "${mouse_source}" mouse_code)
string(REPLACE "\r\n" "\n" mouse_code "${mouse_code}")

function(extract_mouse_method signature output)
  string(FIND "${mouse_code}" "${signature}" method_start)
  if(method_start LESS 0)
    message(FATAL_ERROR "Mouse regression fixture cannot find ${signature}")
  endif()
  string(SUBSTRING "${mouse_code}" ${method_start} -1 method_code)
  string(FIND "${method_code}" "\n}" method_end)
  if(method_end LESS 0)
    message(FATAL_ERROR "Mouse regression fixture cannot delimit ${signature}")
  endif()
  math(EXPR method_length "${method_end} + 2")
  string(SUBSTRING "${method_code}" 0 ${method_length} method_code)
  set(${output} "${method_code}" PARENT_SCOPE)
endfunction()

extract_mouse_method("bool MSWindowsComputer::onMouseMove(" ON_MOUSE_MOVE)
extract_mouse_method("bool MSWindowsComputer::canLeave(" CAN_LEAVE)
extract_mouse_method("void MSWindowsComputer::warpCursor(" WARP_CURSOR)
extract_mouse_method("POINT MSWindowsComputer::warpCursorNoFlush(" WARP_NO_FLUSH)
string(FIND "${mouse_code}" "case DESKFLOW_MSG_PRE_WARP: {" pre_start)
string(FIND "${mouse_code}" "case DESKFLOW_MSG_POST_WARP:" post_start)
if(pre_start LESS 0 OR post_start LESS pre_start)
  message(FATAL_ERROR "Mouse regression fixture cannot delimit the warp-message handler")
endif()
math(EXPR pre_length "${post_start} - ${pre_start}")
string(SUBSTRING "${mouse_code}" ${pre_start} ${pre_length} PRE_WARP_CASE)

file(STRINGS "${hook_source}" message_lines REGEX "^#define DESKFLOW_MSG_(KEY|MOUSE_MOVE|POST_WARP|PRE_WARP|INPUT_FIRST|INPUT_LAST) ")
string(JOIN "\n" MESSAGE_DEFINITIONS ${message_lines})
configure_file("${CMAKE_CURRENT_LIST_DIR}/MSWindowsComputerEventTests.cpp.in" MSWindowsComputerEventTests.cpp @ONLY)
add_executable(MSWindowsComputerEventTests "${CMAKE_CURRENT_BINARY_DIR}/MSWindowsComputerEventTests.cpp")
target_include_directories(MSWindowsComputerEventTests PRIVATE "${CMAKE_SOURCE_DIR}/src/lib")
add_test(NAME MSWindowsComputerEventTests COMMAND MSWindowsComputerEventTests)
set_property(GLOBAL APPEND PROPERTY ${CMAKE_PROJECT_NAME}_tests MSWindowsComputerEventTests)
