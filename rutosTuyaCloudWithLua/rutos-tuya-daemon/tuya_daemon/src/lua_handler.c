#include "lua_handler.h"

static struct Lua_script *lua_scripts[MAX_LUA_SCRIPTS];
static int script_count = 0;
static char* lua_script_dir;

struct Lua_script {
	char filename[256];
	struct lua_State *L;
	int has_init;
	int has_deinit;
};

void init_lua_scripts(char *script_path)
{	
	lua_script_dir = script_path;
	script_count = 0;
	alloc_script_structs(lua_scripts);
	load_lua_scripts(lua_scripts, &script_count, script_path);

	for (int i = 0; i < script_count; i++) {
		if (lua_scripts[i]->L != NULL && lua_scripts[i]->has_init) {
			call_script_init(lua_scripts[i]);
		}
	}
}

void deinit_lua_scripts()
{	
	for (int i = 0; i < script_count; i++) {
		if (lua_scripts[i]->L != NULL && lua_scripts[i]->has_deinit) {
			call_script_deinit(lua_scripts[i]);
		}
	}
	free_script_structs(lua_scripts);
	script_count = 0;
}

/// @brief Loads lua scripts to array
/// @param *scripts Array in which script data will be stored
/// @param *count The count of valid scripts will be returned trough this variable
/// @param script_dir Directory from which the scripts will be taken
void load_lua_scripts(struct Lua_script *scripts[], int *count, char *script_dir)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(script_dir);
	if (d != NULL) {
		int i = 0;
		while ((dir = readdir(d)) != NULL && (i < MAX_LUA_SCRIPTS)) {
			if (is_lua_file(dir->d_name)) {
				strncpy(scripts[i]->filename, script_dir, sizeof(scripts[i]->filename));
				strncat(scripts[i]->filename, "/", sizeof(scripts[i]->filename));
				strncat(scripts[i]->filename, dir->d_name, sizeof(scripts[i]->filename));

				if (check_lua_script(scripts[i])) {
					log_finfo("Did not load script [%s]", scripts[i]->filename);
					continue;
				}
				i++;
			}
		}
		*count = i;
		closedir(d);
	}else{
		log_ferror("Error: Could not open: %s directory for lua scripts", script_dir);
	}
}


/// @brief Checks if lua script is valid, and initialises it's lua state
/// @param *script Script to check
/// @return Returns 0 if valid 1 otherwise
int check_lua_script(struct Lua_script *script)
{
	script->L = lua_open();
	if (script->L != NULL)
		luaL_openlibs(script->L);

	if (load_script_to_state(script)) {
		lua_close(script->L);
		script->L = NULL;
		return 1;
	}

	if (check_lua_functionality(script)) {
		lua_close(script->L);
		script->L = NULL;
		return 1;
	}

	return 0;
}

/// @brief Loads script to lua state
/// @param *script Script to load
/// @return 0 if successfull 1 otherwise
int load_script_to_state(struct Lua_script *script)
{
	if (luaL_loadfile(script->L, script->filename) || lua_pcall(script->L, 0, 0, 0)) {
		log_lua_error(script->L, "Cannot load file [%s]: %s \n", script->filename,
			      lua_tostring(script->L, -1));
		return 1;
	}
	return 0;
}


/// @brief Checks if lua script has the needed functionality, also whether it has init and deinit functions
/// @param *script Script to check
/// @return 0 if it has needed functionality 1 otherwise
int check_lua_functionality(struct Lua_script *script)
{
	lua_getglobal(script->L, "getdata");

	if (!lua_isfunction(script->L, -1)) {
		log_ferror("[%s]  does not have getdata function \n", script->filename);
		return 1;
	}

	if (lua_pcall(script->L, 0, 1, 0) != 0) {
		log_lua_error(script->L, "[%s]Error when running/calling the getdata function: %s \n",
			      script->filename, lua_tostring(script->L, -1));
		return 1;
	}

	if (!lua_isstring(script->L, -1)) {
		log_ferror("[%s] getdata does not return a string");
		return 1;
	}

	lua_pop(script->L, -1); // pop the returned string from getdata

	lua_getglobal(script->L, "init");

	if (!lua_isfunction(script->L, -1)) {
		log_finfo("[%s]  does not have init function \n", script->filename);
		script->has_init = 0;
	} else {
		script->has_init = 1;
	}

	lua_getglobal(script->L, "deinit");
	if (!lua_isfunction(script->L, -1)) {
		log_finfo("[%s]  does not have deinit function \n", script->filename);
		script->has_deinit = 0;
	} else {
		script->has_deinit = 1;
	}

	return 0;
}


/// @brief Logs lua error message
/// @param *L lua state in which error occurred
/// @param *fmt format to log message in
void log_lua_error(lua_State *L, const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	char buff[512];
	vsnprintf(buff, sizeof(buff), fmt, argp);
	log_error(buff);
	lua_pop(L, -1);
	va_end(argp);
}


/// @brief Allocates memory for Lua_script structs
/// @param *scripts[] Array of scripts
void alloc_script_structs(struct Lua_script *scripts[])
{
	for (int i = 0; i < MAX_LUA_SCRIPTS; i++) {
		scripts[i] = (struct Lua_script *)malloc(sizeof(struct Lua_script));
		scripts[i]->L = NULL;
	}

}

