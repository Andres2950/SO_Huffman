#!/bin/bash

FILE="principal"

bibtex "$FILE"
pdflatex -interaction=nonstopmode "$FILE.tex"

echo "COMPIALDODOPDODOODOODODDODOD"
