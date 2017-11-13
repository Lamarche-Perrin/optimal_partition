rm(list = ls())
source("functions.R")

dataCube <- getDataCube (selectedList = "Agenda", onlyInternationalItems = TRUE)
writeDataCube (dataCube, "cube.csv")

smallDataCube <- filterDataCube (dataCube, timeFirst="2014-04-28", timeLast="2014-05-12")
smallDataCube <- filterDataCubeByOrder (smallDataCube, mediaNumber=3, spaceNumber=3)
writeDataCube (smallDataCube, "smallCube.csv")

dataModels <- getDataModels (dataCube)
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataModels (smallDataModels, "smallModel.csv")



smallDataCube <- readDataCube ("smallCube.csv")
smallDataModels <- readDataModels ("smallModel.csv")

stDataCube <- cutDataCube (smallDataCube, media="en_GBR_guardi_int")
stDataModels <- cutDataModels (smallDataModels, media="en_GBR_guardi_int")

writeDataCube (stDataCube, "stCube.csv")
writeDataModels (stDataModels, "stSmallModel.csv")

##dataCube <- getDataCubeFromSample ()
##dataframeCube <- getDataframeCube (dataCube,dataModels)


cube <- readDataCube ("EU_cube.csv")
dimnames(cube$data)

fube <- filterDataCube (cube$data, mediaSelection = c("fr_FRA_lmonde_int"), firstTimeSelection = "2013-12-30", lastTimeSelection = "2013-12-30")

writeDataCube (fube, fileName="S.EU_cube.csv")
cubefube

as.vector(fube)
dimnames(fube)$space

