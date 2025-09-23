#!/bin/bash

FILE="principal"

pdflatex -interaction=nonstopmode "$FILE.tex"
bibtex "$FILE"
pdflatex -interaction=nonstopmode "$FILE.tex"
pdflatex -interaction=nonstopmode "$FILE.tex"

echo "COMPIALDODOPDODOODOODODDODOD"

