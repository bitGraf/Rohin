#include "Config.h"

#include "Engine/Core/String.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Asserts.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Memory/Memory_Arena.h"
#include "Engine/Platform/Platform.h"

struct parse_context {
    uint32 indent_level;

    uint32 current_struct_id;
    char* struct_name;
    uint64 struct_name_length;
};

#define CHECK_IF_BOOL_TRUE(str) (          \
    (string_compare(str, "True", 4)==0) || \
    (string_compare(str, "true", 4)==0) || \
    (string_compare(str, "Yes", 3)==0) ||  \
    (string_compare(str, "yes", 3)==0))

#define CHECK_IF_BOOL_FALSE(str) (          \
    (string_compare(str, "False", 5)==0) || \
    (string_compare(str, "False", 5)==0) || \
    (string_compare(str, "No", 2)==0) ||    \
    (string_compare(str, "no", 2)==0))


void parse_config_line(config_file* file, char* line, uint64 line_length, parse_context* context) {
    int32 next_token_id = (int32)GetArrayCount(file->tokens);

    char SPACE_BUFFER[1025] = {};
    memory_set(SPACE_BUFFER, ' ', sizeof(SPACE_BUFFER)-1);
    SPACE_BUFFER[1024] = 0;

    //RH_DEBUG("Parsing line [%.*s]", line_length, line);
    char* line_end = line + line_length;
    if (string_only_whitespace(line, line_end)) return;

    char* start = line;
    for (uint32 lev = 0; lev < context->indent_level; lev++) {
        if (string_compare(start, "    ", 4) == 0) {
            start = start + 4;
        } else {
            // struct should be over now.
            // multiple structs might have ended, 
            // so we have to check what our new level is.

            config_token* cur_struct = &file->tokens[context->current_struct_id];
            while (cur_struct->value.struct_level > lev) {
                cur_struct = &file->tokens[cur_struct->parent_id];
            }

            context->indent_level = cur_struct->value.struct_level;
            context->current_struct_id = cur_struct->id;
        }
    }

    // check for tokens
    if (start[0] == '#') {
        //RH_INFO("Line comment");
    } else if (string_compare(start, "---", 3) == 0) {
        RH_WARN("File separations not considered yet!");
    } else {
        config_token new_token;
        new_token.id = next_token_id;

        char*  name_str = start;
        uint64 name_length = string_find_first(start, line_end, ':');

        new_token.name = PushArray(file->arena, char, name_length + 1);
        memory_copy(new_token.name, name_str, name_length);
        new_token.name[name_length] = 0;

        char* value_str = string_skip_whitespace(start + name_length + 1, line_end);
        uint64 l1 = string_find_first(value_str, line_end, '\n');
        uint64 l2 = string_find_first(value_str, line_end, '#');
        uint64 value_length = (l1 < l2) ? l1 : l2;
        if (value_length == 0 || string_only_whitespace(value_str, value_str + value_length)) {
            value_length = 0;

            // start of a structure
            context->struct_name = new_token.name;
            context->struct_name_length = name_length;

            new_token.type = config_token_type::STRUCT;
            context->indent_level++;
            new_token.value.struct_level = context->indent_level;

            new_token.parent_id = context->current_struct_id;
            file->tokens[new_token.parent_id].last_child = new_token.id;
            context->current_struct_id = new_token.id;


            //RH_INFO("%.*s%.*s={", context->indent_level*4, SPACE_BUFFER, name_length, name_str);
        } else {
            new_token.parent_id = context->current_struct_id;
            file->tokens[new_token.parent_id].last_child = new_token.id;
            if (string_is_numeric(value_str, value_str + value_length)) {
                if (string_contains(value_str, value_str + value_length, '.')) {
                    // its a float
                    new_token.value.as_float = string_to_float(value_str, value_length);
                    new_token.type = config_token_type::FLOAT;
                    //RH_INFO("%.*s%.*s=%f <FLOAT>", context->indent_level*4, SPACE_BUFFER, name_length, name_str, new_token.value.as_float);
                } else {
                    // its an integar
                    new_token.value.as_int = string_to_int(value_str, value_length);
                    new_token.type = config_token_type::INT;
                    //RH_INFO("%.*s%.*s=%d <INT>", context->indent_level*4, SPACE_BUFFER, name_length, name_str, new_token.value.as_int);
                }
            } else {
                if (CHECK_IF_BOOL_TRUE(value_str)) {
                    // its a bool-yes
                    new_token.value.as_bool = true;
                    new_token.type = config_token_type::BOOL;
                    //RH_INFO("%.*s%.*s=%s <BOOL>", context->indent_level*4, SPACE_BUFFER, name_length, name_str, new_token.value.as_bool ? "True" : "False");
                } else if (CHECK_IF_BOOL_FALSE(value_str)) {
                    // its a bool-no
                    new_token.value.as_bool = false;
                    new_token.type = config_token_type::BOOL;
                    //RH_INFO("%.*s%.*s=%s <BOOL>", context->indent_level*4, SPACE_BUFFER, name_length, name_str, new_token.value.as_bool ? "True" : "False");
                } else {
                    // its a string
                    new_token.value.as_string = PushArray(file->arena, char, value_length + 1);
                    memory_copy(new_token.value.as_string, value_str, value_length);
                    new_token.value.as_string[value_length] = 0;
                    new_token.type = config_token_type::STRING;
                    //RH_INFO("%.*s%.*s=\"%s\" <STRING>", context->indent_level*4, SPACE_BUFFER, name_length, name_str, new_token.value.as_string);
                }
            }
        }

        ArrayPushValue(file->tokens, new_token);

        //RH_INFO("Token: [%.*s]\n         Value: '%.*s'", new_token.name_length, new_token.name_str, value_length, value_str);
    }
}

