getDataCubeFromSample <- function (dataDirectory = "Sample_GeomediaDB",
                                   mediaSelection = NULL,
                                   spaceSelection = NULL,
                                   firstTimeSelection = NULL,
                                   lastTimeSelection = NULL) {


    ## BUILD MEDIA DIMENSION (LIST OF MEDIA)
    mediaList <- c()

    if (is.null(mediaSelection)) {
        allFiles <- list.files(dataDirectory, full.names=TRUE)
        mediaList <- basename(allFiles[file.info(allFiles)$isdir])
    } else {
        for (media in mediaSelection) {
            if (dir.exists(file.path(dataDirectory,media)))
            { mediaList <- c(mediaList, media) }
        }
    }


    ## BUILD TIME DIMENSION (LIST OF DAYS)
    firstTime <- NA
    lastTime <- NA

    if (is.null(firstTimeSelection) || is.null(lastTimeSelection)) {
        for (media in mediaList) {
            data <- read.csv(file.path(dataDirectory,media,"rss_unique_tagged.csv"), sep="\t", stringsAsFactors=FALSE, encoding = "UTF-8")
            times <- unique(substring(data$time,1,10))
            firstTime <- min(times,firstTime,na.rm=TRUE)
            lastTime <- max(times,lastTime,na.rm=TRUE)
        }
    } else {
        regexp <- "[[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2}"
        if (is.character(firstTimeSelection) && nchar(firstTimeSelection) == 10 && grep(regexp,firstTimeSelection) == 1) { firstTime <- firstTimeSelection }
        if (is.character(lastTimeSelection) && nchar(lastTimeSelection) == 10 && grep(regexp,lastTimeSelection) == 1) { lastTime <- lastTimeSelection }
    }

    timeList <- c()
    if (!is.na(firstTime) && !is.na(lastTime))
    { timeList <- as.character(seq(as.Date(firstTime), as.Date(lastTime), "day")) }


    ## BUILD SPACE DIMENSION (LIST OF COUNTRIES)
    spaceList <- c()

    if (is.null(spaceSelection)) {
        data <- read.csv(file.path(dataDirectory,"Dico_Country_Free.csv"), sep="\t", stringsAsFactors=FALSE, encoding = "UTF-8")
        spaceList <- unique(data$TAG_country)
    } else {
        data <- read.csv(file.path(dataDirectory,"Dico_Country_Free.csv"), sep="\t", stringsAsFactors=FALSE, encoding = "UTF-8")
        countries <- unique(data$TAG_country)
        for (space in spaceSelection) {
            if (space %in% countries) { spaceList <- c(spaceList,space) }
        }
    }

    if (!"NOC" %in% spaceList) { spaceList <- c(spaceList,"NOC") }


    ## ORDER DIMENSIONS
    mediaList <- mediaList[order(mediaList)]
    timeList <- timeList[order(timeList)]
    spaceList <- spaceList[order(spaceList)]


    ## BUILD DATA CUBE

    ## Initialization: build empty data cube
    dataCube <- array(0, dim = c(length(mediaList), length(timeList), length(spaceList)),
                      dimnames = list("media"=mediaList, "time"=timeList, "space"=spaceList))

    ## For each media
    for (media in mediaList) {
        ## Initialization: retrieve data
        data <- read.csv(file.path(dataDirectory,media,"rss_unique_tagged.csv"), sep = "\t", stringsAsFactors=FALSE, encoding = "UTF-8")

        ## Format data and count number of tags per item
        data$day <- substr(data$time,1,10)
        data <- data[data$day %in% timeList,]
        data[!data$TAG_country %in% spaceList,"TAG_country"] <- "NOC"
        data$COUNT <- ifelse(data$TAG_country == "NOC", 0, 1)
        counts <- aggregate(data$COUNT, by=list(data$ID,data$day), FUN=sum)
        colnames(counts) <- c("ID","time","value")

        ## For each detected tag
        for (r in 1:nrow(data)) {
            line <- data[r,]
            id <- line$ID
            time <- line$day
            space <- line$TAG_country
            count <- counts[counts$ID == id,"value"]

            ## Increment value in data cube
            if (space != "NOC" && count != 0) {
                dataCube[media,time,space] <- dataCube[media,time,space] + 1/count
            }
        }

        ## Count items with no detected tag
        NOCCounts <- aggregate(counts$value, by=list(counts$ID,counts$time), FUN=sum)
        colnames(NOCCounts) <- c("ID","time","value")
        NOCCounts <- NOCCounts[NOCCounts$value == 0,]
        NOCCounts$value <- 1
        NOCCounts <- aggregate(NOCCounts$value, by=list(NOCCounts$time), FUN=sum)
        colnames(NOCCounts) <- c("time","value")

        ## Set value of "NOC" (No Country) in data cube
        for (r in 1:nrow(NOCCounts)) {
            line <- NOCCounts[r,]
            dataCube[media,line$time,"NOC"] <- line$value
        }

    }


    return (dataCube)
}



