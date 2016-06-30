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

        if (onlyWithGeoTag)
        {
            if (!is.null (spaceSelection))
            { data <- data[data[,spaceField] %in% spaceSelection,] }
        }

        if (onlyWithThemeTag)
        {
            data <- data[data[,tagField] == themeTag,]
        }


        
        ## Suppress items with no tag and items with only the national tag
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

    if (is.null(spaceSelection)) { return (dataCube[mediaSelection,timeList]) }
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
