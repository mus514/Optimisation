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

regular_calcul <- function(liste, sheet, opt_unsolved = FALSE)
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

##write_xlsx(data_list, "resultat_basic.xlsx")


# Conparaison of the complex results 



tri_calculs <- function(liste, sheet, h = 0, inv = "", m = 0, ord = 0, 
                        col = "", vehicule, best_case = FALSE)
{
  data <-liste[[sheet]]
  
  resultat <- vector()
  
  ##
  if((h & m) != 0 & inv != "" & ord == 0)
  {
    
    data <- subset(data, data[["H"]] %in% h & data[["Inventory"]] %in% inv &
                     data[["n"]] %in% m)
    
    data <- data[, -1:-4]
    
    if(vehicule == 2)
    {
      data <- data[, c(3, 5, 6, 4)]
    }
    
    else if(vehicule == 3)
    {
      data <- data[, c(3, 5, 6, 4) + 6]
    }
    
    else if(vehicule == 4)
    {
      data <- data[, c(3, 5, 6, 4) + 12]
    }
    
    else
    {
      data <- data[, c(3, 5, 6, 4) + 18]
    }
    
    resultat <- apply(data, 2, mean, na.rm = T)
    resultat[c(2, 3)] <- format(resultat[c(2, 3)] * 100, scientific = F)
  }
  
  ##
  else if(ord != 0 & (m & h) == 0 & inv == "")
  {
    data <- subset(data, data[["Ordering"]] %in% ord)
    
    if(col == "Opt")
    {
      if(vehicule == 2)
      {
        data <- data[, 9]
      }
      
      else if(vehicule == 3)
      {
        data <- data[,  15]
      }
      
      else if(vehicule == 4)
      {
        data <- data[, 21]
      }
      
      else
      {
        data <- data[, 27]
      }
      
    }
    
    else
    {
      if(vehicule == 2)
      {
        data <- data[, 10]
      }
      
      else if(vehicule == 3)
      {
        data <- data[,  16]
      }
      
      else if(vehicule == 4)
      {
        data <- data[, 22]
      }
      
      else
      {
        data <- data[, 28]
      }
    }
  }
  ##
  
  else if(inv != "" & (h & m & ord) == 0)
  {
    data <- subset(data,data[["Inventory"]] %in% inv)
    
    data <- data[, -1:-4]
    
    if(vehicule == 2)
    {
      data <- data[, 3:6]
    }
    
    else if(vehicule == 3)
    {
      data <- data[, 3:6 + 6]
    }
    
    else if(vehicule == 4)
    {
      data <- data[, 3:6 + 12]
    }
    
    else
    {
      data <- data[, 3:6 + 18]
    }
    
    resultat <- apply(data[,c(1, 2)], 2, mean, na.rm = T)
    resultat <- append(resultat, apply(data[,c(3, 4)], 2, sum, na.rm = T))
  }
  
  resultat
}


tri_best <- function(liste, sheet, ord)
{
  data <- liste[[sheet]]
  
  data <- subset(data, data[["Inventory"]] %in% "high" & 
                   data[["Ordering"]] %in%ord)
  
  data <- data[ ,c(9, 10, 15, 16, 21, 22, 27, 28)]
  
  result_1 <- apply(data, 2, sum, na.rm = T)
  
  data <- liste[[sheet]]
  
  data <- subset(data, data[["Inventory"]] %in% "low" & 
                   data[["Ordering"]] %in%ord)
  
  data <- data[ ,c(9, 10, 15, 16, 21, 22, 27, 28)]
  
  result_2 <- apply(data, 2, sum, na.rm = T)
  
  result <- rep(0, 16)
  j <- 1
  
  for(i in seq(from = 1, to = 15, by = 2))
  {
    result[i] <- result_1[j]
    j <- j + 1
  }
  
  j <- 1
  for(i in seq(from = 2, to = 16, by = 2))
  {
    result[i] <- result_2[j]
    j <- j + 1
  }
  
  result
}









title_6 <- c("H", 3, rep("", 9), 3, rep("", 9), 6, rep("", 5), 6, rep("",5))

title_7 <- c("Inventory", "High", rep("", 9), "Low", rep("", 9),
             "High", rep("", 5), "Low", rep("",5))

title_8 <- c("n", rep(seq(from = 5, to = 50, by = 5), 2), 
             rep(seq(from = 5, to = 30, by = 5), 2))

title_9 <- rep(c("Gap", "%Opt", "%Unsolved", "time"), 8)


