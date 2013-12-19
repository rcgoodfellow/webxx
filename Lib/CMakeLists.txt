
set(LIBSRCS web++.cc)
set(LIBHDRS web++.hh)

add_library(web++ SHARED ${LIBSRCS} ${LIBHDRS})
target_link_libraries(web++ boost_system boost_filesystem boost_log boost_log_setup)