config_file parse_config_from_file(memory_arena* arena, const char* full_filename, const char* simple_name) {
    file_handle file = platform_read_entire_file(full_filename);
    if (!file.num_bytes) {
        RH_ERROR("Failed to read resource file");

        config_file result = {};
        result.arena = nullptr;
        result.tokens = nullptr;
        result.load_successful = false;
        return result;
    }
    RH_TRACE("File '%s' read: %llu bytes", full_filename, file.num_bytes);

    return parse_config_from_buffer(arena, file.data, file.num_bytes, simple_name);
}

config_file parse_config_from_buffer(memory_arena* arena, uint8* buffer, uint64 buffer_size, const char* simple_name) {
    RH_INFO("Parsing config file: '%s'", simple_name);
    // first, turn all \r into \n, so that \r\n is just \n\n and \n works as well
    string_replace((char*)buffer, buffer_size, '\r', '\n');

    // count number of valid lines (not comments, not empty)
    uint64 num_lines = 0;
    {
        char* buffer_end = (char*)buffer + buffer_size;
        char* line = (char*)buffer;
        uint64 line_length = string_find_first(line, buffer_end, '\n');
        while (line && line_length) {
            if (!(string_only_whitespace(line, line+line_length) || (string_first_non_whitespace_char(line, line+line_length) == '#'))) {
                num_lines++;
            }

            // get next line
            line = get_next_line(line, buffer_end, &line_length);
        }
    }
    RH_TRACE("Done prepass: %d valid lines", num_lines);
    
    config_file result = {};
    result.load_successful = false;
    result.arena = arena;
    result.tokens = CreateArray(arena, config_token, num_lines + 1); // num valid lines +1 for the root node.

    // create root node
    config_token root_node;
    root_node.id = 0;
    root_node.name = "root";
    root_node.type = config_token_type::STRUCT;
    root_node.value.struct_level = 0;
    root_node.parent_id = -1;
    ArrayPushValue(result.tokens, root_node);

    parse_context context;
    context.indent_level = 0;
    context.current_struct_id = 0; // root node

    char* buffer_end = (char*)buffer + buffer_size;

    // split line by line
    uint64 line_number = 1;
    char* line = (char*)buffer;
    uint64 line_length = string_find_first(line, buffer_end, '\n');
    while (line && line_length) {
        //RH_TRACE("LINE %d: [%.*s]", line_number++, line_length, line);

        parse_config_line(&result, line, line_length, &context);

        // get next line
        line = get_next_line(line, buffer_end, &line_length);
    }

    // 
    uint64 num_tokens = GetArrayCount(result.tokens);
    RH_DEBUG("Parsed %llu tokens!", num_tokens);
    
    bool32 PRINT_LAYOUT = false;
    if (PRINT_LAYOUT) {
        RH_DEBUG("------- Layout -------------------------", num_tokens);

        char SPACE_BUFFER[1025] = {};
        memory_set(SPACE_BUFFER, ' ', sizeof(SPACE_BUFFER)-1);
        SPACE_BUFFER[1024] = 0;

        for (uint64 n = 0; n < num_tokens; n++) {
            const config_token& tok = result.tokens[n];

            switch (tok.type) {
                case config_token_type::STRUCT: {
                    RH_DEBUG("%.*s%s  [%d|%d|%d]", tok.value.struct_level * 4, SPACE_BUFFER, tok.name, tok.id, tok.parent_id, tok.last_child);
                } break;
                case config_token_type::INT: {
                    config_token parent = result.tokens[tok.parent_id];
                    RH_DEBUG("%.*s%s  [%d|%d|%d]", (parent.value.struct_level + 1)*4, SPACE_BUFFER, tok.name, tok.id, tok.parent_id, tok.last_child);
                } break;
                case config_token_type::FLOAT: {
                    config_token parent = result.tokens[tok.parent_id];
                    RH_DEBUG("%.*s%s  [%d|%d|%d]", (parent.value.struct_level + 1)*4, SPACE_BUFFER, tok.name, tok.id, tok.parent_id, tok.last_child);
                } break;
                case config_token_type::BOOL: {
                    config_token parent = result.tokens[tok.parent_id];
                    RH_DEBUG("%.*s%s  [%d|%d|%d]", (parent.value.struct_level + 1)*4, SPACE_BUFFER, tok.name, tok.id, tok.parent_id, tok.last_child);
                } break;
                case config_token_type::STRING: {
                    config_token parent = result.tokens[tok.parent_id];
                    RH_DEBUG("%.*s%s  [%d|%d|%d]", (parent.value.struct_level + 1)*4, SPACE_BUFFER, tok.name, tok.id, tok.parent_id, tok.last_child);
                } break;
            }
        }
        RH_DEBUG("----------------------------------------", num_tokens);
    }

    //uint32 num_val = *AdvanceBuffer(&buffer, uint32, End);
    result.load_successful = true;
    return result;
}

