library(ggplot2)

plot_objective <- function (input) {
    output <- substr (input, 1, nchar(input)-4)
    data <- read.table (input, sep=';', header=TRUE)

    rates <- unique(data$RATE)
    objectives <- c()
    losses <- c()
    reducs <- c()

    for (rate in rates) {
        loss <- sum (data[data$RATE == rate,'INFORMATION_LOSS'])
        reduc <- sum (data[data$RATE == rate,'SIZE_REDUCTION'])

        losses <- append (losses, loss)
        reducs <- append (reducs, reduc)
        objectives <- append (objectives, ((1-rate) * loss - rate * reduc))
    }

    losses <- append(losses,1)
    reducs <- append(reducs,1)
    rates <- append(rates,1)
    objectives <- append(objectives,-1)

    ggplot (data.frame(rates,objectives,losses,reducs), aes(x=rates)) + labs(x='Compression rate', y='Objective functions') + geom_step (aes(y = losses, colour = "Information loss")) + geom_step (aes(y = reducs, colour = "Size reduction")) #+ geom_line(aes(y=objectives))
    
    ggsave(file=paste(output,'.objectives.png',sep=""),width=9, height=6)
}


plot_solutions <- function (input) {
    output <- substr (input, 1, nchar(input)-4)
    data <- read.csv (input, sep=';', header=TRUE)

    size <- sqrt(nrow(data[data$RATE==0,]))
    dim1 <- 0:(size-1)
    dim2 <- 0:(size-1)

    data$SIZE <- unlist(lapply(strsplit(as.character(data$AGG1),','), FUN=length)) * unlist(lapply(strsplit(as.character(data$AGG2),','), FUN=length))
    data$VALUE <- data$VALUE / data$SIZE
    max <- max (data$VALUE)
    
    for (rate in unique (data$RATE)) {    

        subdata <- data[data$RATE == rate,]
        info <- round((1 - sum(subdata$INFORMATION_LOSS))*100)
        
        microdata <- merge (data.frame(dim1), data.frame(dim2))
        microdata$value <- 0
        microdata$partNb <- 0
        
        microdata$up <- FALSE
        microdata$down <- FALSE
        microdata$left <- FALSE
        microdata$right <- FALSE
        
        microdata$x <- match(microdata$dim2,dim2)
        microdata$y <- match(microdata$dim1,dim1)
        ## microdata$x <- microdata$dim1
        ## microdata$y <- microdata$dim2
        
        partNb <- 1
        for (i in seq(1,nrow(subdata))) {
            row <- subdata[i,]
            pdim1 <- strsplit(as.character(row$AGG1),',')[[1]]
            pdim2 <- strsplit(as.character(row$AGG2),',')[[1]]
            
            for (f in pdim1) {
                for (d in pdim2) {
                    microdata[microdata$dim1 == f & microdata$dim2 == d,"value"] <- row$VALUE
                    microdata[microdata$dim1 == f & microdata$dim2 == d,"partNb"] <- partNb
                }
            }
            partNb <- partNb+1
        }

        for (i in seq(1,nrow(microdata))) {
            row <- microdata[i,]
            
            if (row$x == 1) {
                microdata[i,"left"] <- TRUE
            } else {
                if (microdata[microdata$x == row$x - 1 & microdata$y == row$y,"partNb"] != row$partNb) { microdata[i,"left"] <- TRUE }
            }
            
            if (row$x == length(dim2)) {
                microdata[i,"right"] <- TRUE
            } else {
                if (microdata[microdata$x == row$x + 1 & microdata$y == row$y,"partNb"] != row$partNb) { microdata[i,"right"] <- TRUE }
            }
            
            if (row$y == 1) {
                microdata[i,"up"] <- TRUE
            } else {
                if (microdata[microdata$x == row$x & microdata$y == row$y - 1,"partNb"] != row$partNb) { microdata[i,"up"] <- TRUE }
            }
            
            if (row$y == length(dim1)) {
                microdata[i,"down"] <- TRUE
            } else {
                if (microdata[microdata$x == row$x & microdata$y == row$y + 1,"partNb"] != row$partNb) { microdata[i,"down"] <- TRUE }
            }    
        }
        
        microdata$label <- microdata$partNb
        
        for (i in seq(1,nrow(microdata))) {
            row <- microdata[i,]
            
            if (row$x != 1 && microdata[microdata$x == row$x - 1 & microdata$y == row$y,"partNb"] == row$partNb) { microdata[i,"label"] <- "" }
            
            if (row$y != 1 && microdata[microdata$x == row$x & microdata$y == row$y - 1,"partNb"] == row$partNb) { microdata[i,"label"] <- "" }
        }
        
        
        x <- c()
        y <- c()
        xe <- c()
        ye <- c()
        
        for (i in seq(1,nrow(microdata))) {  
            row <- microdata[i,]
            
            if (row$up) {
                x <- c(x,row$x - 0.5)
                y <- c(y,row$y - 0.5)
                xe <- c(xe,row$x + 0.5)
                ye <- c(ye,row$y - 0.5)
            }

            if (row$down) {
                x <- c(x,row$x - 0.5)
                y <- c(y,row$y + 0.5)
                xe <- c(xe,row$x + 0.5)
                ye <- c(ye,row$y + 0.5)
            }
            
            if (row$left) {
                x <- c(x,row$x - 0.5)
                y <- c(y,row$y + 0.5)
                xe <- c(xe,row$x - 0.5)
                ye <- c(ye,row$y - 0.5)
            }
            
            if (row$right) {
                x <- c(x,row$x + 0.5)
                y <- c(y,row$y + 0.5)
                xe <- c(xe,row$x + 0.5)
                ye <- c(ye,row$y - 0.5)
            }
            
        }

        segments <- data.frame (x=x,y=size+1-y,xe=xe,ye=size+1-ye)

        microdata$dim1 <- factor(microdata$dim1, levels = as.list(dim1))
        
        ggplot (microdata, aes(y = as.factor(dim1),  x = as.factor(dim2))) + scale_y_discrete(limits = rev(as.factor(dim1))) + scale_x_discrete(limits = as.factor(dim2)) + geom_tile (aes(fill = value)) + scale_fill_continuous (low = "white", high = "orange", name="value",limits=c(0,max)) + ggtitle (paste("Compression rate = ", round(rate,5), "     ->     ",info,"% of the original information",sep="")) + theme (axis.text.x = element_text(angle = 90, hjust = 1)) + geom_segment (data=segments, aes(x=x, y=y, xend=xe, yend=ye)) + geom_text (aes(label = label), size=3) + labs (x = 'dim2', y='dim1')

        strrate <- formatC(round(rate,6), format='f', digits=6)
        ggsave(file=paste(output,'.rate=',strrate,".png",sep=""),width=9, height=6)
    }
}
