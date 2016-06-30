library(ggplot2)
source("functions.R")

mediaOrder <- c(
    "fr_BEL_lesoir_int",
    "es_ESP_elpais_int",
    "fr_FRA_lmonde_int",
    "en_CAN_starca_int",
    "en_SGP_twoday_int",
    "en_GBR_guardi_int",
    "en_NZL_nzhera_int",
    "en_JPN_jatime_int",
    "en_CHN_mopost_int",
    "en_AUS_austra_int",
    "en_USA_nytime_int",
    "en_IND_tindia_int"
)

printHeatmaps ("UKR_cube", "MS_UKR_partitions", mediaOrder=mediaOrder)

