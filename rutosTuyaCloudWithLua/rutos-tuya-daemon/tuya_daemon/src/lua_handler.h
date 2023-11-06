#ifndef LUA_HANDLER_H
#define LUA_HANDLER_H

#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/luaconf.h>
#include <dirent.h>
#include <stdlib.h>
#include "utils.h"

#define MAX_LUA_SCRIPTS 15



/// @brief Initialises lua scripts from folder, checks them for validity
/// @param script_path Path to folder in which lua scripts will be stored
void init_lua_scripts(char *script_path);

/// @brief Deinitialises lua scripts and frees memory previuosly allocated for  running them
void deinit_lua_scripts();

/// @brief Call getdata method from script stored at index
/// @param index Index of script to call getdata from
/// @param *data_buffer Buffer in which the data from script will be stored, data is only valid if function returned 0
/// @param buffer_size size of data buffer
/// @return Returns 0 if successfull 1 otherwise
int call_script_getdata(int index, char *data_buffer, int buffer_size);

/// @brief Returns count of valid to run scripts
int get_valid_script_count();

/// @brief Calls lua script function specified in tuya actions name i.e luaScriptName_luaFunctionName action parameters will be parsed
// into a lua table and passed as an argument to the function, the function should return a string in json format
/// @param *script_name Script to open
/// @param *function_name Name of function to call
/// @param *message_json Tuya action message converted to json
/// @param *return_buffer Buffer in which the return of lua function will be stored
/// @param buffer_size Size of return_buffer
/// @return 0 on success 1 otherwise
int call_action_script(char *script_name, char *function_name, cJSON *param_json, char* return_buffer, int buffer_size);

#endif