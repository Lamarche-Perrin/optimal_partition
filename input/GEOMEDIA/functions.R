



getDataCube <- function (dataDirectory = "Analyse_Geomedia",
                         nameDictionary = "Pays_agenda",
                         selectedList,
                         mediaSelection = NULL,
                         onlyInternationalItems = FALSE) {

    ## BUILD MEDIA DIMENSION (LIST OF MEDIA)

    mediaList <- c()

    fileName <- file.path(dataDirectory,selectedList,"data",paste("X.Global_metadata_",selectedList,".csv",sep=""))
    data <- read.csv(fileName, sep="\t", stringsAsFactors=FALSE, encoding="UTF-8")
    allMedia <- data$Name_Flux
    allMedia <- allMedia[allMedia != ""]

    if (is.null(mediaSelection)) {
        mediaList <- allMedia
    } else {
        for (media in mediaSelection) {
            if (media %in% allMedia)
            { mediaList <- c(mediaList, media) }
        }
    }


    ## COLLECT DATA ##

    allData <- data.frame(media=character(0), time=character(0), space=character(0), value=numeric(0))

    articleNumber <- 0

    for (media in mediaList) {

        ## For each media, open corresponding file ##
        file <- file.path(dataDirectory,selectedList,"data",media,"Results_Tags",nameDictionary,"Pays_agenda_Approx_T1_FULL_RSS_by_TAG.csv")
        data <- read.csv(file, sep="\t", stringsAsFactors=FALSE, encoding="UTF-8")
        data <- data[,c("ID","Flux","time","TAG")]
        
        ## Suppress items with no tag and items with only the national tag
        if (onlyInternationalItems) {
            data <- data[data$TAG != "",]
            country <- substring(media,4,6)
            data$count <- ifelse(data$TAG != country,1,0)
            counts <- aggregate(data$count, by=list(data$ID), FUN=sum)
            data <- data[!data$ID %in% counts[counts$x == 0,1],]
            articleNumber <- articleNumber + length(unique(data$ID))        
        }
        
        ## Compute number of tagged countries for each item ##
        data$count <- 1
        counts <- aggregate(data$count, by=list(data$ID), FUN=sum)
        data <- merge(x=data, y=counts, by.x="ID", by.y="Group.1")
        data$score <- 1/data$x
        data[data$TAG == "","TAG"] <- "NOC"

        ## Aggregate data to the week-level ##
        data$week <- as.Date(substr(data$time,1,10), format="%Y-%m-%d")
        data$week <- data$week - ((as.POSIXlt(data$week)$wday + 6) %% 7)
        result <- aggregate(data$score, by=list(data$Flux,data$week,data$TAG), FUN=sum)

        ## Format and append data ##
        colnames(result) <- c("media","time","space","value")
        allData <- rbind(allData,result)
    }


    ## BUILD DIMENSION LABELS

    allData$media <- as.character(allData$media)
    allData$time <- as.character(allData$time)
    allData$space <- as.character(allData$space)

    media <- unique(allData$media)
    time <- unique(allData$time)
    space <- unique(allData$space)

    media <- media[order(media)]
    time <- time[order(time)]
    space <- space[order(space)]



    ## Build data cube (multidimensional array)

    dataCube <- array(0,
                      dim = c(length(media), length(time), length(space)),
                      dimnames = list("media"=media, "time"=time, "space"=space)
                      )

    for (r in 1:nrow(allData)) {
        d <- allData[r,]
        dataCube[d$media,d$time,d$space] = d$value
    }

    
    return (dataCube)
}



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
        spaceList <- unique(data$TAG)
    } else {
        data <- read.csv(file.path(dataDirectory,"Dico_Country_Free.csv"), sep="\t", stringsAsFactors=FALSE, encoding = "UTF-8")
        countries <- unique(data$TAG)
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
        data[!data$TAG %in% spaceList,"TAG"] <- "NOC"
        data$COUNT <- ifelse(data$TAG == "NOC", 0, 1)
        counts <- aggregate(data$COUNT, by=list(data$ID,data$day), FUN=sum)
        colnames(counts) <- c("ID","time","value")

        ## For each detected tag
        for (r in 1:nrow(data)) {
            line <- data[r,]
            id <- line$ID
            time <- line$day
            space <- line$TAG
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



filterDataCube <- function (dataCube,
                            mediaList = NULL,
                            timeFirst = NULL, timeLast = NULL,
                            spaceList = NULL)
{
    if (is.null(mediaList)) { mediaList <- dimnames(dataCube)$media }
    if (is.null(spaceList)) { spaceList <- dimnames(dataCube)$space }

    timeList <- dimnames(dataCube)$time
    if (!is.null(timeFirst) && !is.null(timeLast)) { timeList <- timeList[timeList >= timeFirst & timeList <= timeLast] }
    
    return (dataCube[mediaList,timeList,spaceList])
}


filterDataCubeByOrder <- function (dataCube,
                                   mediaNumber = NULL,
                                   timeNumber = NULL,
                                   spaceNumber = NULL)
{
    mediaList <- dimnames(dataCube)$media
    timeList <- dimnames(dataCube)$time
    spaceList <- dimnames(dataCube)$space

    if (!is.null(mediaNumber)) {
        mediaList <- apply(dataCube,c(1),sum)
        mediaList <- mediaList[order(-mediaList)]
        mediaList <- names(mediaList[1:mediaNumber])
    }

    if (!is.null(timeNumber)) {
        timeList <- apply(dataCube,c(2),sum)
        timeList <- timeList[order(-timeList)]
        timeList <- names(timeList[1:timeNumber])
    }

    if (!is.null(spaceNumber)) {
        spaceList <- apply(dataCube,c(3),sum)
        spaceList <- spaceList[order(-spaceList)]
        spaceList <- names(spaceList[1:spaceNumber])
    }

    return (dataCube[mediaList,timeList,spaceList])
}



filterDataModels <- function (dataModels,
                              mediaList = NULL,
                              timeFirst = NULL, timeLast = NULL,
                              spaceList = NULL)
{
    for (model in names(dataModels)) {
        dataModels[[model]] <- filterDataCube (dataModels[[model]], mediaList, timeFirst, timeLast, spaceList)
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


getDataModels <- function (dataCube) {

    media <- dimnames(dataCube)[[1]]
    time <- dimnames(dataCube)[[2]]
    space <- dimnames(dataCube)[[3]]


    ## COMPUTE MARGINAL VALUES

    v <- sum(dataCube)
    M <- apply(dataCube,c(1),sum)
    T <- apply(dataCube,c(2),sum)
    S <- apply(dataCube,c(3),sum)
    MT <- apply(dataCube,c(1,2),sum)
    MS <- apply(dataCube,c(1,3),sum)
    TS <- apply(dataCube,c(2,3),sum)


    ## INITIALISE MODELS

    dataModels <- list()
    dataModels[["MS+MT+ST"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                      dimnames = list("media"=media, "time"=time,"space"=space))
    dataModels[["MS+MT"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                   dimnames = list("media"=media, "time"=time,"space"=space))
    dataModels[["MT+ST"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                   dimnames = list("media"=media, "time"=time,"space"=space))
    dataModels[["MS+ST"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                   dimnames = list("media"=media, "time"=time,"space"=space))
    dataModels[["ST"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                dimnames = list("media"=media, "time"=time,"space"=space))
    dataModels[["MS"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                dimnames = list("media"=media, "time"=time,"space"=space))
    dataModels[["MT"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                dimnames = list("media"=media, "time"=time,"space"=space))


    ## COMPUTE MODELS

    for (vm in media) {
        for (vt in time) {
            for (vs in space) {
                dataModels[["MS+MT+ST"]][vm,vt,vs] <- (M[vm] * T[vt] * S[vs]) / (v * v)
                dataModels[["MS+MT"]][vm,vt,vs] <- (M[vm] * TS[vt,vs]) / v
                dataModels[["MT+ST"]][vm,vt,vs] <- (T[vt] * MS[vm,vs]) / v
                dataModels[["MS+ST"]][vm,vt,vs] <- (S[vs] * MT[vm,vt]) / v
                dataModels[["ST"]][vm,vt,vs] <- (MT[vm,vt] * MS[vm,vs]) / M[vm]
                dataModels[["MS"]][vm,vt,vs] <- (MT[vm,vt] * TS[vt,vs]) / T[vt]
                dataModels[["MT"]][vm,vt,vs] <- (MS[vm,vs] * TS[vt,vs]) / S[vs]
            }
        }
    }

    return (dataModels)
}





readDataCube <- function (fileName) {
    dim <- scan (file=fileName, sep=";", nlines=1)
    d <- scan (file=fileName, sep=";", skip=length(dim)+1, nlines=1)
    dimnames <- list()
    for (i in 1:length(dim)) {
        dimnames[[i]] <- scan (file=fileName, what="character",
                               sep=";", skip=i, nlines=1, na.strings="NULL")
    }
    
    return (array(d,dim,dimnames))
}



writeDataCube <- function (dataCube, fileName) {   
    write.table (t(dim(dataCube)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE)
    lapply (dimnames(dataCube), function(x) write.table(t(x), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE))
    write.table (t(as.vector(dataCube)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
}


writeDataModels <- function (dataModels, fileName) {
    name <- substr(fileName,1,nchar(fileName)-4)
    extension <- substr(fileName,nchar(fileName)-3,nchar(fileName))
    for (model in names(dataModels)) {
        writeDataCube (dataModels[[model]], fileName=paste(name,"_",model,extension,sep=""))
    }
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
