#include "layout.h"

extern struct ashwc_server server;

void layout_grid(struct ashwc_workspace *workspace) {
  struct ashwc_output *output = workspace->output;

  uint32_t count =
      wl_list_length(&workspace->masters) + wl_list_length(&workspace->slaves);

  if (count == 0) {
    return;
  }

  uint32_t outer = server.config->outer_gaps;
  uint32_t inner = server.config->inner_gaps;
  uint32_t border = server.config->border_width;

  uint32_t cols = ceil(sqrt((double)count));
  uint32_t rows = (count + cols - 1) / cols;

  uint32_t cell_width = (output->usable_area.width - 2 * outer -
                         (cols - 1) * 2 * inner - cols * 2 * border) /
                        cols;

  uint32_t cell_height = (output->usable_area.height - 2 * outer -
                          (rows - 1) * 2 * inner - rows * 2 * border) /
                         rows;

  uint32_t i = 0;
  struct ashwc_toplevel *t;

#define PLACE_WINDOW(win)                                                      \
  do {                                                                         \
    uint32_t row = i / cols;                                                   \
    uint32_t col = i % cols;                                                   \
    uint32_t x = output->usable_area.x + outer +                               \
                 col * (cell_width + 2 * border + 2 * inner) + border;         \
    uint32_t y = output->usable_area.y + outer +                               \
                 row * (cell_height + 2 * border + 2 * inner) + border;        \
    toplevel_set_pending_state(win, x, y, cell_width, cell_height);            \
    i++;                                                                       \
  } while (0)

  wl_list_for_each(t, &workspace->masters, link) { PLACE_WINDOW(t); }

  wl_list_for_each(t, &workspace->slaves, link) { PLACE_WINDOW(t); }

#undef PLACE_WINDOW
}

struct ashwc_toplevel *
layout_grid_find_neighbor(struct ashwc_workspace *workspace,
                          struct ashwc_toplevel *current,
                          enum ashwc_direction direction) {
  size_t count =
      wl_list_length(&workspace->masters) + wl_list_length(&workspace->slaves);

  if (count <= 1)
    return current;

  size_t cols = ceil(sqrt((double)count));
  size_t rows = (count + cols - 1) / cols;

  struct ashwc_toplevel *windows[count];

  size_t i = 0;

  struct ashwc_toplevel *t;

  wl_list_for_each(t, &workspace->masters, link) windows[i++] = t;

  wl_list_for_each(t, &workspace->slaves, link) windows[i++] = t;

  size_t index = 0;

  for (i = 0; i < count; i++) {
    if (windows[i] == current) {
      index = i;
      break;
    }
  }

  int row = index / cols;
  int col = index % cols;

  switch (direction) {
  case ASHWC_LEFT:
    col--;
    break;

  case ASHWC_RIGHT:
    col++;
    break;

  case ASHWC_UP:
    row--;
    break;

  case ASHWC_DOWN:
    row++;
    break;
  }

  if (row < 0 || row >= (int)rows)
    return current;

  if (col < 0 || col >= (int)cols)
    return current;

  size_t target = row * cols + col;

  if (target >= count)
    return current;

  return windows[target];
}
