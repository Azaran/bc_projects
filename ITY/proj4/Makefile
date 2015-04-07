#!/bin/sh
# LaTeX makefile
FILE=proj4
all:$(FILE)

.PHONY:clean
clean:
	\rm *.aux *.blg *.out *.bbl *.log *.ps *.dvi
$(FILE):$(FILE).tex
	latex $(FILE)
	bibtex $(FILE)
	latex $(FILE)
	latex $(FILE)
	dvips $(FILE)
	ps2pdf -sPAPERSIZE=a4 $(FILE) $(FILE)
