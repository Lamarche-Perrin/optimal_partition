#!/usr/bin/env Rscript
library("methods")
library("roxygen2")
package.skeleton (name="functions", code_files="functions.R", force=TRUE)
roxygenize (package.dir="functions", clean=TRUE)
