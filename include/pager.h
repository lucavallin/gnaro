#ifndef PAGER_H
#define PAGER_H

#include <stdint.h>

enum {
  // PAGER_MAX_PAGES is the maximum number of pages that can be handled
  PAGER_MAX_PAGES = 100,
  // 4 kilobytes, same size as a virtual memory page in most architectures,
  // so that a database page corresponds to a single memory page for the OS.
  PAGER_PAGE_SIZE = 4096
};

// Pager is an abstraction that handles disk I/O.
typedef struct {
  int file_descriptor;
  uint32_t file_length;
  uint32_t num_pages;
  void *pages[PAGER_MAX_PAGES];
} Pager;

// Open the database file and keeps track of its size
Pager *pager_open(const char *filename);

// GET a pointer to the page with the given page number
void *pager_get_page(Pager *pager, uint32_t page_num);

// Write the page with the given page number to disk
void pager_flush(Pager *pager, uint32_t page_num);

// Get the page number of the first unused page
uint32_t pager_get_unused_page_num(Pager *pager);

#endif
