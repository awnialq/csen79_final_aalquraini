# Sin-Yaw Wang <swang24@scu.edu>
OPT=-g
STD=-std=c++20
CXXFLAGS=$(OPT) $(STD)

%.o:	%.cxx
	$(CXX) -c $(CXXFLAGS) $<

SRCS=record.cxx treedata.cxx treeio.cxx main.cxx 
OBJS=$(SRCS:.cxx=.o)
ALL=decisionTree

all: $(ALL)

decisionTree:	$(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $+

clean:
	/bin/rm -f $(ALL) $(OBJS)
	/bin/rm -rf $(ALL:=.dSYM)

depend: $(SRCS)
	TMP=`mktemp -p .`; export TMP; \
	sed -e '/^# DEPENDENTS/,$$d' Makefile > $$TMP; \
	echo '# DEPENDENTS' >> $$TMP; \
	$(CXX) -MM $+ >> $$TMP; \
	/bin/mv -f $$TMP Makefile

# DEPENDENTS
record.o: record.cxx record.h
treedata.o: treedata.cxx record.h treedata.h
treeio.o: treeio.cxx record.h treedata.h
main.o: main.cxx record.h treedata.h
