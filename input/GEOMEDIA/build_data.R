rm(list = ls())
source("functions.R")

dataCube <- getDataCube (selectedList = "Agenda", onlyInternationalItems = TRUE)
writeDataCube (dataCube,"cube.csv")

smallDataCube <- filterDataCube (dataCube, timeFirst="2014-04-28", timeLast="2014-05-12")
smallDataCube <- filterDataCubeByOrder (smallDataCube, mediaNumber=3, spaceNumber=3)
writeDataCube (smallDataCube, "smallCube.csv")

dataModels <- getDataModels (dataCube)
smallDataModels <- filterDataModelsFromDataCube (dataModels, smallDataCube)
writeDataModels (smallDataModels, "smallModels.csv")


##dataCube <- getDataCubeFromSample ()
##dataframeCube <- getDataframeCube (dataCube,dataModels)



















