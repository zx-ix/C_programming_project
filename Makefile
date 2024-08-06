# Please use this Makefile as provided. You should not make modifications.

BUILDDIR=../outputs
CFLAGS=-Iprovided_functions -I. -lm -ltomcrypt -Lprovided_functions/libtomcrypt -Iprovided_functions/libtomcrypt/src/headers
# -ltomcrypt provides hashing functions
# -lm is math, needed for tomcrypt

all: clean database search transforms_demo test libtomcrypt
clean:
	rm -f $(BUILDDIR)/database $(BUILDDIR)/search $(BUILDDIR)/transforms_demo $(BUILDDIR)/test_stats_mean $(BUILDDIR)/test_stats_variance $(BUILDDIR)/test_transformations
test: test_stats_mean test_stats_variance test_transformations
debug: CFLAGS += -g

# if you run the command: make SILENT=1, the output won't be printed to the console
# (to make evaluate.sh easier to read)
ifdef SILENT
.SILENT:
endif

# task 2
database: libtomcrypt
	mkdir -p $(BUILDDIR)
	gcc database.c -o $(BUILDDIR)/database $(CFLAGS)
# task 3
search: libtomcrypt
	mkdir -p $(BUILDDIR)
	gcc search.c -o $(BUILDDIR)/search $(CFLAGS)

# test programs
test_stats_mean: libtomcrypt
	mkdir -p $(BUILDDIR)
	gcc provided_functions/test_stats_mean.c -o $(BUILDDIR)/test_stats_mean $(CFLAGS)
test_stats_variance: libtomcrypt
	mkdir -p $(BUILDDIR)
	gcc provided_functions/test_stats_variance.c -o $(BUILDDIR)/test_stats_variance $(CFLAGS)
test_transformations: libtomcrypt
	mkdir -p $(BUILDDIR)
	gcc provided_functions/test_transformations.c -o $(BUILDDIR)/test_transformations $(CFLAGS)
transforms_demo: libtomcrypt
	mkdir -p $(BUILDDIR)
	gcc provided_functions/transformations_demo.c -o $(BUILDDIR)/transforms_demo $(CFLAGS)

# libtomcrypt (library for hash functions)
libtomcrypt:
	$(MAKE) -s -C provided_functions/libtomcrypt 
