#ifndef CVIDEO_H
#define CVIDEO_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define CVIDEO_PERCENTAGE_BAR_X_FACTOR 10
#define CVIDEO_PERCENTAGE_BAR_Y_FACTOR 2

#define CVIDEO_STD_X 60
#define CVIDEO_STD_Y 20

#define CVIDEO_FLAG_VERBOSE   0x00000001
#define CVIDEO_FLAG_AUTOCLEAR 0x00000002

struct coordinate {
    int x;
    int y;
};

struct cvideo_cfg {
    int video_x, video_y, video_len;
    char char_void, char_point;
    char *video_mem;
    int flags;
    int cursor_x, cursor_y;
};

int cvideo_stdinit(int x, int y, struct cvideo_cfg *cfg);
int cvideo_initialize(struct cvideo_cfg *cfg);
int cvideo_cleanup(struct cvideo_cfg *cfg);
void cvideo_clear_screen(struct cvideo_cfg cfg);
int cvideo_draw_line(int ax, int ay, int bx, int by, struct cvideo_cfg *cfg);
int cvideo_get_point_by_xy(int x, int y, struct cvideo_cfg cfg);
int cvideo_set(int x, int y, struct cvideo_cfg *cfg);
int cvideo_unset(int x, int y, struct cvideo_cfg *cfg);
int cvideo_print(struct cvideo_cfg cfg);
int cvideo_move(int x, int y, struct cvideo_cfg *cfg);
int cvideo_map(int x, int y, struct cvideo_cfg *dst, struct cvideo_cfg src);
int cvideo_prints(char *s, struct cvideo_cfg *cfg);
int cvideo_percentage_bar(double percent, struct cvideo_cfg *cfg);
int cvideo_overlap(int x, int y, struct cvideo_cfg *overlapped, struct cvideo_cfg src);
int cvideo_cursor_rewind(struct cvideo_cfg *cfg);

#endif
