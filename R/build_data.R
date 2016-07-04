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

smallMediaList <- c(
    "fr_FRA_lmonde_int",
    "fr_BEL_lesoir_int",
    "es_ESP_elpais_int"
)

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
    firstDateSelection="2013-12-30",
    lastDateSelection="2014-12-29",
    timeLevel = "month",
    onlyInternationalItems = TRUE
)

dataModels <- getDataModels (dataCube, c("MT","MS","ST"))

UKRDataCube <- filterDataCube (dataCube, timeFirst="2013-12-30", timeLast="2014-11-29", mediaList=mediaList, spaceList=c("UKR"))
UKRDataModels <- filterDataModelsFromDataCube (dataModels, UKRDataCube)
writeDataCube (UKRDataCube, UKRDataModels, "UKR_cube.csv")

smallDataCube <- filterDataCube (dataCube, timeFirst="2013-12-30", timeLast="2014-02-27", mediaList=smallMediaList, spaceList=c("UKR"))
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataCube (smallDataCube, smallDataModels, "small_UKR_cube.csv")






dataCube <- getDatacubeFromDataset (
    datasetDirectory = "../input/Geomedia_extract_AGENDA",
    themeTag = "Ebola",
    mediaSelection = c("fr_FRA_lmonde_int","fr_BEL_lesoir_int","es_ESP_elpais_int"),
    countrySelection = c("FRA","DEU"),
    firstDateSelection = "2014-01-01",
    lastDateSelection = "2014-12-31",
    timeLevel = "week",
    onlyInternational = TRUE
)
dataCube


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
    onlyWithGeoTag = TRUE,
    onlyInternational = TRUE
)

dataModels <- getDataModelsFromDataCube (dataCube, c("MT","MS","ST"))

filteredDataCube <- filterDataCube (
    dataCube,
    firstTimeSelection = "2014-05-19",
    lastTimeSelection = "2014-10-05",
    mediaSelection = c("fr_FRA_lmonde_int"),
    spaceSelection = c("PSE","ISR","RUS","UKR","IRQ","GBR","IND","USA","CUB","SYR","TUR","PAK")
)

filteredDataModels <- filterDataModelsFromDataCube (dataModels, filteredDataCube)

writeDataCube (filteredDataCube, filteredDataModels, "../input/geomedia/ST.LeMonde.2.cube.csv")


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
    cubeFileName = "../input/geomedia/small.ST.LeMonde.cube.csv",
    partitionFileName = "../output/geomedia/ST.LeMonde/small.ST.LeMonde.ST.partitions.csv",
    outputName = "../output/geomedia/ST.LeMonde/small.ST.LeMonde.ST.partition",
    xdim = "time", ydim = "space"
)

printHeatmaps (
    cubeFileName = "../input/geomedia/ST.LeMonde.cube.csv",
    partitionFileName = "../output/geomedia/ST.LeMonde/ST.LeMonde.ST.partitions.csv",
    outputName = "../output/geomedia/ST.LeMonde/ST/ST.LeMonde.ST.partition",
    xdim = "time", ydim = "space"
)

printHeatmaps (
    cubeFileName = "../input/geomedia/ST.LeMonde.cube.csv",
    partitionFileName = "../output/geomedia/ST.LeMonde/ST.LeMonde.MS.partitions.csv",
    outputName = "../output/geomedia/ST.LeMonde/MS/ST.LeMonde.MS.partition",
    xdim = "time", ydim = "space"
)

printHeatmaps (
    cubeFileName = "../input/geomedia/ST.LeMonde.cube.csv",
    partitionFileName = "../output/geomedia/ST.LeMonde/ST.LeMonde.MT.partitions.csv",
    outputName = "../output/geomedia/ST.LeMonde/MT/ST.LeMonde.MT.partition",
    xdim = "time", ydim = "space"
)



