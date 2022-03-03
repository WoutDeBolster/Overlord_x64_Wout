#pragma once

//#define LAB_W4
//#define LAB_W5
//#define LAB_W6
//#define LAB_W7

#ifdef LAB_W4
#define TODO_W4(msg) Logger::LogTodo(L"[LAB W4] " msg);
#define TODO_W4_() Logger::LogTodo(L"[LAB W4]");
#else
#define TODO_W4(msg)
#define TODO_W4_()
#endif

#ifdef LAB_W5
#define TODO_W5(msg) Logger::LogTodo(L"[LAB W5] " msg);
#define TODO_W5_() Logger::LogTodo(L"[LAB W5]");
#else
#define TODO_W5(msg)
#define TODO_W5_()
#endif

#ifdef LAB_W7
#define TODO_W7(msg) Logger::LogTodo(L"[LAB W7] " msg);
#define TODO_W7_() Logger::LogTodo(L"[LAB W7]");
#else
#define TODO_W7(msg)
#define TODO_W7_()
#endif