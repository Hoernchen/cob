
add_executable(a1 lex.cpp parser.cpp CodegenVisitor.cpp ASTGraphVisitor.cpp main.cpp)
target_link_libraries(a1 LLVMCore LLVMSupport)
