#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "math.h"

struct FileData{
    const char* filename;
    void* file_data;
    unsigned long long file_size;
};

struct Line{
    char* string;
    unsigned short length;
};

static inline long long get_size(const char* filename) {
    struct stat file_stats;
    if (stat(filename, &file_stats) == -1)
        return -1;
    return file_stats.st_size;
}

static inline void load_mapped(struct FileData* file_data){
    long long file_size = get_size(file_data->filename);
    int file = open(file_data->filename, O_RDONLY);

    file_data->file_data = mmap(0, file_size, PROT_READ, MAP_PRIVATE, file, 0);
    file_data->file_size = file_size;
    close(file);
}

static inline void load_c(struct FileData* file_data) {
    FILE *fp = fopen(file_data->filename, "rb");

    long long file_size = get_size(file_data->filename);

    file_data->file_data = malloc(file_size + 1);

    fread(file_data->file_data, 1, file_size, fp);

    fclose(fp);
}

static inline void load_c_pre(struct FileData* file_data) {
    FILE *fp = fopen(file_data->filename, "rb");

    fread(file_data->file_data, 1, file_data->file_size, fp);

    fclose(fp);
}


static inline void read_file(struct FileData* file_data) {
    file_data->file_size = get_size(file_data->filename);
    file_data->file_data = malloc(file_data->file_size + 1);

    int fd = open(file_data->filename, O_RDONLY);

    read(fd, file_data->file_data, file_data->file_size);

    close(fd);
}

static inline void read_file2(struct FileData* file_data) {
    file_data->file_size = get_size(file_data->filename);
    file_data->file_data = malloc(file_data->file_size + 1);

    int fd = open(file_data->filename, O_RDONLY);

    ssize_t total = 0;
    while (total < file_data->file_size) {
        ssize_t n = read(fd, (char*)file_data->file_data + total, file_data->file_size - total);
        if (n <= 0) break;
        total += n;
    }

    close(fd);
}

static inline vec3_t parse_vertex(struct Line line, uint16_t start_index){
    vec3_t result = {};
    size_t cord_index = start_index;

    result.x = parse_float(&line.string[cord_index++], &cord_index);
    result.y = parse_float(&line.string[cord_index++], &cord_index);
    result.z = parse_float(&line.string[cord_index++], &cord_index);

    return result;
}

static inline vec2_t parse_uv(struct Line line, const uint16_t start_index){
    vec2_t result = {};
    size_t cord_index = start_index;

    result.x = parse_float(&line.string[cord_index++], &cord_index);
    result.y = parse_float(&line.string[cord_index++], &cord_index);

    return result;
}

static inline size_t parse_indicie(struct Line line, const uint16_t start_index){
    uint32_t index_index = 0;
    uint8_t temp = 0;
    uint32_t indicies[12] = {};
    for (size_t i = start_index; i < line.length; i++){
        uint32_t value = parse_to_uint32(&line.string[i+1], &i);

        if(line.string[i] == '/' || line.string[i] == ' '){
            indicies[index_index] = value;
            index_index++;
            if(line.string[i] == '/') temp++;
        }
    }
    uint32_t values [9];
    if(index_index < 9){
        if(temp < 5){
            values[0] = indicies[0];
            values[1] = indicies[1];
            values[2] = 0;

            values[3] = indicies[2];
            values[4] = indicies[3];
            values[5] = 0;

            values[6] = indicies[4];
            values[7] = indicies[5];
            values[8] = 0;
        }else{
            values[0] = indicies[0];
            values[1] = 0;
            values[2] = indicies[1];

            values[3] = indicies[2];
            values[4] = 0;
            values[5] = indicies[3];

            values[6] = indicies[4];
            values[7] = 0;
            values[8] = indicies[5];
        }
    }
    else if(index_index > 9){
        uint32_t values2 [3];
        values2[0] = indicies[0];
        values2[1] = indicies[1];
        values2[2] = indicies[2];

         //memcpy(&values2[3], &indicies[6], sizeof(uint32_t) * 6);
    }
    memcpy(values, indicies, sizeof(uint32_t) * 9);
    return 0;
}


static inline bool get_line(struct Line* line,struct FileData data, long long* offset){
    return false;
    if(*offset > data.file_size) return false;
    while (((char*)data.file_data)[line->length] != '\n') {
        line->length++;
    }
    line->string = (char*)malloc(line->length * sizeof(char));

    memcpy(line->string, &((char*)data.file_data)[*offset], line->length * sizeof(char));
    *offset+=line->length;
    return true;
}