config_token* config_find_token(config_file* file, char* var_name) {
    int32 num_tokens = (int32)GetArrayCount(file->tokens);
    uint64 var_len = string_length(var_name);
    char* var_end = var_name + var_len;

    if (string_contains(var_name, var_end, '.')) {
        uint64 delim = string_find_first(var_name, var_end, '.');
        char*  search_name = var_name;
        uint64 search_len = delim;

        bool found = false;
        bool at_end = false;
        int32 parent_id = 0;
        int32 struct_id = 0;
        int32 last_child = num_tokens;
        while (!found && !at_end) {
            config_token* found_tok = NULL;
            for (int32 n = (struct_id+1); n <= last_child; n++) {
                config_token* tok = &file->tokens[n];
                if (tok->parent_id == parent_id && tok->type==config_token_type::STRUCT) {
                    if (string_compare(tok->name, search_name, search_len) == 0) {
                        // found
                        found_tok = tok;
                        struct_id = n;
                        parent_id = struct_id;
                        last_child = tok->last_child;
                        break;
                    }
                }
                at_end = (tok->id == last_child);
            }

            if (at_end) return nullptr;
            search_name += search_len+1;
            //found = (found_tok != NULL);

            if (string_contains(search_name, var_end, '.')) {
                // repeat
                search_len = string_find_first(search_name, var_end, '.');
                continue;
            } else {
                for (int32 n = (struct_id+1); n <= last_child; n++) {
                    config_token* tok = &file->tokens[n];
                    if (string_compare(tok->name, search_name) == 0) {
                        if (tok->parent_id == struct_id)
                            return tok;
                        else
                            return nullptr;
                    }
                }
            }
            return nullptr;
        }

    } else {
        for (int32 n = 1; n < num_tokens; n++) {
            config_token* tok = &file->tokens[n];
            if (string_compare(tok->name, var_name) == 0) {
                return tok;
            }
        }
    }

    return nullptr;
}

int32 get_config_int(config_file* file, char* var_name, int32 default_value) {
    AssertMsg(file->arena && file->tokens, "ConfigFile not initialized!");
    config_token* tok = config_find_token(file, var_name);

    if (tok && tok->type==config_token_type::INT) {
        return tok->value.as_int;
    } else {
        return default_value;
    }
}

bool32 get_config_bool(config_file* file, char* var_name, bool32 default_value) {
    AssertMsg(file->arena && file->tokens, "ConfigFile not initialized!");
    config_token* tok = config_find_token(file, var_name);

    if (tok && tok->type==config_token_type::INT) {
        return tok->value.as_bool;
    } else {
        return default_value;
    }
}

real32 get_config_float(config_file* file, char* var_name, real32 default_value) {
    AssertMsg(file->arena && file->tokens, "ConfigFile not initialized!");
    config_token* tok = config_find_token(file, var_name);

    if (tok && tok->type==config_token_type::INT) {
        return tok->value.as_float;
    } else {
        return default_value;
    }
}

char* get_config_string(config_file* file, char* var_name, char* default_value) {
    AssertMsg(file->arena && file->tokens, "ConfigFile not initialized!");
    config_token* tok = config_find_token(file, var_name);

    if (tok && tok->type==config_token_type::STRING) {
        return tok->value.as_string;
    } else {
        return default_value;
    }
}