#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

void solve(FILE *fp) {
    int rows = 0, cols = 0;
    char emp, obs, full;
    if (fscanf(fp, "%d %c %c %c\n", &rows, &emp, &obs, &full) != 4 ||
        rows <= 0 || emp == obs || emp == full || obs == full ||
        emp < 32 || emp > 126 || obs < 32 || obs > 126 || full < 32 || full > 126) {
        fprintf(stderr, "map error\n");
        return;
    }

    char **grid = calloc(rows, sizeof(char *));
    char *line = NULL;
    size_t cap = 0;

    for (int i = 0; i < rows; ++i) {
        ssize_t len = getline(&line, &cap, fp);
        if (len <= 0) { fprintf(stderr, "map error\n"); free(line); free(grid); return; }
        if (line[len - 1] == '\n') line[--len] = '\0';
        if (i == 0) cols = len;
        if (len != cols || cols <= 0) { fprintf(stderr, "map error\n"); free(line); free(grid); return; }
        for (int j = 0; j < cols; ++j) {
            if (line[j] != emp && line[j] != obs) {
                fprintf(stderr, "map error\n");
                free(line);
                for (int k = 0; k < i; ++k) free(grid[k]);
                free(grid);
                return;
            }
        }
        grid[i] = line;
        line = NULL;
        cap = 0;
    }

    if (getline(&line, &cap, fp) != -1) {
        free(line);
        for (int i = 0; i < rows; ++i) free(grid[i]);
        free(grid);
        fprintf(stderr, "map error\n");
        return;
    }
    free(line);

    int **dp = calloc(rows, sizeof(int *));
    int max = 0, mr = 0, mc = 0;
    for (int r = 0; r < rows; ++r) {
        dp[r] = calloc(cols, sizeof(int));
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == emp) {
                dp[r][c] = (r == 0 || c == 0) ? 1 : min3(dp[r-1][c], dp[r][c-1], dp[r-1][c-1]) + 1;
                if (dp[r][c] > max) { max = dp[r][c]; mr = r; mc = c; }
            }
        }
    }

    for (int r = mr - max + 1; r <= mr; ++r)
        for (int c = mc - max + 1; c <= mc; ++c)
            grid[r][c] = full;

    for (int r = 0; r < rows; ++r) {
        puts(grid[r]);
        free(grid[r]);
        free(dp[r]);
    }
    free(grid);
    free(dp);
}

int main(int ac, char **av) {
    if (ac == 1) solve(stdin);
    else {
        for (int i = 1; i < ac; ++i) {
            if (i > 1) putchar('\n');
            FILE *f = fopen(av[i], "r");
            if (f) { solve(f); fclose(f); }
            else fprintf(stderr, "map error\n");
        }
    }
    return 0;
}
