#!/bin/sh
# LaTeX makefile
FILE=proj5
all:$(FILE)

.PHONY:clean
clean:
	\rm *.aux *.blg *.out *.bbl *.log *.ps *.dvi *.pdf
$(FILE):$(FILE).tex
	latex $(FILE)
	bibtex $(FILE)
	latex $(FILE)
	latex $(FILE)
	dvips $(FILE)
	ps2pdf $(FILE).ps $(FILE).pdf

