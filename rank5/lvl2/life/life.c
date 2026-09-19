#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int **alloc_grid(int w, int h) {
    int **g = calloc(h, sizeof(int *));
    for (int i = 0; i < h; ++i) g[i] = calloc(w, sizeof(int));
    return g;
}

void free_grid(int **g, int h) {
    for (int i = 0; i < h; ++i) free(g[i]);
    free(g);
}

int count_n(int **g, int w, int h, int x, int y) {
    int n = 0;
    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
            if ((i || j) && y + i >= 0 && y + i < h && x + j >= 0 && x + j < w)
                n += g[y + i][x + j];
    return n;
}

int main(int ac, char **av) {
    if (ac != 4) return 1;
    int w = atoi(av[1]), h = atoi(av[2]), it = atoi(av[3]);
    if (w <= 0 || h <= 0 || it < 0) return 1;

    int **b = alloc_grid(w, h);
    int x = 0, y = 0, pen = 0;
    char c;
    while (read(0, &c, 1) > 0) {
        if (c == 'x') pen = !pen;
        else if (c == 'w' && y > 0) y--;
        else if (c == 'a' && x > 0) x--;
        else if (c == 's' && y < h - 1) y++;
        else if (c == 'd' && x < w - 1) x++;
        if (pen) b[y][x] = 1;
    }

    while (it--) {
        int **next = alloc_grid(w, h);
        for (int r = 0; r < h; ++r) {
            for (int col = 0; col < w; ++col) {
                int n = count_n(b, w, h, col, r);
                next[r][col] = (b[r][col] && (n == 2 || n == 3)) || (!b[r][col] && n == 3);
            }
        }
        free_grid(b, h);
        b = next;
    }

    for (int r = 0; r < h; ++r) {
        for (int col = 0; col < w; ++col) putchar(b[r][col] ? 'O' : ' ');
        putchar('\n');
    }
    free_grid(b, h);
    return 0;
}
