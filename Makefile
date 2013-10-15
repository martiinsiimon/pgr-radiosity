#makefile pro projekt pgr-radiosita

FILES=main.cpp

SRC=src/
DOC=doc/

ARCHIVE=xsimon14.zip
PDFFILE=$(DOC)pgr.pdf

RM=rm -rf
AR=zip


.PHONY: all src doc

all: src doc

src:
	make -C $(SRC)

doc:
	make -C $(DOC)

run:
	make -C $(SRC) run

clean: clean-src clean-doc
	$(RM) $(ARCHIVE)

clean-src:
	make -C $(SRC) clean

clean-doc:
	make -C $(DOC) clean

pack: doc src clean
	$(AR) $(ARCHIVE) Makefile README INSTALL LICENSE $(SRC)$(FILES) $(SRC)Makefile $(PDFFILE)
