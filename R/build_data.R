rm(list = ls())
source("functions.R")

##countrySelection = c("AUT","BEL","BGR","HRV","CYP","DNK","EST","FIN","FRA","DEU","GRC","HUN","IRL","ITA","LVA","LTU","LUX","MLT","NLD","POL","PRT","ROU","SVK","SVN","ESP","SWE","UKR","GBR"),
##mediaSelection = c("en_GBR_dailyt_int","en_GBR_guardi_int","es_ESP_catalu_int","es_ESP_elpais_int","fr_BEL_derheu_int","fr_BEL_lesoir_int","fr_FRA_lepari_int","fr_FRA_lmonde_int"),


mediaList <- c(
    "fr_FRA_lmonde_int",
    "fr_BEL_lesoir_int",
    "es_ESP_elpais_int",
    "en_GBR_guardi_int",
    "en_USA_nytime_int",
    "en_AUS_austra_int",
    "en_CAN_starca_int",
    "en_NZL_nzhera_int",
    "en_CHN_mopost_int",
    "en_IND_tindia_int",
    "en_JPN_jatime_int",
    "en_SGP_twoday_int"
)

dataCube <- getDataCube (
    dataDirectory = "Geomedia_extract_AGENDA_titre&desc",
    eventTag = "Ebola",
    mediaSelection = mediaList,
    countrySelection = c("UKR"),
    firstDateSelection = "2014-01-01",
    lastDateSelection = "2014-12-31",
    timeLevel = "month",
    onlyInternationalItems = TRUE
)


dataCube
dataModels <- getDataModels (dataCube, c("MT","MS","ST"))
writeDataCube (dataCube, dataModels, "UKR_cube.csv")

smallDataCube <- filterDataCube (dataCube, timeFirst="2014-01-06", timeLast="2014-03-10")
smallDataCube <- filterDataCubeByOrder (smallDataCube, mediaNumber=10, spaceNumber=10)
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataCube (smallDataCube, smallDataModels, "largeCube.csv")


## EBOLA EXPERIMENT

dataCube <- getDataCube (
    dataDirectory = "Geomedia_extract_AGENDA_titre&desc",
    eventTag = "Ebola",
    #mediaSelection = c("en_GBR_dailyt_int","en_GBR_guardi_int"),
    #firstDateSelection = "2014-01-01",
    #lastDateSelection = "2014-12-31",
    timeLevel = "week",
    onlyWithTag = TRUE,
    onlyGeolocated = FALSE,
    onlyInternationalItems = FALSE
)

dataCube[,,"NOC"] <- apply(dataCube, c(1,2), sum)
dataCube[,,dimnames(dataCube)$space[dimnames(dataCube)$space != "NOC"]] <- 0

dataModels <- getDataModels (dataCube, c("MT"))

smallDataCube <- filterDataCube (dataCube, spaceList=c("NOC"))
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataCube (smallDataCube, smallDataModels, "ebola_tag.csv")


dataCube <- getDataCube (
    dataDirectory = "Geomedia_extract_AGENDA_titre&desc",
    eventTag = "Ebola",
    #mediaSelection = c("en_GBR_dailyt_int","en_GBR_guardi_int"),
    #firstDateSelection = "2014-01-01",
    #lastDateSelection = "2014-12-31",
    timeLevel = "week",
    onlyWithTag = FALSE,
    onlyGeolocated = FALSE,
    onlyInternationalItems = FALSE
)

dataCube[,,"NOC"] <- apply(dataCube, c(1,2), sum)
dataCube[,,dimnames(dataCube)$space[dimnames(dataCube)$space != "NOC"]] <- 0

dataModels <- getDataModels (dataCube, c("MT"))

smallDataCube <- filterDataCube (dataCube, spaceList=c("NOC"))
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataCube (smallDataCube, smallDataModels, "ebola_all.csv")


## DATA FOR ASM

dataCube <- getDataCube (
    dataDirectory = "Geomedia_extract_AGENDA",
    eventTag = "Ebola",
    firstDateSelection = "2014-12-15",
    lastDateSelection = "2014-12-21",
    timeLevel = "week",
    onlyInternationalItems = TRUE
)

dataModels <- getDataModels (dataCube, c("MT","MS","ST"))

smallDataCube <- filterDataCubeByOrder (dataCube, spaceNumber=48)
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataCube (smallDataCube, smallDataModels, "MS_2014-12-15.csv")



## UKR EXPERIMENT

