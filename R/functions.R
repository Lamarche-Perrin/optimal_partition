#' functions: A package for computating the notorious bar statistic.
#'
#' The functions package provides three categories of important functions:
#' functions, bar and baz.
#' 
#' @section functions functions:
#' The functions functions ...
#' @title Le package HelloWorld
#' @author Juhui WANG
#' @docType package
#' @name functions-package
NULL

library(ggplot2)
library(stringr)

#' From dataset to data cube
#' 
#' Build a data cube (media x space x time) out of the GEOMEDIA dataset.
#'
#' @param datasetDirectory Location of the GEOMEDIA dataset.
#' @param mediaFile The file in the dataset directory that contains the list of media referenced in the dataset.
#' @param mediaField1 The name of the column containing media identifiers in the media file (see \code{mediaFile} above).
#' @param itemFile The name of the files in the media directories that contains the raw data of the dataset.
#' @param mediaField2 The name of the column containing media identifiers in the item file (see \code{dataFile} above).
#' @param itemField The name of the column containing item identifiers in the item file (see \code{dataFile} above).
#' @param timeField The name of the column containing dates (format: YYYY-MM-DD) in the item file (see \code{dataFile} above).
#' @param spaceField The name of the column containing country identifiers in the item file (see \code{dataFile} above).
#' @param tagField The name of the column indicating the presence of the thematic tag in the item file (see \code{dataFile} above).
#' @param mediaSelection A vector of media identifiers that will be included in the "media" dimension of the data cube. If \code{NULL}, all media in the referenced dataset will be included in the data cube.
#' @param spaceSelection A vector of countries identifiers that will be included in the "space" dimension of the data cube. If \code{NULL}, all countries in the the referenced dataset will be included in the data cube.
#' @param firstTimeSelection Format: "YYYY-MM-DD". Date beginning the data cube "time" dimension (date included). If \code{NULL}, all dates will be included in the data cube.
#' @param lastTimeSelection Format: "YYYY-MM-DD". Date ending the data cube "time" dimension (date included). If \code{NULL}, all dates will be included in the data cube.
#' @param timeLevel Either "day", "week", or "month". For the two latters, the "time" dimension will be preliminary aggregated at the week or at the month level.
#' @param onlyWithGeoTag The data cube will only contain articles that have at least one country tag in the dataset (thus suppressing articles with no associated country).
#' @param onlyInternational The data cube will only contain citations from a media to a country that is different from the media's country (thus suppressing citations from the media's country to this same country).
#' @param onlyWithThemeTag The data cube will only contain articles that have the thematic tag in the dataset (thus suppressing articles that are not associated to the thematic tag).
#' @return The resulting data cube.
#' @examples
#' dataCube <- getDataCubeFromDataset (
#'     datasetDirectory = "../../input/Geomedia_extract_AGENDA",
#'     mediaFile = "X.Global_metadata.csv",
#'     itemFile = "rss_unique_TAG_country_Ebola.csv",
#'     itemField = "ID", mediaField1 = "Name_Flux", mediaField2 = "feed",
#'     timeField = "time", spaceField = "TAG_country", tagField = "TAG_ebola",
#'     mediaSelection = c("fr_FRA_lmonde_int","fr_BEL_lesoir_int","es_ESP_elpais_int"),
#'     spaceSelection = c("FRA","DEU"),
#'     firstTimeSelection = "2014-01-01",
#'     lastTimeSelection = "2014-12-31",
#'     timeLevel = "month",
#'     onlyInternational = TRUE
#' )
#' dimnames(dataCube)
#' print(dataCube)
getDataCubeFromDataset <- function (datasetDirectory,
                                    mediaFile, itemFile,
                                    mediaField1, mediaField2, itemField,
                                    timeField, spaceField, tagField,
                                    mediaSelection = NULL,
                                    spaceSelection = NULL,
                                    firstTimeSelection = NULL,
                                    lastTimeSelection = NULL,
                                    timeLevel = "day",
                                    onlyWithGeoTag = FALSE,
                                    onlyInternational = FALSE,
                                    onlyWithThemeTag = FALSE
                                    ) {

    ## BUILD MEDIA DIMENSION (LIST OF MEDIA)

    mediaList <- c()

    file <- file.path (datasetDirectory, mediaFile)
    data <- read.csv (file, sep="\t", stringsAsFactors=FALSE, encoding="UTF-8")
    allMedia <- data[,mediaField1]
    
    if (is.null (mediaSelection)) {
        mediaList <- allMedia
    } else {
        mediaList <- mediaSelection[mediaSelection %in% allMedia]
    }

    ## COLLECT DATA ##

    allData <- data.frame (media=character(0), time=character(0), space=character(0), value=numeric(0), stringsAsFactors = FALSE)

    for (media in mediaList) {

        ## For each media, open corresponding file
        file <- file.path (datasetDirectory, media, itemFile)
        data <- read.csv (file, sep="\t", stringsAsFactors=FALSE, encoding="UTF-8")
        data <- data[,c(itemField,mediaField2,timeField,spaceField,tagField)]
        data$day <- as.Date (substr(data[,timeField],1,10), format="%Y-%m-%d")

        ## Only keep items within the space and time selection
        if (!is.null (firstTimeSelection))
        { data <- data[data$day >= firstTimeSelection,] }

        if (!is.null (lastTimeSelection))
        { data <- data[data$day <= lastTimeSelection,] }

        if (!is.null (spaceSelection))
        { data <- data[data[,spaceField] %in% spaceSelection,] }

        if (onlyWithGeoTag)
        {
            data <- data[!data[,spaceField] == "",]
        }

        if (onlyWithThemeTag)
        {
            data <- data[data[,tagField] == themeTag,]
        }


        
        ## Suppress items with only the national tag
        if (onlyWithGeoTag) {
            country <- substring(media,4,6)
            data <- data[!data[,spaceField] == country,]
        }

        if (onlyInternational) {
            country <- substring(media,4,6)
            data <- data[!data[,spaceField] %in% c("",country),]
        }
        
        ## Compute number of tagged countries for each item ##
        data$count <- 1
        counts <- aggregate (data$count, by=list(data[,itemField]), FUN=sum)
        data <- merge (x=data, y=counts, by.x=itemField, by.y="Group.1")
        data$score <- 1/data$x
        data[data[,spaceField] == "",spaceField] <- "NOC"

        ## Aggregate data to the week or month level ##
        if (timeLevel == "day") {
            result <- data[,c(mediaField2,"day",spaceField,"score")]
            colnames(result) <- c("media","time","space","value")
        }

        if (timeLevel == "week")
        {
            data$week <- as.POSIXlt(data$day)
            data$week <- data$week - ((data$week$wday + 6) %% 7) * 60*60*24
            data$week <- as.Date (data$week)
            result <- aggregate (data$score, by = list (data[,mediaField2], data$week, data[,spaceField]), FUN=sum)
            colnames(result) <- c("media","time","space","value")
        }

        if (timeLevel == "month")
        {
            data$month <- as.POSIXlt(data$day)
            data$month$mday <- 1
            data$month <- as.Date (data$month)
            result <- aggregate (data$score, by = list (data[,mediaField2], data$month, data[,spaceField]), FUN=sum)
            colnames(result) <- c("media","time","space","value")
        }

        ## Format and append data ##
        allData <- rbind (allData, result)
    }

    allData$time <- as.character (allData$time)

    ## BUILD DIMENSION LABELS
    media <- unique (allData$media)
    time <- unique (allData$time)
    space <- unique (allData$space)

    media <- media[order(media)]
    time <- time[order(time)]
    space <- space[order(space)]    


    ## Build data cube (multidimensional array)

    dataCube <- array(0,
                      dim = c(length(media), length(time), length(space)),
                      dimnames = list("media"=media, "time"=as.character(time), "space"=space)
                      )

    for (r in 1:nrow(allData)) {
        d <- allData[r,]
        dataCube[d$media,d$time,d$space] = d$value
    }

    
    return (dataCube)
}




