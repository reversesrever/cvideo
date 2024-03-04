#include "cvideo.h"

int cvideo_stdinit(int x, int y, struct cvideo_cfg* cfg) {
    cfg->char_point = '.';
    cfg->char_void = ' ';
    cfg->flags = CVIDEO_FLAG_AUTOCLEAR;
    cfg->video_x = x;
    cfg->video_y = y;
    cfg->video_len = cfg->video_x * cfg->video_y;
    return cvideo_initialize(cfg);
}

int cvideo_initialize(struct cvideo_cfg* cfg) {
    cfg->video_mem = (char*) malloc(sizeof(char) * cfg->video_len);
    if (cfg->video_mem == NULL) {
        if (cfg->flags & CVIDEO_FLAG_VERBOSE) {
            printf("Could not allocate memory \n");
        }
        return -1;
    }
    cfg->cursor_x = 0;
    cfg->cursor_y = cfg->video_y - 1;
    memset((char*) cfg->video_mem, cfg->char_void, cfg->video_len);
    return 0;
}
int cvideo_cleanup(struct cvideo_cfg* cfg) {
    free(cfg->video_mem);
    return 0;
}

void cvideo_clear_screen(struct cvideo_cfg cfg) {
    system("clear");
}

int cvideo_draw_line(int ax, int ay, int bx, int by, struct cvideo_cfg* cfg) {
	int i, j, px, py, d, n;
	float m;
	int s;
	if (ax == bx) {
		for (i = ay; i < by + 1; i++) {
			cvideo_set(ax, i, cfg);
		}
	} else if (ay == by) {
		for (i = ax; i < bx + 1; i++) {
			cvideo_set(i, ay, cfg);
		}
	} else {
		++bx;
		m = ((float) by - ay) / ((float) bx - ax - 1);
		px = ax;
		py = ay;
		j = 0;
		if (ay < by) {
			s = py < by + 1;
		} else if (ay > by) {
			s = py > by - 1;
		}
		// else s = true;
		while ((px < bx) && s) {
			py = round (m * ((float) j)) + ay;
			n = round (m * ((float) (j + 1))) + ay;
			if (ay < by) 
                d = n - py;
			else if (by < ay) 
                d = py - n;
			if (d == 0) {
				cvideo_set(px, py, cfg);
				++px;
				++j;
			} else {
				for (i = 0; i < d; i++) {
					cvideo_set(px, py, cfg);
					if (ay < by)
						++py;
					else
						--py;
				}
				++px;
				++j;
			}
			if (ay < by)
				s = py < by + 1;
			else if (ay > by)
				s = py > by - 1;
			// else s = true;
		}
	}
	return 0;
}

int cvideo_get_point_by_xy(int x, int y, struct cvideo_cfg cfg) {
    return ((cfg.video_x * cfg.video_y) - ((cfg.video_x * y) + (cfg.video_x-x-1))) - 1;
}

int cvideo_set(int x, int y, struct cvideo_cfg* cfg) {
    if (cfg->video_mem == NULL)
        return -1;
    if (x > cfg->video_x)
        return -1;
    if (x < 0)
        return -1;
    if (y > cfg->video_y)
        return -1;
    if (y < 0)
        return -1;
    cfg->video_mem[cvideo_get_point_by_xy(x, y, *cfg)] = cfg->char_point;
    return 0;
}

int cvideo_unset(int x, int y, struct cvideo_cfg* cfg) {
    if (cfg->video_mem == NULL)
        return -1;
    if (x > cfg->video_x)
        return -1;
    if (x < 0)
        return -1;
    if (y > cfg->video_y)
        return -1;
    if (y < 0)
        return -1;
    cfg->video_mem[cvideo_get_point_by_xy(x, y, *cfg)] = cfg->char_void;
    return 0;
}

int cvideo_print(struct cvideo_cfg cfg) {
    int x, y, n;
    n = 0;
    if (cfg.flags & CVIDEO_FLAG_AUTOCLEAR) {
        cvideo_clear_screen(cfg);
    }
    printf(" ");
    for (x = 0; x < cfg.video_x; x++)
        printf("_");
    printf("\n");
    for (y = 0; y < cfg.video_y; y++) {
        printf("|");
        for (x=0;x<cfg.video_x;x++)
            printf("%c", cfg.video_mem[n++]);
        printf("| \n");
    }
    printf("|");
    for (x = 0; x < cfg.video_x; x++)
        printf("_");
    printf("|\n");
    return 0;
}

int cvideo_move(int x, int y, struct cvideo_cfg* cfg) {
    int x_i, y_i;
    char* buffer;
    buffer = (char*) malloc(sizeof(char) * cfg->video_len);
    if (buffer == NULL) {
        if (cfg->flags & CVIDEO_FLAG_VERBOSE) {
            printf("Could not allocate memory \n");
        }
        return -1;
    }
    memset(buffer, cfg->char_void, cfg->video_len);
    for (y_i = 0; y_i < cfg->video_y; y_i++) {
        for(x_i = 0; x_i < cfg->video_x; x_i++) {
            if(x_i + x < cfg->video_x && x_i + x >= 0 && y_i + y < cfg->video_y && y_i + y >= 0) {
                buffer[cvideo_get_point_by_xy(x_i + x, y_i + y, *cfg)] = cfg->video_mem[cvideo_get_point_by_xy(x_i, y_i, *cfg)];
            }
        }
    }
    memcpy((char*) cfg->video_mem, (char*) buffer, cfg->video_len);
    free(buffer);
    return 0;
}

