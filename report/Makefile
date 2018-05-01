
SRC= abstract.tex \
     motivation.tex \
     logics.tex \
     rworks.tex \
     fragment-simple.tex \
		 fragment-card.tex \
		 fragment-order.tex \
		 fragment-spen.tex \
		 fragment-impl.tex \
     conclusion.tex \
		 acknow.tex \
		 appendix.tex \
     dp-mset.bib

all: main.pdf

main.bbl: main.tex dp-mset.bib
	pdflatex -file-line-error main.tex
	bibtex main.aux

main.pdf: $(SRC) main.bbl
	pdflatex -file-line-error main.tex

paper.pdf: paper.tex  
	pdflatex paper.tex
	bibtex paper.aux
	pdflatex paper.tex

clean:
	rm *.syn* *.aux *.log *~ *.bbl *.bst *.out
