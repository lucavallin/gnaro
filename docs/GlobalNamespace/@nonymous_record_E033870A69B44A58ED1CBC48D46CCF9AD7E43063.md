# struct 

*Defined at ./include/table.h#28*

 Table is a struct that represents a table. SQLite uses a B-tree for fast lookups, inserts and deletes. We’ll start with something simpler. Like a B-tree, it will group rows into pages, but instead of arranging those pages as a tree it will arrange them as an array.

 Data will be stored as follows: - Store rows in blocks of memory called pages - Each page stores as many rows as it can fit - Rows are serialized into a compact representation with each page - Pages are only allocated as needed - Keep a fixed-size array of pointers to pages



## Members

public uint32_t num_rows

public void *[100] pages



