#include "pager.h"
#include "log.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

Pager *pager_open(const char *filename) {
  log_debug("opening file %s...", filename);

  // Read/Write mode, create if not exists, user read/write permission
  int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

  if (fd == -1) {
    log_error("failed to open file %s", filename);
    exit(EXIT_FAILURE);
  }

  log_debug("allocating pager...");

  off_t file_length = lseek(fd, 0, SEEK_END);
  Pager *pager = malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_length = file_length;

  // Initialize pages to NULL
  for (uint32_t i = 0; i < PAGER_MAX_PAGES; i++) {
    log_debug("initializing page %d...", i);
    pager->pages[i] = NULL;
  }

  return pager;
}

// pager_get_page() can handle a cache miss. It assumes pages are saved one
// after the other in the database file: page 0 at offset 0, page 1 at offset
// 4096, page 2 at offset 8192, etc. If the requested page lies outside the
// bounds of the file, we know it should be blank, so we just allocate some
// memory and return it. The page will be added to the file when we flush the
// cache to disk later.
void *pager_get_page(Pager *pager, uint32_t page_num) {
  log_debug("getting page %d...", page_num);

  if (page_num > PAGER_MAX_PAGES) {
    log_error("page number %d out of bounds, max: %d", page_num,
              PAGER_MAX_PAGES);
    exit(EXIT_FAILURE);
  }

  if (pager->pages[page_num] == NULL) {
    // Cache miss. Allocate memory and load from file.
    log_debug("page %d not found...", page_num);
    log_debug("allocating memory for page %d...", page_num);

    void *page = malloc(PAGER_PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGER_PAGE_SIZE;

    // We might save a partial page at the end of the file
    if (pager->file_length % PAGER_PAGE_SIZE) {
      log_debug("partial page found...");
      num_pages += 1;
    }

    if (page_num <= num_pages) {
      log_debug("reading page %d from file...", page_num);

      lseek(pager->file_descriptor, page_num * PAGER_PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGER_PAGE_SIZE);

      if (bytes_read == -1) {
        log_error("error reading file: %m");
        exit(EXIT_FAILURE);
      }
    }

    log_debug("page %d loaded...", page_num);
    pager->pages[page_num] = page;
  }

  log_debug("page %d found...", page_num);

  return pager->pages[page_num];
}

// The length of the file encodes how many rows are in the database, so we need
// to write a partial page at the end of the file, therefore pager_flush()
// takes both a page number and a size.
void pager_flush(Pager *pager, uint32_t page_num, uint32_t size) {
  log_debug("flushing page %d...", page_num);
  if (pager->pages[page_num] == NULL) {
    log_error("tried to flush null page");
    exit(EXIT_FAILURE);
  }

  log_debug("flushed page %d", page_num);

  log_debug("seeking to page %d...", page_num);
  off_t offset =
      lseek(pager->file_descriptor, page_num * PAGER_PAGE_SIZE, SEEK_SET);
  if (offset == -1) {
    log_error("error seeking: %m");
    exit(EXIT_FAILURE);
  }

  log_debug("writing page %d...", page_num);
  ssize_t bytes_written =
      write(pager->file_descriptor, pager->pages[page_num], size);

  if (bytes_written == -1) {
    log_error("error writing page: %m");
    exit(EXIT_FAILURE);
  }

  log_debug("written page %d", page_num);
}
