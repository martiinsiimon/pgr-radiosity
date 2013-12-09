#makefile pro projekt pgr-radiosita

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

run-cpu:
	make -C $(SRC) run-cpu

run-gpu:
	make -C $(SRC) run-gpu

clean: clean-src clean-doc
	$(RM) $(ARCHIVE)

clean-src:
	make -C $(SRC) clean

clean-doc:
	make -C $(DOC) clean

pack: doc src clean
	$(AR) $(ARCHIVE) Makefile README INSTALL LICENSE $(SRC)* $(SRC)Makefile $(PDFFILE)
