
data <- read.csv("timer.csv")
data <- data[data$PARAM1 > 1,]

pdf("timer.pdf")

plot (data$PARAM1, data$TIME, log="xy")

for (size in 2:10) {
    d <- data[data$PARAM0 == size,]
    lines(d$PARAM1, d$TIME)    
}

dev.off()
