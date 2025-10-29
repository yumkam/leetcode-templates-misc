OPT?=3
CXXFLAGS?=-O$(OPT) -Wall -W -Wextra
ifneq ($(SANITIZE),no)
CXXFLAGS+=-fsanitize=address -fsanitize=undefined
endif
ifeq ($(CXX),clang)
LOADLIBES+=-lstdc++ -lubsan --rtlib=compiler-rt -lgcc_s
endif
LDFLAGS+=-pthread
STD?=17
PCH=${XDG_RUNTIME_DIR}/pch
CXXFLAGS+=-std=c++$(STD)
CXXFLAGS+=-g$(DEBUG)
CPPFLAGS+=-I /usr/include/nlohmann/
CPPFLAGS+=-I dense_hash_map/include/
CPPFLAGS+=-I sparsehash/src
CPPFLAGS+=-isystem $(PCH) -Winvalid-pch
#CXXFLAGS+=-std=c++2a
CC?=g++
LDFLAGS+=$(CXXFLAGS)
CPPFLAGS+=$(CPPFLAGS_$*)
CXXFLAGS+=$(CXXFLAGS_$*)
CFLAGS+=$(CFLAGS_$*)
LDFLAGS+=$(LDFLAGS_$*)
LOADLIBES+=$(LOADLIBES_$*)
CPPFLAGS_range_comparator-test+=-DRANGE_COMPARATOR_TEST=1
#LOADLIBES+=-lgmpxx -lgmp
default-init-allocator-test: CPPFLAGS += -DDEFAULT_INIT_ALLOCATOR_TEST=1
int128-test: CPPFLAGS += -DINT128_TEST=1
fenwick: fenwick.o
fenwick.o: fenwick.cpp fenwick-tree-2d.hpp fenwick-tree-zero.hpp fenwick-tree.hpp
nstest: nstest.o nstest-foo.o nstest-bar.o
input.txt: %: $(shell xdg-user-dir DOWNLOAD)/%
	mv -i $< $@
%.s: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -S -fverbose-asm -o $@ $<

pch: $(PCH)/bits/stdc++.h.gch/$(shell echo $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS)|md5sum|sed 's/ .*//').gch

$(PCH)/bits/stdc++.h.gch/$(shell echo $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS)|md5sum|sed 's/ .*//').gch: /usr/include/x86_64-linux-gnu/c++/$(shell $(CXX) -dumpversion)/bits/stdc++.h $(PCH)/bits/stdc++.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(filter-out -l%,$(LOADLIBES)) -x c++-header -o $@ $<

$(PCH)/bits/stdc++.h:
	mkdir -p $(PCH)/bits
	echo \#'include_next <bits/stdc++.h>' > $@