rm(list = ls())
source("functions.R")

mediaList <- c(
    "fr_FRA_lmonde_int",
    "fr_BEL_lesoir_int",
    "es_ESP_elpais_int",
    "en_GBR_guardi_int",
    "en_USA_nytime_int",
    "en_AUS_austra_int",
    "en_CAN_starca_int",
    "en_NZL_nzhera_int",
    "en_CHN_mopost_int",
    "en_IND_tindia_int",
    "en_JPN_jatime_int",
    "en_SGP_twoday_int"
)

dataCube <- getDataCubeFromDataset (
    datasetDirectory = "../input/Geomedia_extract_AGENDA",
    mediaFile = "X.Global_metadata.csv",
    itemFile = "rss_unique_TAG_country_Ebola.csv",
    itemField = "ID", mediaField1 = "Name_Flux", mediaField2 = "feed",
    timeField = "time", spaceField = "TAG_country", tagField = "TAG_ebola",
    timeLevel = "week",
    firstTimeSelection = "2014-04-28",
    lastTimeSelection = "2014-10-26",
    onlyWithGeoTag = TRUE,
    onlyInternational = TRUE,
    onlyWithThemeTag = FALSE
)

dataModels <- getDataModelsFromDataCube (dataCube, c("MT","MS","ST"))

UKRDataCube <- filterDataCube (dataCube, mediaSelection=mediaList, spaceSelection=c("UKR"))
UKRDataModels <- filterDataModelsFromDataCube (dataModels, UKRDataCube)
writeDataCube (UKRDataCube, UKRDataModels, "../input/geomedia/MT.UKR.2.cube.csv")

smallDataCube <- filterDataCube (dataCube, timeFirst="2013-12-30", timeLast="2014-02-27", mediaList=smallMediaList, spaceList=c("UKR"))
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataCube (smallDataCube, smallDataModels, "small_UKR_cube.csv")


printHeatmaps (
    partitionFileName = "../output/geomedia/MT.UKR/MT.UKR.1.MS.partitions.csv",
    outputName = "../output/geomedia/MT.UKR/MS/MT.UKR.1.MS.partition",
    xdim = "time", ydim = "media", dataModel = TRUE, log = TRUE
)





## SPATIO-TEMPORAL EXPERIMENT

rm(list = ls())
source("functions.R")

dataCube <- getDataCubeFromDataset (
    datasetDirectory = "../input/Geomedia_extract_AGENDA",
    mediaFile = "X.Global_metadata.csv",
    itemFile = "rss_unique_TAG_country_Ebola.csv",
    itemField = "ID", mediaField1 = "Name_Flux", mediaField2 = "feed",
    timeField = "time", spaceField = "TAG_country", tagField = "TAG_ebola",
    timeLevel = "week",
    firstTimeSelection = "2014-01-06",
    lastTimeSelection = "2014-06-30",
    onlyWithGeoTag = TRUE,
    onlyInternational = TRUE,
    onlyWithThemeTag = FALSE
)

dataModels <- getDataModelsFromDataCube (dataCube, c("MT","MS","ST"))

filteredDataCube <- filterDataCube (
    dataCube,
    firstTimeSelection = "2014-01-06",
    lastTimeSelection = "2014-06-30",
    mediaSelection = c("fr_FRA_lmonde_int"),
    spaceSelection = c("USA","UKR","RUS","CHN","IRQ","SYR","ISR","PSE","IRN","IND","PAK","CUB")
)

filteredDataModels <- filterDataModelsFromDataCube (dataModels, filteredDataCube)

writeDataCube (filteredDataCube, filteredDataModels, "../input/geomedia/ST.LeMonde.4.cube.csv")


smallDataCube <- filterDataCube (
    dataCube,
    firstTimeSelection = "2014-06-02",
    lastTimeSelection = "2014-07-27",
    mediaSelection = c("fr_FRA_lmonde_int"),
    spaceSelection = c("PSE","ISR","RUS","UKR","IRQ","GBR","IND","FRA","USA","CUB","SYR","TUR")
)

smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)

writeDataCube (smallDataCube, smallDataModels, "../input/geomedia/small.ST.LeMonde.cube.csv")


## print heatmaps

printHeatmaps (
    partitionFileName = "../output/geomedia/ST.LeMonde/ST.LeMonde.4.ST.partitions.csv",
    outputName = "../output/geomedia/ST.LeMonde/ST/ST.LeMonde.4.ST.partition",
    xdim = "time", ydim = "space", dataModel = TRUE, log = TRUE,
    yorder = c("SYR","IRN","PAK","ISR","PSE","RUS","UKR","CHN","USA","CUB","IND","IRQ")
)