/// @brief Frees memory associated with Lua_script from array
/// @param *scripts[] Array of scripts
void free_script_structs(struct Lua_script *scripts[])
{
	for (int i = 0; i < MAX_LUA_SCRIPTS; i++) {
		if (scripts[i] != NULL) {
			if (scripts[i]->L != NULL) {
				lua_close(scripts[i]->L);
				scripts[i]->L = NULL;
			}
			free(scripts[i]);
			scripts[i] = NULL;
		}
	}
}

/// @brief Call init function of a script
/// @param *script Script from which to call the function
void call_script_init(struct Lua_script *script)
{
	if(!script->has_init){
		return;
	}

	lua_getglobal(script->L, "init");

	if (lua_isfunction(script->L, -1)) {
		if (lua_pcall(script->L, 0, 0, 0) != 0) {
			log_lua_error(script->L, "[%s]Error when running/calling init function: %s \n",
				      script->filename, lua_tostring(script->L, -1));
		}
	}
}


/// @brief Call deinit function of a script
/// @param *script Script from which to call the function
void call_script_deinit(struct Lua_script *script)
{

	if(!script->has_deinit){
		return;
	}

	lua_getglobal(script->L, "deinit");

	if (lua_isfunction(script->L, -1)) {
		if (lua_pcall(script->L, 0, 0, 0) != 0) {
			log_lua_error(script->L, "[%s]Error when running/calling deinit function: %s \n",
				      script->filename, lua_tostring(script->L, -1));
		}
	}
}

int call_script_getdata(int index, char *data_buffer, int buffer_size)
{
	if (index < 0 || index >= MAX_LUA_SCRIPTS) {
		return 1;
	}

	struct Lua_script *script = lua_scripts[index];

	if (script == NULL || script->L == NULL) {
		return 1;
	}

	lua_getglobal(script->L, "getdata");

	if (lua_isfunction(script->L, -1)) {
		if (lua_pcall(script->L, 0, 1, 0) != 0) {
			log_lua_error(script->L, "[%s]Error when running getdata function: %s \n",
				      script->filename, lua_tostring(script->L, -1));
			return 1;
		}

	if (!lua_isstring(script->L, -1)) {
		log_ferror(
			"[%s]Error when running getdata function it did not return a string value\n",
			script->filename);
	}

		strncpy(data_buffer, lua_tostring(script->L, -1), buffer_size);
		lua_pop(script->L, -1);
		return 0;
	}

	return 1;
}

int get_valid_script_count()
{
	return script_count;
}



int call_action_script(char *script_name, char *function_name, cJSON *message_json, char* return_buffer, int buffer_size){
	struct lua_State *L = lua_open();

	if(L == NULL){
		log_ferror("Error: could not open lua_state for [%s] action script", script_name);
	}

	luaL_openlibs(L);


	char script_path[256];
	strncpy(script_path, lua_script_dir, sizeof(script_path));
	strncat(script_path, "/", sizeof(script_path));
	strncat(script_path, script_name, sizeof(script_path));

	if (luaL_loadfile(L, script_path) || lua_pcall(L, 0, 0, 0)) {
		log_lua_error(L, "Cannot load action file [%s]: %s \n", script_path,
			      lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}

	lua_getglobal(L, function_name);

	if(!lua_isfunction(L, -1)){
		log_error("Error: function name parsed from action is not valid");
		lua_close(L);
		return 1;
	}


	if(convert_json_to_lua_table(L, message_json)){
		log_error("Error: could not convert action parameters to lua table");
		lua_close(L);
		return 1;
	}


	if (lua_pcall(L, 1, 1, 0) != 0) {
		log_lua_error(L, "[%s]Error when running %s function: %s \n",
					script_name, function_name, lua_tostring(L, -1));
		lua_close(L);
			return 1;
	}

	if (!lua_isstring(L, -1)) {
		log_ferror(
			"[%s]Error when running %s function it did not return a string value\n",
			script_name, function_name);
		lua_close(L);
			return 1;
	}

	strncpy(return_buffer, lua_tostring(L, -1), buffer_size);
	lua_pop(L, -1);

	lua_close(L);
	return 0;
}


/// @brief Converts tuya action messages inputParameters into lua table on the stack
/// @param *L Pointer to lua state
/// @param *message_json cJSON object containing messages json
/// @return 0 on success 1 otherwise
int convert_json_to_lua_table(struct lua_State *L, cJSON *message_json){

	if(message_json == NULL){
		return 1;
	}
	
	cJSON *current_item = cJSON_GetObjectItem(message_json,"inputParams");
	int param_count =  cJSON_GetArraySize(current_item);
	
	current_item = current_item->child;

	//Create empty table if there are no params
	if(param_count == 0){
		lua_createtable(L, 0, 1);
		lua_pushstring(L, "empty");
		lua_pushnil(L);
		lua_settable(L, -3);
		return 0;
	}
	
	lua_createtable(L, 0, param_count);

	for(int i = 0; i < param_count && current_item != NULL; i++){
		
		switch (current_item->type)
		{
		case cJSON_String:
			lua_pushstring(L, current_item->string);
			lua_pushstring(L, current_item->valuestring);
			lua_settable(L, -3);
			break;
		case cJSON_Number:
			lua_pushstring(L, current_item->string);
			lua_pushnumber(L, current_item->valueint);
			lua_settable(L, -3);
			break;
		default:
			lua_pushstring(L, "empty");
			lua_pushnil(L);
			lua_settable(L, -3);
			break;
		}

		current_item = current_item->next;
	}

	return 0;
	 
}

