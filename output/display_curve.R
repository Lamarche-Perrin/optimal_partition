
data.b <- read.csv("output.graph.4.best.csv", sep=";")

agg.b <- aggregate(data.b[,c("COMPLEXITY","INFORMATION_LOSS")], by=list(data.b$SCALE), FUN=sum)
colnames(agg.b)[1] <- "SCALE"

agg.b[agg.b$SCALE==-1,"SCALE"] <- Inf
agg.b <- agg.b[order(agg.b$SCALE),]

comp.b <- agg.b$COMPLEXITY
loss.b <- agg.b$INFORMATION_LOSS

##comp.b <- head(agg.b$COMPLEXITY,-1)
##loss.b <- head(agg.b$INFORMATION_LOSS,-1

i <- which(comp.b==9)


data.w <- read.csv("output.graph.5.worst.csv", sep=";")

agg.w <- aggregate(data.w[,c("COMPLEXITY","INFORMATION_LOSS")], by=list(data.w$SCALE), FUN=sum)
colnames(agg.w)[1] <- "SCALE"

agg.w[agg.w$SCALE==-1,"SCALE"] <- Inf
agg.w <- agg.w[order(agg.w$SCALE),]

comp.w <- agg.w$COMPLEXITY
loss.w <- agg.w$INFORMATION_LOSS

##comp.w <- tail(agg.w$COMPLEXITY,-1)
##loss.w <- tail(agg.w$INFORMATION_LOSS,-1)


##pdf("graph_curve.pdf", width=4.5, height=3.5)
png("graph_curve.png", width=1800, height=1400, res=400, bg="transparent")

par(mar=c(3.5,3.5,0.5,0)+0.1)
plot (
    comp.b, loss.b, type="s", cex=2,
    xlab="Complexity (number of super-edges)",
    ylab="Information loss (KL divergence in bits)",
    mgp = c(2.5, 1, 0)
)

lines (comp.w, loss.w, type="s")


points(head(comp.b,1),head(loss.b,1),col="black",cex=1.5)
points(tail(comp.b,1),tail(loss.b,1),col="black",cex=1.5)

points(comp.b[i],loss.b[i],col="red",cex=1.5)
abline(h=loss.b[i],lty=2,col="red",lwd=2)
abline(v=comp.b[i],lty=2,col="red",lwd=2)

##points(comp,loss)

dev.off()