printHeatmaps (
    partitionFileName = "../output/geomedia/ST.LeMonde/ST.LeMonde.4.MS.partitions.csv",
    outputName = "../output/geomedia/ST.LeMonde/MS/ST.LeMonde.4.MS.partition",
    xdim = "time", ydim = "space", dataModel = TRUE, log = TRUE,
    yorder = c("SYR","IRN","PAK","ISR","PSE","RUS","UKR","CHN","USA","CUB","IND","IRQ")
)

printHeatmaps (
    partitionFileName = "../output/geomedia/ST.LeMonde/ST.LeMonde.4.MT.partitions.csv",
    outputName = "../output/geomedia/ST.LeMonde/MT/ST.LeMonde.4.MT.partition",
    xdim = "time", ydim = "space", dataModel = TRUE, log = TRUE,
    yorder = c("SYR","IRN","PAK","ISR","PSE","RUS","UKR","CHN","USA","CUB","IND","IRQ")
)


## co-citations experiment

rm(list = ls())
source("functions.R")
source("functions2.R")

dataCube <- getDataCubeFromDataset2 (
    datasetDirectory = "../input/Geomedia_extract_AGENDA",
    mediaFile = "X.Global_metadata.csv",
    itemFile = "rss_unique_TAG_country_Ebola.csv",
    itemField = "ID", mediaField1 = "Name_Flux", mediaField2 = "feed",
    timeField = "time", spaceField = "TAG_country", tagField = "TAG_ebola",
    firstTimeSelection = "2014-01-01",
    lastTimeSelection = "2014-12-31",
    timeLevel = "year",
    onlyWithGeoTag = TRUE,
    onlyInternational = FALSE,
    onlyWithThemeTag = FALSE
)

dataModels <- getDataModelsFromDataCube2 (dataCube, c("null", "MT","MSS","SST","SS"))

filteredDataCube <- filterDataCube2 (
    dataCube,
    mediaSelection = c("fr_FRA_lmonde_int"),
    spaceSelection = c("USA","IRN","RUS","IRQ","SYR","TUR","DEU","FRA","CHN")
)

filteredDataModels <- filterDataModelsFromDataCube2 (dataModels, filteredDataCube)

writeDataCube2 (filteredDataCube, filteredDataModels, "../input/geomedia/SS.LeMonde.3.cube.csv")


order <- c("FRA","USA","DEU","TUR","SYR","IRN","IRQ","RUS","CHN")

printHeatmaps (
    partitionFileName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.null.partitions.csv",
    outputName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.null.partition",
    xdim = "space2", ydim = "space1", dataModel = FALSE, log = TRUE,
    xorder = order, yorder = rev(order), scale = 0, partId = FALSE
)

printHeatmaps (
    partitionFileName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.SS.partitions.csv",
    outputName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.SS.partition",
    xdim = "space2", ydim = "space1", dataModel = TRUE, log = TRUE,
    xorder = order, yorder = rev(order), scale = 0, partId = FALSE
)

printHeatmaps (
    partitionFileName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.MSS.partitions.csv",
    outputName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.MSS.partition",
    xdim = "space2", ydim = "space1", dataModel = TRUE, log = TRUE,
    xorder = order, yorder = rev(order), scale = 0, partId = FALSE
)




printHeatmaps (
    partitionFileName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.null.partitions.csv",
    outputName = "../output/geomedia/SS.LeMonde/null/SS.LeMonde.3.null.partition",
    xdim = "space2", ydim = "space1", dataModel = FALSE, log = TRUE,
    xorder = order, yorder = rev(order)
)

printHeatmaps (
    partitionFileName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.SS.partitions.csv",
    outputName = "../output/geomedia/SS.LeMonde/SS/SS.LeMonde.3.SS.partition",
    xdim = "space2", ydim = "space1", dataModel = TRUE, log = TRUE,
    xorder = order, yorder = rev(order)
)

printHeatmaps (
    partitionFileName = "../output/geomedia/SS.LeMonde/SS.LeMonde.3.MSS.partitions.csv",
    outputName = "../output/geomedia/SS.LeMonde/MSS/SS.LeMonde.3.MSS.partition",
    xdim = "space2", ydim = "space1", dataModel = TRUE, log = TRUE,
    xorder = order, yorder = rev(order)
)
