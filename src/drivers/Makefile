# SUBDIRS here are expected to be independent of each other
ifeq ($(PLATFORM),tile)
SUBDIRS= mfmt2 mfzrun stub
else
SUBDIRS= mfmc mfmtest mfzrun # ulamtest # mfmdha mfmsim mfmbigtile mfmcity #mfmheadless
endif

.PHONY:	$(SUBDIRS) all clean realclean

all:	$(SUBDIRS)

clean:	$(SUBDIRS)
	@rm -f *~

realclean:	clean $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
