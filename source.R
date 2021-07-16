library("readxl")
library("writexl")
library("stringr")
setwd("~/Google drive/data_analysis")

### Import data

importData <- function(x)
{
                        
  if(x == "SBC.xlsx")
  {
    sheet <- tail(excel_sheets(x), 13)
  }
  
  else if(grepl("ARF", x))
  {
    sheet <- excel_sheets(x)
  }
  
  else
  {
    sheet <- tail(excel_sheets(x), 12)
  }
  
  data_list <- list()
  
  for(i in 1:length(sheet))
  {
    data <- read_excel(x, sheet = sheet[i])
    
    data <- data[2:2561, c(1, 2, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 
                           20, 21, 28, 29, 30, 31, 32, 33, 40, 41,
                           42, 43, 44, 45, 52, 53)]
    
    names(data)[1:4] <- c("H", "Inventory", "n", "Ordering")
    
    name <- c("UB", "LB", "Gap", "Time", "Opt", "Unsolved")
    
    k <- 2
    
    for(a in c(5, 11, 17, 23))
    {
      for(j in 0:5)
      {
        names(data)[a+j] <- paste0("V", k,"_",name[j+1])
      }
      
      k <- k + 1
    }
    
    data[,1] <- sapply(data[,1], as.numeric)
    data[,2] <- sapply(data[,2], as.character)
    
    for(s in 3:28)
    {
      data[ ,s] <- sapply(data[ ,s], as.numeric)
    }
    
    data_list[[i]] <- data
    
  }
  
  sheet <- str_remove(sheet, "vi_")
  names(data_list) <- sheet
  data_list
  
}


sbc <- importData("SBC.xlsx")
arf_sf <- importData("ARF - SF.xlsx")
sbc_vc <- importData("SBC - VC.xlsx")



### Tables and Statistics

basic_calcul <- function(liste, sheet, opt_unsolved = FALSE)
{
  data <- liste[[sheet]]
  data <- data[, -1:-4]
  
  resultat <- 0

  if(opt_unsolved)
  {
    name <- names(data)
    name <- name[str_detect(name, c("Opt", "Unsolved"))]
    j <- 1
    
    for(i in name)
    {
      resultat[j] <- sum(data[[i]], na.rm = T)
      j <- j + 1
    }
  }
  
  else
  {
    name <- names(data)
    j <- 1
    
    for(i in name)
    {
      if(grepl("Opt", i) || grepl("Unsolved", i))
      {
        resultat[j] <- sum(data[[i]], na.rm = T)
      }
      
      else
      {
        resultat[j] <- mean(data[[i]], na.rm = T)
        
      }
      
      j <- j + 1
    }
  }
  
  resultat
}


### Tables

data_list <- list()

title_1 <- c(c("", "2 Vehicles", rep("", 5), "3 Vehicles", rep("", 5)), 
             "4 Vehicles", rep("", 5), "5 Vehicles", rep("", 5))
  
title_2 <- c("Average", rep(NA, 3), "Sum", NA, "Average", rep(NA, 3), 
               "Sum", NA, "Average", rep(NA, 3), "Sum", NA, "Average", 
               rep(NA, 3), "Sum", NA)
  
title_3 <- c(rep(c("UB", "LB", "Gap", "Time", "#Opt", "#Unsolved"), 4))
  
title_4 <- c("SF", "vc", "vr", "vc+vr")
  
title_5 <- c("hc1_full", "hc2", "hc3", "cos", "qua", "cus", "lex_full")
 
# Standard Formulation 
temp <- basic_calcul(arf_sf, "SF")
  
temp <- rbind(temp, basic_calcul(sbc, "vc"))
  
temp <- rbind(temp, basic_calcul(sbc, "vr"))
  
temp <- rbind(temp, basic_calcul(sbc_vc, "vc+vr"))
  
temp <- rbind(title_2, title_3 ,temp)
  
temp <- cbind(c(rep("", 2), title_4), temp)
  
temp <- as.data.frame(temp)
  
names(temp) <- title_1
  
data_list[["Standard_formulation"]]<- temp

# SBC
temp <- vector()

for(i in title_5)
{
  temp <- rbind(temp, basic_calcul(sbc, i))
}

temp <- rbind(title_2, title_3 ,temp)

temp <- cbind(c(rep("", 2), title_5), temp)

temp <- as.data.frame(temp)

names(temp) <- title_1

data_list[["Symetry_breaking"]]<- temp



# Conparaison of the reults obtained with ARF vs SF

temp <- vector()

temp <- rbind(temp, basic_calcul(arf_sf, "SF", TRUE))

temp <- rbind(temp, basic_calcul(arf_sf, "ARF", TRUE))

temp <- rbind(rep(c("#Opt", "#Unsolved"), 4), temp)

temp <- cbind(c("", "SF", "ARF"), temp)

temp <- as.data.frame(temp)

names(temp) <- c("", "2 Vehicles", "", "3 Vehicles","", "4 Vehicles","", 
                  "5 Vehicles", "")

data_list[["Comparison_AFR_SF"]]<- temp

## Combined VC result

temp <- vector()

name_1 <- names(sbc)[str_detect(names(sbc), "vc")]


for(i in name_1)
{
  temp <- rbind(temp, basic_calcul(sbc, i))
}

name_2 <- names(sbc_vc)

for(i in name_2)
{
  temp <- rbind(temp, basic_calcul(sbc_vc, i))
}

temp <- rbind(title_2, title_3, temp)

temp <- cbind(c("", "", name_1, name_2), temp)

temp <- as.data.frame(temp)

names(temp) <- title_1

data_list[["Combined VC results"]] <- temp


## ARF vs VC

temp <- vector()

temp <- rbind(temp, basic_calcul(sbc, "vc"))

temp <- rbind(temp, basic_calcul(arf_sf, "ARF"))

temp <- rbind(title_2, title_3, temp)

temp <- cbind(c("", "", "VC", "ARF"), temp)

temp <- as.data.frame(temp)

names(temp) <- title_1

data_list[["ARF vs VC"]] <- temp

write_xlsx(data_list, "resultat_basic.xlsx")