#' Test title
#'
#' @param cubeFile Test param
#' @return Test return
#' @examples
#' Test
#' example
printHeatmaps <- function (cubeFile, partitionFile, log=TRUE, mediaOrder=NULL) {
    
    cube <- readDataCube(paste(cubeFile,"csv",sep="."))

    media <- dimnames(cube$data)$media
    if (!is.null(mediaOrder)) { media <- mediaOrder }

    time <- dimnames(cube$data)$time
    space <- dimnames(cube$data)$space

    inputData <- read.csv (paste(partitionFile,"csv",sep="."), sep = ";")


    lmin <- 0
    lmax <- 0
    for (scale in unique(inputData$SCALE)) {

        data <- inputData[inputData$SCALE == scale,]
        partNb <- 1
        for (i in seq(1,nrow(data))) {
            row <- data[i,]
            sign <- 0
            if (row$DATA > row$MODEL) {            
                if (log) {
                    sign <- - (ppois (row$DATA, row$MODEL, lower.tail = FALSE, log.p=TRUE) + log(2))
                } else {
                    sign <- 1 - 2*ppois (row$DATA, row$MODEL, lower.tail = FALSE)
                }
            }

            if (row$DATA < row$MODEL) {
                if (log) {
                    sign <- ppois (row$DATA, row$MODEL, lower.tail = TRUE, log.p=TRUE) + log(2)
                } else {
                    sign <- -(1 - 2*ppois (row$DATA, row$MODEL, lower.tail = TRUE)) 
                }
            }
            lmin <- min(sign,lmin,na.rm=TRUE)
            lmax <- max(sign,lmax,na.rm=TRUE)
        }
    }


    num <- 0
    for (scale in unique(inputData$SCALE)) {
        
        data <- inputData[inputData$SCALE == scale,]
        informationLoss <- sum(data$INFORMATION_LOSS)
        sizeReduction <- sum(data$SIZE_REDUCTION)
        size <- nrow(data)
        
        
        microData <- merge(data.frame(media), data.frame(time))
        microData$value <- 0
        microData$sign <- 0
        microData$partNb <- 0
        
        microData$up <- FALSE
        microData$down <- FALSE
        microData$left <- FALSE
        microData$right <- FALSE
        
        microData$x <- match(microData$time,time)
        microData$y <- match(microData$media,media)
        
        partNb <- 1
        for (i in seq(1,nrow(data))) {
            row <- data[i,]

            strmedia <- as.character(row$DIM_1)
            strmedia <- substr(strmedia, 2, nchar(strmedia)-1)
            pmedia <- strsplit(strmedia,', ')[[1]]

            strtime <- as.character(row$DIM_2)
            strtime <- substr(strtime, 2, nchar(strtime)-1)
            ptime <- strsplit(strtime,', ')[[1]]
            if (length(ptime) > 1) { ptime <- time[time >= ptime[1] & time <= ptime[2]] }

            sign <- 0
            if (row$DATA > row$MODEL) {            
                if (log) {
                    sign <- - (ppois (row$DATA, row$MODEL, lower.tail = FALSE, log.p=TRUE) + log(2))
                } else {
                    sign <- 1 - 2*ppois (row$DATA, row$MODEL, lower.tail = FALSE)
                }
            }

            if (row$DATA < row$MODEL) {
                if (log) {
                    sign <- ppois (row$DATA, row$MODEL, lower.tail = TRUE, log.p=TRUE) + log(2)
                } else {
                    sign <- -(1 - 2*ppois (row$DATA, row$MODEL, lower.tail = TRUE)) 
                }
            }
            
            for (m in pmedia) {
                for (t in ptime) {
                    microData[microData$media == m & microData$time == t,"value"] <- row$DATA
                    microData[microData$media == m & microData$time == t,"sign"] <- sign
                    microData[microData$media == m & microData$time == t,"partNb"] <- partNb
                }
            }
            partNb <- partNb+1
        }

        for (i in seq(1,nrow(microData))) {
            row <- microData[i,]
            
            if (row$x == 1) { microData[i,"left"] <- TRUE } else {
                                                              if (microData[microData$x == row$x - 1 & microData$y == row$y,"partNb"] != row$partNb) { microData[i,"left"] <- TRUE }
                                                          }
            
            if (row$x == length(time)) { microData[i,"right"] <- TRUE } else {
                                                                          if (microData[microData$x == row$x + 1 & microData$y == row$y,"partNb"] != row$partNb) { microData[i,"right"] <- TRUE }
                                                                      }
            
            if (row$y == 1) { microData[i,"up"] <- TRUE } else {
                                                            if (microData[microData$x == row$x & microData$y == row$y - 1,"partNb"] != row$partNb) { microData[i,"up"] <- TRUE }
                                                        }
            
            if (row$y == length(media)) { microData[i,"down"] <- TRUE } else {
                                                                          if (microData[microData$x == row$x & microData$y == row$y + 1,"partNb"] != row$partNb) { microData[i,"down"] <- TRUE }
                                                                      }    
        }
        
        microData$label <- microData$partNb
        
        for (i in seq(1,nrow(microData))) {
            row <- microData[i,]
            
            if (row$x != 1 && microData[microData$x == row$x - 1 & microData$y == row$y,"partNb"] == row$partNb) { microData[i,"label"] <- "" }
            
            if (row$y != 1 && microData[microData$x == row$x & microData$y == row$y - 1,"partNb"] == row$partNb) { microData[i,"label"] <- "" }
        }
        
        
        x <- c()
        y <- c()
        xe <- c()
        ye <- c()
        
        for (i in seq(1,nrow(microData))) {  
            row <- microData[i,]
            
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
        
        segments <- data.frame(x,y,xe,ye)

        microData$media <- factor(microData$media, levels = as.list(media))
        
        l <- max(-min(microData$sign), max(microData$sign))
        l <- max(-lmin,lmax)

        g <- ggplot (microData, aes (y = media, x = time))
        g <- g + geom_tile (aes (fill = sign))
        g <- g + scale_fill_gradient2 (low = "blue", mid="white", high = "red", name="Signif.", limits=c(-l,l))
        g <- g + ggtitle (paste("",sep=""))
        g <- g + theme (axis.text.x = element_text(angle = 90, hjust = 1))
        g <- g + geom_segment(data=segments,aes(x=x,y=y,xend=xe,yend=ye))
        g <- g + geom_text(aes(label = label), size=3)
        ##g

        ggsave(file=paste(partitionFile,"_n=",num,"_s=",scale,"_size=",size,"_loss=",informationLoss,".png",sep=""), width=9, height=6)

        num <- num+1
    }
}





filterDataCubeByOrder <- function (dataCube,
                                   mediaNumber = NULL,
                                   timeNumber = NULL,
                                   spaceNumber = NULL)
{
    mediaSelection <- dimnames(dataCube)$media
    timeList <- dimnames(dataCube)$time
    spaceSelection <- dimnames(dataCube)$space

    if (!is.null(mediaNumber)) {
        mediaSelection <- apply(dataCube,c(1),sum)
        mediaSelection <- mediaSelection[order(-mediaSelection)]
        mediaSelection <- names(mediaSelection[1:mediaNumber])
    }

    if (!is.null(timeNumber)) {
        timeList <- apply(dataCube,c(2),sum)
        timeList <- timeList[order(-timeList)]
        timeList <- names(timeList[1:timeNumber])
    }

    if (!is.null(spaceNumber)) {
        spaceSelection <- apply(dataCube,c(3),sum)
        spaceSelection <- spaceSelection[order(-spaceSelection)]
        spaceSelection <- names(spaceSelection[1:spaceNumber])
    }

    return (dataCube[mediaSelection,timeList,spaceSelection])
}



filterDataModels <- function (dataModels,
                              mediaSelection = NULL,
                              firstTimeSelection = NULL, lastTimeSelection = NULL,
                              spaceSelection = NULL)
{
    for (model in names(dataModels)) {
        dataModels[[model]] <- filterDataCube (dataModels[[model]], mediaSelection, firstTimeSelection, lastTimeSelection, spaceSelection)
    }
    return (dataModels)
}


filterDataModelsFromDataCube <- function (dataModels, dataCube)
{
    return (filterDataModels (dataModels, dimnames(dataCube)$media, head(dimnames(dataCube)$time,1), tail(dimnames(dataCube)$time,1), dimnames(dataCube)$space))
}



filterDataModelsByOrder <- function (dataModels,
                                     mediaNumber = NULL,
                                     timeNumber = NULL,
                                     spaceNumber = NULL)
{
    for (model in names(dataModels)) {
        dataModels[[model]] <- filterDataModelsByOrder (dataModels[[model]], mediaNumber, timeNumber, spaceNumber)
    }
    return (dataModels)
}


cutDataCube <- function (dataCube,
                         media=NULL,
                         time=NULL,
                         space=NULL)
{
    return (filterDataCube (dataCube,
                            mediaSelection = c(media),
                            firstTimeSelection = time, lastTimeSelection = time,
                            spaceSelection = c(space)))
}


cutDataModels <- function (dataModels,
                           media=NULL,
                           time=NULL,
                           space=NULL)
{
    for (model in names(dataModels)) {
        dataModels[[model]] <- cutDataCube (dataModels[[model]], media, time, space)
    }
    return (dataModels)
}







readDataCube <- function (fileName) {
    d <- scan (file=fileName, sep=";", nlines=1)
    names <- scan (file=fileName, what="character", sep=";", skip=1, nlines=1)
    dim <- scan (file=fileName, sep=";", skip=2, nlines=1)
    models <- scan (file=fileName, sep=";", what="character", skip=length(dim)+3, nlines=1)

    dimnames <- list()
    for (i in 1:length(dim)) {
        dimnames[[names[i]]] <- scan (file=fileName, what="character",
                                      sep=";", skip=i+2, nlines=1, na.strings="NULL")
    }

    n <- 1
    dataCube <- list()
    for (model in models) {
        d <- scan (file=fileName, sep=";", skip=length(dim)+3+n, nlines=1)
        dataCube[[model]] <- array(d,dim,dimnames)
        n <- n+1
    }
    
    return (dataCube)
}



readDataModels <- function (fileName) {
    dataModels <- list()
    name <- substr(fileName,1,nchar(fileName)-4)
    extension <- substr(fileName,nchar(fileName)-3,nchar(fileName))
    for (model in c("MS","MS+MT","MS+MT+ST","MS+ST","MT","MT+ST","ST")) {
        dataModels[[model]] <- readDataCube (fileName=paste(name,"_",model,extension,sep=""))
    }
    return (dataModels)
}



writeDataCube <- function (dataCube, dataModels = NULL, fileName) {   
    write.table (t(length(dim(dataCube))), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE)
    write.table (t(names(dimnames(dataCube))), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    write.table (t(dim(dataCube)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    lapply (dimnames(dataCube), function(x) write.table(t(x), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE))
    write.table (t(c("data",names(dataModels))), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)    
    write.table (t(as.vector(dataCube)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    lapply (dataModels, function(x) write.table(t(as.vector(x)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE))
}


writeDataModels <- function (dataModels, fileName) {
    name <- substr(fileName,1,nchar(fileName)-4)
    extension <- substr(fileName,nchar(fileName)-3,nchar(fileName))
    for (model in names(dataModels)) {
        writeDataCube (dataModels[[model]], fileName=paste(name,"_",model,extension,sep=""))
    }
}

writeDataCubeAsMatrix <- function (dataCube, dataModels, fileName) {   
    write.table (t(c("FLOWS","DATES","COUNTRIES","OBSERVED","MODEL_ASTI","MODEL_ASM","MODEL_ATM","EBOLA_OBSERVED","EBOLA_MODEL_ASTI","EBOLA_MODEL_ASM","EBOLA_MODEL_ATM")), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE)
    lapply (dimnames(dataCube), function(x) write.table(t(x), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE))
    write.table (t(as.vector(dataCube)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    for (model in c("ST","MS","MT")) {
        write.table (t(as.vector(dataModels[[model]])), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    }

    # THIS IS FAKE!
    write.table (t(as.vector(dataCube)/10), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    for (model in c("ST","MS","MT")) {
        write.table (t(as.vector(dataModels[[model]])/10), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    }
}


writeDataCubeAsFrame <- function (dataCube, dataModels, fileName) {
    media <- dimnames(dataCube)[[1]]
    time <- dimnames(dataCube)[[2]]
    space <- dimnames(dataCube)[[3]]

    frame <- data.frame(
        FLOWS = rep(media, time=length(time)*length(space)),
        DATES = rep(time, time=length(space), each=length(media)),
        COUNTRIES = rep(space, each=length(media)*length(time)),
        stringsAsFactors = FALSE
    )

    frame$OBSERVED <- as.vector(dataCube)
    frame$MODEL_ASTI <- as.vector(dataModels[["ST"]])
    frame$MODEL_ASM <- as.vector(dataModels[["MS"]])
    frame$MODEL_ATM <- as.vector(dataModels[["MT"]])

    # THIS IS FAKE!
    frame$EBOLA_OBSERVED <- as.vector(dataCube)/10
    frame$EBOLA_MODEL_ASTI <- as.vector(dataModels[["ST"]])/10
    frame$EBOLA_MODEL_ASM <- as.vector(dataModels[["MS"]])/10
    frame$EBOLA_MODEL_ATM <- as.vector(dataModels[["MT"]])/10
    
    write.csv(frame, file=fileName, row.names = FALSE)
}

getDataframeCube <- function (dataCube, dataModels) {
    
    media <- dimnames(dataCube)[[1]]
    time <- dimnames(dataCube)[[2]]
    space <- dimnames(dataCube)[[3]]

    dataframeCube <- data.frame(
        media = rep(media, time=length(time)*length(space)),
        time = rep(time, time=length(space), each=length(media)),
        space = rep(space, each=length(media)*length(time)),
        stringsAsFactors = FALSE
    )

    dataframeCube$observed <- as.vector(dataCube)
    dataframeCube$MS <- as.vector(dataModels[["MS"]])
    dataframeCube$MT <- as.vector(dataModels[["MT"]])
    dataframeCube$ST <- as.vector(dataModels[["ST"]])
    dataframeCube$MS_MT <- as.vector(dataModels[["MS+MT"]])
    dataframeCube$MS_ST<- as.vector(dataModels[["MS+ST"]])
    dataframeCube$MT_ST <- as.vector(dataModels[["MT+ST"]])
    dataframeCube$MS_MT_ST <- as.vector(dataModels[["MS+MT+ST"]])

    dataframeCube <- dataframeCube[order(dataframeCube$media,dataframeCube$time,dataframeCube$space),]

    return (dataframeCube)
}
