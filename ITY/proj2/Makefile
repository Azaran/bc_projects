#!/bin/sh
# LaTeX makefile
FILE=proj2
all:$(FILE).pdf

.PHONY:clean
clean:
	\rm *.aux *.blg *.out *.bbl *.log
$(FILE).pdf:$(FILE).tex
	pdflatex $(FILE)
	pdflatex $(FILE)
