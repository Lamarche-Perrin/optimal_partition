size <- 8
probs <- c(1,3/4,2/3,1/2)

rates <- seq(0,1,0.001)
col <- 1

pdf ('results.pdf')
plot (0, xlim=c(0,1), ylim=c(-1,0), xlab='Compression rate', ylab='Information criterion')


for (prob in probs) {
    input <- paste ("matrix.size=",size,".prob=",round(prob,2),".csv", sep="")
    results <- read.table (input, sep=';', header=TRUE)

    min.objectives <- rates*0
    for (rate in unique(results$RATE)) {
        loss <- sum (results[results$RATE == rate,'INFORMATION_LOSS'])
        reduc <- sum (results[results$RATE == rate,'SIZE_REDUCTION'])

        objectives <- ((1-rates) * loss - rates * reduc)
        min.objectives <- pmin (objectives, min.objectives)
    }

    lines (rates, min.objectives, lty=1, lwd=2, col=col)
    col <- col+1
}
