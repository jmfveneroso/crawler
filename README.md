# Web Crawler

This project implements a simple web crawler for the Information Retrieval course at Universidade Federal de Minas Gerais.
The crawler finds, parses and collects web pages aiming for diversity by doing a "breadth first" link search in the web.
The finished command line tool achieved a performance of up to 150 page downloads per second and averaged about 110 
pages per second when crawled urls were stored in primary memory. Another experiment was conducted
with urls stored in secondary memory, however the performance decreased significantly, achieving only
about a fourth of the former download speed.

---

### Build

To compile this project run the following command inside the root directory:

```
$ make
```

Two binaries will be produced in the build directory: "crawler" and "thread_test".

---

### Usage

```
$ crawler output_file database_file [max_urls]
```

The output file is where the html pages will be stored.

The database file is where the url database is going to be written if the option IN_MEMORY
is not selected.

The parameter max_urls is optional. If is greater than 0, the crawler will only download the specified number of urls.

---

### Configuration

Further configuration can be made by altering the "config.h" file. Instructions are present in the file itself.

---

### Test

To run tests. Compile with:

```
$ make all
```

And run tests by executing:

```
$ make test
```
