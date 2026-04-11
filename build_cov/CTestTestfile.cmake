# CMake generated Testfile for 
# Source directory: /home/petrus/IDrive/Documentos/projetos_dev/my_comp
# Build directory: /home/petrus/IDrive/Documentos/projetos_dev/my_comp/build_cov
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(unit_tests "/home/petrus/IDrive/Documentos/projetos_dev/my_comp/build_cov/unit_tests")
set_tests_properties(unit_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/petrus/IDrive/Documentos/projetos_dev/my_comp/CMakeLists.txt;124;add_test;/home/petrus/IDrive/Documentos/projetos_dev/my_comp/CMakeLists.txt;0;")
add_test(acceptance_tests "/home/petrus/IDrive/Documentos/projetos_dev/my_comp/build_cov/acceptance_tests")
set_tests_properties(acceptance_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/petrus/IDrive/Documentos/projetos_dev/my_comp/CMakeLists.txt;210;add_test;/home/petrus/IDrive/Documentos/projetos_dev/my_comp/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