#' From data cube to data models
#' 
#' Build different data models by using the marginal values of the provided data cube (media x space x time).
#'
#' @param dataCube The data cube out of which the data models are built.
#' @param models A vector of data models to be computed: "ST" for the individual spatio-temporal agenda, "MS" for the global spatial agenda, "MT" for the global temporal agenda, and "ST+MS", "ST+MT", "MS+MT", "ST+MS+MT" for any combination (if NULL, all possible data models are computed).
#' @return The resulting data models.
#' @examples
#' dataModels <- getDataModelsFromDataCube ()
#' print(dataModels)
getDataModelsFromDataCube <- function (dataCube, models = NULL) {

    media <- dimnames(dataCube)$media
    time <- dimnames(dataCube)$time
    space <- dimnames(dataCube)$space

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

    dataModels[["null"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                  dimnames = list("media"=media, "time"=time,"space"=space))

    if (is.null(models) || "ST+MS+MT" %in% models)
    {
        dataModels[["ST+MS+MT"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                          dimnames = list("media"=media, "time"=time,"space"=space))
    }
    
    if (is.null(models) || "MS+MT" %in% models)
    {
        dataModels[["MS+MT"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                       dimnames = list("media"=media, "time"=time,"space"=space))
    }

    if (is.null(models) || "ST+MT" %in% models)
    {
        dataModels[["ST+MT"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                       dimnames = list("media"=media, "time"=time,"space"=space))
    }

    if (is.null(models) || "ST+MS" %in% models)
    {
        dataModels[["ST+MS"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                       dimnames = list("media"=media, "time"=time,"space"=space))
    }

    if (is.null(models) || "ST" %in% models)
    {
        dataModels[["ST"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                    dimnames = list("media"=media, "time"=time,"space"=space))
    }

    if (is.null(models) || "MS" %in% models)
    {
        dataModels[["MS"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                    dimnames = list("media"=media, "time"=time,"space"=space))
    }

    if (is.null(models) || "MT" %in% models)
    {
        dataModels[["MT"]] <- array(NA, dim = c(length(media), length(time), length(space)),
                                    dimnames = list("media"=media, "time"=time,"space"=space))
    }

    ## COMPUTE MODELS

    for (vm in media) {
        for (vt in time) {
            for (vs in space) {
                dataModels[["null"]][vm,vt,vs] <- 1
                
                if (is.null(models) || "ST+MS+MT" %in% models)
                { dataModels[["ST+MS+MT"]][vm,vt,vs] <- (M[vm] * T[vt] * S[vs]) / (v * v) }

                if (is.null(models) || "MS+MT" %in% models)
                { dataModels[["MS+MT"]][vm,vt,vs] <- (M[vm] * TS[vt,vs]) / v }

                if (is.null(models) || "ST+MT" %in% models)
                { dataModels[["ST+MT"]][vm,vt,vs] <- (T[vt] * MS[vm,vs]) / v }

                if (is.null(models) || "ST+MS" %in% models)
                { dataModels[["ST+MS"]][vm,vt,vs] <- (S[vs] * MT[vm,vt]) / v }

                if (is.null(models) || "ST" %in% models)
                { dataModels[["ST"]][vm,vt,vs] <- (MT[vm,vt] * MS[vm,vs]) / M[vm] }

                if (is.null(models) || "MS" %in% models)
                { dataModels[["MS"]][vm,vt,vs] <- (MT[vm,vt] * TS[vt,vs]) / T[vt] }

                if (is.null(models) || "MT" %in% models)
                { dataModels[["MT"]][vm,vt,vs] <- (MS[vm,vs] * TS[vt,vs]) / S[vs] }
            }
        }
    }

    return (dataModels)
}


#' Filtering a data cube
#' 
#' Filtering the planes of a data cube along its three dimensions (media x space x time).
#'
#' @param dataCube The data cube to be filtered.
#' @param mediaSelection A vector of media identifiers that will be kept included in the "media" dimension of the resulting data cube. If \code{NULL}, all media in the original data cube are kept.
#' @param spaceSelection A vector of countries identifiers that will be kept included in the "space" dimension of the resulting data cube. If \code{NULL}, all countries in the original data cube are kept.
#' @param firstTimeSelection Format: "YYYY-MM-DD". Date beginning the resulting data cube "time" dimension (date included). If \code{NULL}, all dates in the original data cube are kept.
#' @param lastTimeSelection Format: "YYYY-MM-DD". Date ending the resulting data cube "time" dimension (date included). If \code{NULL}, all dates in the original data cube are kept.
#' @return The resulting filtered data cube.
#' @examples
#' dataModels <- getDataModelsFromDataCube ()
#' print(dataModels)
filterDataCube <- function (dataCube,
                            mediaSelection = NULL,
                            firstTimeSelection = NULL, lastTimeSelection = NULL,
                            spaceSelection = NULL)
{
    if (is.null(mediaSelection)) { mediaSelection <- dimnames(dataCube)$media }
    if (is.null(spaceSelection)) { spaceSelection <- dimnames(dataCube)$space }

    timeList <- dimnames(dataCube)$time
    if (!is.null(firstTimeSelection) && !is.null(lastTimeSelection)) { timeList <- timeList[timeList >= firstTimeSelection & timeList <= lastTimeSelection] }

    return (dataCube[mediaSelection,timeList,spaceSelection,drop=FALSE])
}



#' Filtering data models
#' 
#' Filtering the planes of data models along their three dimensions (media x space x time).
#'
#' @param dataCube The data models to be filtered.
#' @param mediaSelection A vector of media identifiers that will be kept included in the "media" dimension of the resulting data models. If \code{NULL}, all media in the original data models are kept.
#' @param spaceSelection A vector of countries identifiers that will be kept included in the "space" dimension of the resulting data models. If \code{NULL}, all countries in the original data models are kept.
#' @param firstTimeSelection Format: "YYYY-MM-DD". Date beginning the resulting data models "time" dimension (date included). If \code{NULL}, all dates in the original data models are kept.
#' @param lastTimeSelection Format: "YYYY-MM-DD". Date ending the resulting data models "time" dimension (date included). If \code{NULL}, all dates in the original data models are kept.
#' @return The resulting filtered data models.
#' @examples
#' dataModels <- getDataModelsFromDataCube ()
#' print(dataModels)
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



#' Filtering data models according to a data cube
#' 
#' Filtering the planes of data models according to a data cube that have been previously filtered (see filterDataCube).
#'
#' @param dataModels The data models to be filtered.
#' @param dataCube The data cube to be used to filter the models.
#' @return The resulting filtered data models.
#' @examples
#' dataModels <- getDataModelsFromDataCube ()
#' print(dataModels)
filterDataModelsFromDataCube <- function (dataModels, dataCube)
{
    return (filterDataModels (dataModels, dimnames(dataCube)$media, head(dimnames(dataCube)$time,1), tail(dimnames(dataCube)$time,1), dimnames(dataCube)$space))
}


#' Writing a data cube
#' 
#' Writing a data cube (and its eventual data models) in a CSV file that has the following structure.
#' - Line #1: Number of dimensions (always 3 in the case of a data cube)
#' - Line #2: Name of dimensions (always "media", "time", and "space" in the case of a data cube)
#' - Line #3: List of media identifiers ("media" dimension)
#' - Line #4: List of dates ("time" dimension)
#' - Line #5: List of countries ("space" dimension)
#' - Line #6: List of data models recorded in the next lines. ("data" corresponds to the initial data, with no model.)
#' - Next lines: The data associated to the initial cube or its models, in the order given by the line #6. This data is presented as a list of floating numbers corresponding to the reading of the cube while beginning with the first dimension ("media"), then the second ("time"), and the third ("space").
#'
#' @param dataCube The data cube to be written.
#' @param dataModels The data models to be written. If NULL, only the data in the cube are written.
#' @param fileName The name of the file where the data cube should be written.
#' @examples
#' dataModels <- getDataModelsFromDataCube ()
#' print(dataModels)
writeDataCube <- function (dataCube, dataModels = NULL, fileName) {   
    write.table (t(length(dim(dataCube))), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE)
    write.table (t(names(dimnames(dataCube))), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    write.table (t(dim(dataCube)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    lapply (dimnames(dataCube), function(x) write.table(t(x), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE))
    write.table (t(c("data",names(dataModels))), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)    
    write.table (t(as.vector(dataCube)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE)
    lapply (dataModels, function(x) write.table(t(as.vector(x)), file=fileName, sep=";", quote=FALSE, row.names=FALSE, col.names=FALSE, append=TRUE))
    x <- 0
}

#' Reading a data cube
#'
#' Reading a data cube (and its eventual data models) from a CSV file that has the following structure.
#' - Line #1: Number of dimensions (always 3 in the case of a data cube)
#' - Line #2: Name of dimensions (always "media", "time", and "space" in the case of a data cube)
#' - Line #3: List of media identifiers ("media" dimension)
#' - Line #4: List of dates ("time" dimension)
#' - Line #5: List of countries ("space" dimension)
#' - Line #6: List of data models recorded in the next lines. ("data" corresponds to the initial data, with no model.)
#' - Next lines: The data associated to the initial cube or its models, in the order given by the line #6. This data is presented as a list of floating numbers corresponding to the reading of the cube while beginning with the first dimension ("media"), then the second ("time"), and the third ("space").
#'
#' @param fileName The name of the file from where the data cube should be read.
#' @examples
#' dataModels <- getDataModelsFromDataCube ()
#' print(dataModels)
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



printHeatmaps <- function (partitionFileName,
                           outputName,
                           scales = NULL, firstNum = 0,
                           xdim, ydim, xorder = NULL, yorder = NULL,
                           log = FALSE, dataModel = FALSE,
                           partId = TRUE
                           ) {

    if (xdim == "media") { xfield <- "DIM_1" }
    if (xdim == "time") { xfield <- "DIM_2" }
    if (xdim == "space") { xfield <- "DIM_3" }
    if (xdim == "space1") { xfield <- "DIM_3" }
    if (xdim == "space2") { xfield <- "DIM_4" }

    if (ydim == "media") { yfield <- "DIM_1" }
    if (ydim == "time") { yfield <- "DIM_2" }
    if (ydim == "space") { yfield <- "DIM_3" }
    if (ydim == "space1") { yfield <- "DIM_3" }
    if (ydim == "space2") { yfield <- "DIM_4" }

    partitions <- read.csv (partitionFileName, sep = ";")

    xlist <- c()
    ylist <- c()
    sumValue <- 0

    if (is.null(scales)) { scales <- unique(partitions$SCALE) }
    scale <- unique(partitions$SCALE)[1]
    data <- partitions[partitions$SCALE == scale,]
    for (i in seq(1,nrow(data))) {
        row <- data[i,]

        sumValue <- sumValue + row$DATA
        
        if (xdim == "media" || xdim == "space" || xdim == "space1" || xdim == "space2") {
            strxlist <- as.character(row[,xfield])
            strxlist <- substr(strxlist, 2, nchar(strxlist)-1)
            pxlist <- strsplit(strxlist,', ')[[1]]
        }
        
        if (xdim == "time") {                
            strxlist <- as.character(row[,xfield])
            strxlist <- substr(strxlist, 2, nchar(strxlist)-1)
            pxlist <- strsplit(strxlist,', ')[[1]]
            if (length(pxlist) > 1) { pxlist <- xlist[xlist >= pxlist[1] & xlist <= pxlist[2]] }
        }

        xlist <- c(xlist,pxlist)

        if (ydim == "media" || ydim == "space" || ydim == "space1" || ydim == "space2") {
            strylist <- as.character(row[,yfield])
            strylist <- substr(strylist, 2, nchar(strylist)-1)
            pylist <- strsplit(strylist,', ')[[1]]
        }
        
        if (ydim == "time") {                
            strylist <- as.character(row[,yfield])
            strylist <- substr(strylist, 2, nchar(strylist)-1)
            pylist <- strsplit(strylist,', ')[[1]]
            if (length(pylist) > 1) { pylist <- ylist[ylist >= pylist[1] & ylist <= pylist[2]] }
        }

        ylist <- c(ylist,pylist)
    }

    xlist <- unique(xlist)
    ylist <- unique(ylist)

    if (!is.null(xorder)) { xlist <- xorder }
    if (!is.null(yorder)) { ylist <- yorder }

    lmin <- 0
    lmax <- 0
    for (scale in scales) {

        data <- partitions[partitions$SCALE == scale,]
        partNb <- 1
        for (i in seq(1,nrow(data))) {
            row <- data[i,]

            if (!dataModel) {
                sign <- row$DATA / row$MODEL
            }
            
            if (dataModel) {
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
            }
            
            lmin <- min(sign,lmin,na.rm=TRUE)
            lmax <- max(sign,lmax,na.rm=TRUE)
        }
    }

    maxNum <- length(unique(partitions$SCALE))
    maxNum <- ceiling(log10(maxNum))

    num <- firstNum
    for (scale in scales) {
        
        data <- partitions[partitions$SCALE == scale,]
        informationLoss <- sum(data$INFORMATION_LOSS)
        sizeReduction <- sum(data$SIZE_REDUCTION)
        size <- nrow(data)
                
        microData <- merge(data.frame(xlist), data.frame(ylist))
        microData$value <- 0
        microData$sign <- 0
        microData$partNb <- 0
        
        microData$up <- FALSE
        microData$down <- FALSE
        microData$left <- FALSE
        microData$right <- FALSE
        
        microData$x <- match(microData$xlist,xlist)
        microData$y <- match(microData$ylist,ylist)
        
        partNb <- 1
        for (i in seq(1,nrow(data))) {
            row <- data[i,]

            if (xdim == "media" || xdim == "space" || xdim == "space1" || xdim == "space2") {
                strxlist <- as.character(row[,xfield])
                strxlist <- substr(strxlist, 2, nchar(strxlist)-1)
                pxlist <- strsplit(strxlist,', ')[[1]]
            }
            
            if (xdim == "time") {                
                strxlist <- as.character(row[,xfield])
                strxlist <- substr(strxlist, 2, nchar(strxlist)-1)
                pxlist <- strsplit(strxlist,', ')[[1]]
                if (length(pxlist) > 1) { pxlist <- xlist[xlist >= pxlist[1] & xlist <= pxlist[2]] }
            }

            if (ydim == "media" || ydim == "space" || ydim == "space1" || ydim == "space2") {
                strylist <- as.character(row[,yfield])
                strylist <- substr(strylist, 2, nchar(strylist)-1)
                pylist <- strsplit(strylist,', ')[[1]]
            }
            
            if (ydim == "time") {                
                strylist <- as.character(row[,yfield])
                strylist <- substr(strylist, 2, nchar(strylist)-1)
                pylist <- strsplit(strylist,', ')[[1]]
                if (length(pylist) > 1) { pylist <- ylist[ylist >= pylist[1] & ylist <= pylist[2]] }
            }

            if (!dataModel) {
                sign <- row$DATA / row$MODEL
            }

            if (dataModel) {
                if (row$MODEL == 0) {
                    sign <- NaN
                } else {
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
                }
            }
            
            for (x in pxlist) {
                for (y in pylist) {
                    microData[microData$xlist == x & microData$ylist == y,"value"] <- row$DATA
                    microData[microData$xlist == x & microData$ylist == y,"sign"] <- sign
                    microData[microData$xlist == x & microData$ylist == y,"partNb"] <- partNb
                }
            }
            partNb <- partNb+1
        }

        for (i in seq(1,nrow(microData))) {
            row <- microData[i,]
            
            if (row$x == 1) {
                microData[i,"left"] <- TRUE
            } else {
                if (microData[microData$x == row$x - 1 & microData$y == row$y,"partNb"] != row$partNb) { microData[i,"left"] <- TRUE }
            }
            
            if (row$x == length(xlist)) {
                microData[i,"right"] <- TRUE
            } else {
                if (microData[microData$x == row$x + 1 & microData$y == row$y,"partNb"] != row$partNb) { microData[i,"right"] <- TRUE }
            }
            
            if (row$y == 1) {
                microData[i,"up"] <- TRUE
            } else {
                if (microData[microData$x == row$x & microData$y == row$y - 1,"partNb"] != row$partNb) { microData[i,"up"] <- TRUE }
            }
            
            if (row$y == length(ylist)) {
                microData[i,"down"] <- TRUE
            } else {
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

        microData$xlist <- factor(microData$xlist, levels = as.list(xlist))
        microData$ylist <- factor(microData$ylist, levels = as.list(ylist))
        
        if (!dataModel) {
            l <- lmax

            g <- ggplot (microData, aes (x = xlist, y = ylist))
            g <- g + geom_tile (aes (fill = sign))
            g <- g + scale_fill_gradient (low = "white", high = "black", na.value = "white", name="Citations", limits=c(0,l))
            g <- g + ggtitle (paste("",sep="")) + xlab (xdim) + ylab (ydim)
            g <- g + theme (axis.text.x = element_text(angle = 90, hjust = 1, size=13, color="black"))
            g <- g + theme (axis.text.y = element_text(size=13, color="black"))
            g <- g + geom_segment(data=segments,aes(x=x,y=y,xend=xe,yend=ye))
            if (partId) { g <- g + geom_text(aes(label = label), size=3) }
        }

        if (dataModel) {
            l <- max(-lmin,lmax)

            g <- ggplot (microData, aes (x = xlist, y = ylist))
            g <- g + geom_tile (aes (fill = sign))
            g <- g + scale_fill_gradient2 (low = "blue", mid="white", high = "red", na.value = "grey", name="Signif.", limits=c(-l,l))
            g <- g + ggtitle (paste("",sep="")) + xlab (xdim) + ylab (ydim)
            g <- g + theme (axis.text.x = element_text(angle = 90, hjust = 1))
            g <- g + geom_segment(data=segments,aes(x=x,y=y,xend=xe,yend=ye))
            if (partId) { g <- g + geom_text(aes(label = label), size=3) }
        }

        strNum <- str_pad (num, maxNum, pad = "0")
        ggsave (file = paste(outputName,".num=",strNum,".scale=",scale,".size=",size,".loss=",informationLoss,".png",sep=""), width=9, height=6)

        num <- num + 1
    }
}
