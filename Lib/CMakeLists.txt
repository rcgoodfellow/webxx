include_directories(${CMAKE_SOURCE_DIR}/Lib)

set(LIBSRCS 
  utility.cc
  http/server.cc
  ws/base64.cpp
  ws/receiver.cc
  ws/session.cc
)
set(LIBHDRS 
  http/server.hh
  ws/base64.hpp
  ws/receiver.hh
  ws/session.cc
)

add_library(web++ SHARED ${LIBSRCS} ${LIBHDRS})
target_link_libraries(web++ boost_system boost_filesystem boost_log boost_log_setup)

install(TARGETS web++ DESTINATION lib)
install(FILES utility.hh log.hh DESTINATION include/web++)
install(FILES http/server.hh DESTINATION include/web++/http)
install(FILES ws/receiver.hh ws/session.hh ws/base64.hpp DESTINATION include/web++/ws)
