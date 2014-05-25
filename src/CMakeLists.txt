
add_executable(a1 lex.cpp parser.cpp CodegenVisitor.cpp ASTGraphVisitor.cpp TCheckVisitor.cpp main.cpp)

if(NOT MSVC)
target_link_libraries(a1 pthread ffi tinfo dl m LLVMCore LLVMSupport tinfo)
else()
target_link_libraries(a1 LLVMCore LLVMSupport)
endif()