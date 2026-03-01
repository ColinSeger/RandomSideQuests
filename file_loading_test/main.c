#include "file_loader.h"
#include "../platform.h"

void test_mmap(){
    struct FileData cube = {};
    struct FileData napoleon = {};
    cube.filename = "Cube.obj";
    napoleon.filename = "napoleon.OBJ";

    struct Timer start = profile_start();

    load_mapped(&cube);

    profile_end(start);

    start = profile_start();

    load_mapped(&napoleon);

    profile_end(start);

    if(!cube.file_data) printf("Failed to load Cube \n");
    if(!napoleon.file_data) printf("Failed to load Napoleon \n");
}

void test_c_std(){
    struct FileData cube = {};
    struct FileData napoleon = {};
    cube.filename = "Cube.obj";
    napoleon.filename = "napoleon.OBJ";

    struct Timer start = profile_start();

    load_c(&cube);

    profile_end(start);

    start = profile_start();

    load_c(&napoleon);

    profile_end(start);

    if(!cube.file_data) printf("Failed to load Cube \n");
    if(!napoleon.file_data) printf("Failed to load Napoleon \n");
}

void test_c_std_pre_alloc(){
    struct FileData cube = {};
    struct FileData napoleon = {};
    cube.filename = "Cube.obj";
    napoleon.filename = "napoleon.OBJ";
    cube.file_size = get_size(cube.filename);
    napoleon.file_size = get_size(napoleon.filename);

    cube.file_data = malloc(cube.file_size + 1);
    napoleon.file_data = malloc(napoleon.file_size + 1);

    struct Timer start = profile_start();

    load_c_pre(&cube);

    profile_end(start);

    start = profile_start();

    load_c_pre(&napoleon);

    profile_end(start);

    if(!cube.file_data) printf("Failed to load Cube \n");
    if(!napoleon.file_data) printf("Failed to load Napoleon \n");
}

void test_read(){
    struct FileData cube = {};
    struct FileData napoleon = {};
    cube.filename = "Cube.obj";
    napoleon.filename = "napoleon.OBJ";

    struct Timer start = profile_start();

    read_file(&cube);

    profile_end(start);

    start = profile_start();

    read_file(&napoleon);

    profile_end(start);

    if(!cube.file_data) printf("Failed to load Cube \n");
    if(!napoleon.file_data) printf("Failed to load Napoleon \n");

    printf("%s", (char*)cube.file_data);
}

int main(){
    printf("mmap Load \n");
    test_mmap();
    struct FileData cube = {};
    cube.filename = "Cube.obj";
    cube.filename = "napoleon.OBJ";
    struct Timer start = profile_start();

    load_mapped(&cube);
    long long offset = 0;
    struct Line line = {};
    while (get_line(&line, cube, &offset)) {
        if(line.string[0] == 'v' && line.string[1] == ' '){
            parse_vertex(line, 2);
        }
        else if(line.string[0] == 'v' && line.string[1] == 't'){
            vec2_t t = parse_uv(line, 3);
        }
        else if(line.string[0] == 'v' && line.string[1] == 'n'){
            vec3_t t = parse_vertex(line, 3);
        }
        else if(line.string[0] == 'f' && line.string[1] == ' '){
            parse_indicie(line, 1);
        }
    }

    profile_end(start);

    return 0;
}
