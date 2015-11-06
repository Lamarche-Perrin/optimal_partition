rm(list=ls())

file1 <- "en_GBR_guardi_int.csv"
file2 <- "itemsPays.csv"
file3 <- "pays.csv"
file4 <- "dist_cepii.csv"

output1 <- "data.csv"
output2 <- "space.csv"
output3 <- "time.csv"
output4 <- "graph.csv"

setwd("C:/Users/Robin/Mes projets/programming/aggregation_algorithms/aggregation/full_aggregation/data")


rss <- read.table(file.path(file1),
                    header=TRUE,sep="\t",
                    stringsAsFactors=FALSE,
                    encoding="UTF-8")
  
tags <- read.table(file.path(file2),
                  header=TRUE,sep=",",
                  stringsAsFactors=FALSE,
                  encoding="UTF-8")

date <- substr(rss[rss$ID_Item == tags[1,"idItem"],"Date_publication"],1,10)
tags[1,"date"] <- date
dateMin <- date
dateMax <- date

for (i in 2:nrow(tags)) {

  date <- substr(rss[rss$ID_Item == tags[i,"idItem"],"Date_publication"],1,10)
  if (date < dateMin) { dateMin <- date }
  if (date > dateMax) { dateMax <- date }
  tags[i,"date"] <- date

}

write.table(tags,file.path(output1),row.names=FALSE,col.names=FALSE,sep=",",quote=FALSE)

sDates <- seq(as.Date(dateMin),to = as.Date(dateMax), by='days')
write.table(sDates,file.path(output3),row.names=FALSE,col.names=FALSE,sep=",",quote=FALSE)


pays <- read.table(file.path(file3),
                  header=TRUE,sep=";",quote="",
                  stringsAsFactors=FALSE,
                  encoding="UTF-8")

write.table(pays[,1],file.path(output2),row.names=FALSE,col.names=FALSE,sep=",",quote=FALSE)


graph <- read.table(file.path(file4),
                   header=TRUE,sep=";",quote="",
                   stringsAsFactors=FALSE,
                   encoding="UTF-8")

write.table(graph[graph$contig==1,c("iso_o","iso_d")],file.path(output4),row.names=FALSE,col.names=FALSE,sep=",",quote=FALSE)

