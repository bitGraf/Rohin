#pragma once

#include "Engine/Defines.h"

struct memory_arena;

enum class config_token_type {
    BOOL,
    INT,
    FLOAT,
    STRING,
    STRUCT,
    NONE
};

struct config_token {
    // represents a key-value pair
    char* name;

    config_token_type type;
    union value_t {
        bool32 as_bool;
        int32  as_int;
        real32 as_float;
        char*  as_string;
        uint32 struct_level;
    } value;

    int32 id; // probably unnecessary
    int32 parent_id;
    int32 last_child;
};
struct config_file {
    bool32 load_successful;
    memory_arena* arena;
    config_token* tokens; // dynarray
};

RHAPI config_file parse_config_from_file(memory_arena* arena, const char* full_filename, const char* simple_name);
RHAPI config_file parse_config_from_buffer(memory_arena* arena, uint8* buffer, uint64 buffer_size, const char* simple_name);
RHAPI int32  get_config_int(config_file*    file, char* var_name, int32  default_value);
RHAPI bool32 get_config_bool(config_file*   file, char* var_name, bool32 default_value);
RHAPI real32 get_config_float(config_file*  file, char* var_name, real32 default_value);
RHAPI char*  get_config_string(config_file* file, char* var_name, char*  default_value);