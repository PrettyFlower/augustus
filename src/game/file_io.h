#ifndef GAME_FILE_IO_H
#define GAME_FILE_IO_H

#include "scenario/data.h"

typedef struct {
    int mission;
    int custom_mission;
    int treasury;
    int population;
    int month;
    int year;
} saved_game_info;

typedef struct {
    uint8_t description[MAX_BRIEF_DESCRIPTION];
    int image_id;
    int start_year;
    int climate;
    int map_size;
    int total_invasions;
    int player_rank;
    int is_open_play;
    int open_play_id;
    scenario_win_criteria win_criteria;
} scenario_info;

int game_file_io_read_scenario(const char *filename);

int game_file_io_read_scenario_info(const char *filename, scenario_info *info);

int game_file_io_write_scenario(const char *filename);

int game_file_io_read_saved_game(const char *filename, int offset);

int game_file_io_read_saved_game_info(const char *filename, saved_game_info *info);

int game_file_io_write_saved_game(const char *filename);

int game_file_io_delete_saved_game(const char *filename);

int zlib_decompress(const void* input_buffer, int input_length, void* output_buffer, int* output_length);

int zlib_compress(const void* input_buffer, int input_length, void* output_buffer, int output_buffer_length, int* output_length);

#endif // GAME_FILE_IO_H