int cvideo_map(int x, int y, struct cvideo_cfg* dst, struct cvideo_cfg src) {
    int src_x, src_y;
    for (src_y = src.video_y - 1; src_y > -1; src_y--) {
        for (src_x = 0; src_x < src.video_x; src_x++) {
            dst->video_mem[cvideo_get_point_by_xy(src_x + x, src_y + y, *dst)] = src.video_mem[cvideo_get_point_by_xy(src_x, src_y, src)];
        }
    }
    return 0;
}

int cvideo_prints(char* s, struct cvideo_cfg* cfg) {
    int n, len;
    len = strlen(s);
    if (cfg->cursor_y - 1 < -1) {
        cvideo_move(0, 1, cfg);
        cfg->cursor_x=0;
        cfg->cursor_y++;
    }
    for (cfg->cursor_y, n=0; ; cfg->cursor_y--) {
        if (cfg->cursor_x > cfg->video_x - 1)
            cfg->cursor_x=0;
        for (cfg->cursor_x; cfg->cursor_x < cfg->video_x; cfg->cursor_x++) {
            if (n > len - 1)
                return 0;
            switch (s[n]) {
                case '\n':
                    if (cfg->cursor_y-1>=0)
                        cfg->cursor_y--;
                    else
                        cvideo_move(0, 1, cfg);
                    cfg->cursor_x = -1;
                    n++;
                    break;
                case '\r':
                    cfg->cursor_x = -1;
                    n++;
                    break;
                case '\b':
                    if (n - 1 >= 0)
                        if (s[n - 1] == '\b')
                            if (cfg->cursor_y + 1 <= cfg->video_y) {
                                cfg->cursor_y++;
                                cfg->cursor_x = -1;
                            }
                    if (cfg->cursor_x -1 >= 0)
                        cfg->cursor_x -= 2;
                    n++;
                    break;
                default:
                    if (cfg->cursor_y < 0) {
                        cvideo_move(0, 1, cfg);
                        cfg->cursor_x = 0;
                        cfg->cursor_y++;
                    }
                    cfg->video_mem[cvideo_get_point_by_xy(cfg->cursor_x, cfg->cursor_y, *cfg)] = s[n++];
                    break;
            }
        }
    }
    if (cfg->flags & CVIDEO_FLAG_VERBOSE)
        printf("Out of video memory \n");
    return -1;
}

int cvideo_percentage_bar(double percent, struct cvideo_cfg* cfg) {
    char save_char_point, save_cursor_x, save_cursor_y;
    char percent_s[BUFSIZ];
    if (percent < 0 || percent > 100)
        return -1;
    save_char_point = cfg->char_point;
    cfg->char_point = '-';
    cvideo_draw_line(cfg->video_x / CVIDEO_PERCENTAGE_BAR_X_FACTOR,
    cfg->video_y / CVIDEO_PERCENTAGE_BAR_Y_FACTOR,
    cfg->video_x - (cfg->video_x / CVIDEO_PERCENTAGE_BAR_X_FACTOR),
    cfg->video_y / CVIDEO_PERCENTAGE_BAR_Y_FACTOR,
    cfg);
    cfg->char_point = '#';
    cvideo_draw_line(cfg->video_x / CVIDEO_PERCENTAGE_BAR_X_FACTOR,
    cfg->video_y / CVIDEO_PERCENTAGE_BAR_Y_FACTOR,
    (cfg->video_x - (cfg->video_x / CVIDEO_PERCENTAGE_BAR_X_FACTOR)) * (percent / 100),
    cfg->video_y / CVIDEO_PERCENTAGE_BAR_Y_FACTOR,
    cfg);
    cfg->char_point = '[';
    cvideo_set(cfg->video_x / CVIDEO_PERCENTAGE_BAR_X_FACTOR,
    cfg->video_y / CVIDEO_PERCENTAGE_BAR_Y_FACTOR,
    cfg);
    cfg->char_point = ']';
    cvideo_set(cfg->video_x - (cfg->video_x / CVIDEO_PERCENTAGE_BAR_X_FACTOR),
    cfg->video_y / CVIDEO_PERCENTAGE_BAR_Y_FACTOR,
    cfg);
    save_cursor_x = cfg->cursor_x;
    save_cursor_y = cfg->cursor_y;
    sprintf(percent_s, "  %.2lf%%  ", percent);
    cfg->cursor_x = (cfg->video_x / CVIDEO_PERCENTAGE_BAR_Y_FACTOR) - strlen(percent_s) / CVIDEO_PERCENTAGE_BAR_Y_FACTOR;
    cfg->cursor_y = cfg->video_y / CVIDEO_PERCENTAGE_BAR_Y_FACTOR;
    cvideo_prints(percent_s, cfg);
    cfg->char_point = save_char_point;
    cfg->cursor_x = save_cursor_x;
    cfg->cursor_y = save_cursor_y;
    return 0;
} 

int cvideo_overlap(int x, int y, struct cvideo_cfg* overlapped, struct cvideo_cfg src) {
    int src_x, src_y;
    for (src_y = src.video_y - 1; src_y > -1; src_y--) {
        for (src_x = 0; src_x < src.video_x; src_x++) {
            if (src.video_mem[cvideo_get_point_by_xy(src_x, src_y, src)] != src.char_void) {
                overlapped->video_mem[cvideo_get_point_by_xy(src_x + x, src_y + y, *overlapped)] = src.video_mem[cvideo_get_point_by_xy(src_x, src_y, src)];
            }
        }
    }
    return 0;
}

int cvideo_cursor_rewind(struct cvideo_cfg *cfg) {
    cfg->cursor_x = 0;
    cfg->cursor_y = cfg->video_y - 1;
    return 0;
}