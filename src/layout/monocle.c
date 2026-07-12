#include "layout.h"

extern struct ashwc_server server;

void layout_monocle(struct ashwc_workspace *workspace) {
  struct ashwc_output *output = workspace->output;

  uint32_t outer = server.config->outer_gaps;
  uint32_t border = server.config->border_width;

  int x = output->usable_area.x + outer + border;
  int y = output->usable_area.y + outer + border;

  int width = output->usable_area.width - 2 * outer - 2 * border;

  int height = output->usable_area.height - 2 * outer - 2 * border;

  struct ashwc_toplevel *t;

  wl_list_for_each(t, &workspace->masters, link) {
    toplevel_set_pending_state(t, x, y, width, height);
  }

  wl_list_for_each(t, &workspace->slaves, link) {
    toplevel_set_pending_state(t, x, y, width, height);
  }
}


struct ashwc_toplevel *layout_monocle_next(struct ashwc_workspace *workspace,
                                           struct ashwc_toplevel *current) {
  struct wl_list *next = current->link.next;

  if (toplevel_is_master(current)) {
    if (next == &workspace->masters) {
      next = workspace->slaves.next;
    }
  } else {
    if (next == &workspace->slaves) {
      next = workspace->masters.next;
    }
  }

  if (next == &workspace->masters || next == &workspace->slaves) {
    return current;
  }

  return wl_container_of(next, current, link);
}

struct ashwc_toplevel *layout_monocle_prev(struct ashwc_workspace *workspace,
                                           struct ashwc_toplevel *current) {
  struct wl_list *prev = current->link.prev;

  if (toplevel_is_master(current)) {
    if (prev == &workspace->masters) {
      prev = workspace->slaves.prev;
    }
  } else {
    if (prev == &workspace->slaves) {
      prev = workspace->masters.prev;
    }
  }

  if (prev == &workspace->masters || prev == &workspace->slaves) {
    return current;
  }

  return wl_container_of(prev, current, link);
}

void layout_monocle_swap(struct ashwc_toplevel *a, struct ashwc_toplevel *b) {
  if (a == NULL || b == NULL || a == b) {
    return;
  }

  layout_swap_tiled_toplevels(a, b);
}
