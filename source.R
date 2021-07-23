###
### Mustapha.B
###

library("readxl")
library("writexl")
library("stringr")

## Setting directory
setwd("~/Google drive/data_analysis")

###
### impoortFile(file)
###
## importFile : Import data from excel file
##
## Argument (file) : name of the excel file
##
## Output : list of data
##
## Example : importFile("SBC.xlsx")
##

importFile <- function(file)
{
  
  if(file == "SBC.xlsx")
  {
    sheet <- tail(excel_sheets(file), 13)
  }
  
  else if(grepl("ARF", file))
  {
    sheet <- excel_sheets(file)
  }
  
  else
  {
    sheet <- tail(excel_sheets(file), 12)
  }
  
  data_list <- list()
  
  for(i in 1:length(sheet))
  {
    data <- read_excel(file, sheet = sheet[i])
    
    data <- data[2:2561, c(1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 
                           20, 21, 28, 29, 30, 31, 32, 33, 40, 41,
                           42, 43, 44, 45, 52, 53)]
    
    names(data)[1:5] <- c("H", "Inventory", "#", "n", "Ordering")
    
    name <- c("UB", "LB", "Gap", "Time", "Opt", "Unsolved")
    
    k <- 2
    
    for(a in c(6, 12, 18, 24))
    {
      for(j in 0:5)
      {
        names(data)[a+j] <- paste0("V", k,"_",name[j+1])
      }
      
      k <- k + 1
    }
    
    data[,1] <- sapply(data[,1], as.numeric)
    data[,2] <- sapply(data[,2], as.character)
    
    for(s in 3:29)
    {
      data[ ,s] <- sapply(data[ ,s], as.numeric)
    }
    
    data_list[[i]] <- data
    
  }
  
  sheet <- str_remove(sheet, "vi_")
  names(data_list) <- sheet
  
  data_list
  
}

###
### find_missing(liste)
###
## find_missing : find missing run Time
##
## Argument (liste) : list of data frame
##
## Output : list file name (.json)
##
## Example : find_missing(sbc)
##

find_missing <- function(liste)
{
  data <- list()
  
  temp <- vector()
  
  for(j in names(liste))
  {
    for(k in 1:2560)
    {
      for(l in c(9, 15, 21, 27))
      {
        if(is.na(liste[[j]][k, l]))
        {
          if(liste[[j]][["Ordering"]][k] == 0)
          {
            temp <- append(temp, paste0("absH", liste[[j]][["H"]][k], liste[[j]][["Inventory"]][k],
                                    "_", liste[[j]][["#"]][k], "n", liste[[j]][["n"]][k], ".json"))
          }
          
          else
          {
            temp <- append(temp, paste0("absH", liste[[j]][["H"]][k], liste[[j]][["Inventory"]][k],
                                       "_", liste[[j]][["#"]][k], "n",liste[[j]][["n"]][k],"_", 
                                       liste[[j]][["Ordering"]][k], ".json"))
          }
        }  
      }
    }
    
    data[[j]] <- as.data.frame(unique(temp))
    temp <- vector()
  }
  
  data
}

###
### standard_calculs(liste, sheet, opt_unsolved = FALSE)
###
## standard_calculs: Calcul standard formulation ans symmetry breaking constraints
##
## Argument 
##
##        liste : list of data frame
##        sheet : the data frame target
##        opt_unsolved : if true return only Opt and Unsolved
##
## Output : numeric vector
##
## Example : standard_calculs(sbc, "vc")
##

standard_calculs <- function(liste, sheet, opt_unsolved = FALSE)
{
  data <- liste[[sheet]]
  data <- data[, -1:-5]
  
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


###
### subset_calculs(liste, sheet, opt_unsolved = FALSE)
###
## subset_calculs: Calcul standard formulation ans symmetry breaking constraints
##
## Argument 
##
##        liste : list of data frame
##        sheet : the data frame target
##        h : The periode 3 or 6
##        inv : the inventory high or low
##        m :  The instance from 5 to 50 by 5
##        ord : type or ordering from 1 to 15
##        col : could be Opt or Insolved
##        vehicles : numbers of vehicles used 2 to 5
##
##
## Output : numeric vector
##
## Example : subset_calculs(sbc, "vc", h = 3, inv = "high", m = 12, vehicles = 4)
##

subset_calculs <- function(liste, sheet, h = 0, inv = "", m = 0, ord = 0, 
                        col = "", vehicles)
{
  data <-liste[[sheet]]
  
  resultat <- vector()
  
  ##
  if((h & m) != 0 && inv != "" && ord == 0)
  {
    
    data <- subset(data, data[["H"]] %in% h & data[["Inventory"]] %in% inv &
                     data[["n"]] %in% m)
    
    data <- data[, -1:-5]
    
    if(vehicles == 2)
    {
      data <- data[, c(3, 5, 6, 4)]
    }
    
    else if(vehicles == 3)
    {
      data <- data[, c(3, 5, 6, 4) + 6]
    }
    
    else if(vehicles == 4)
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
  
  else if(ord != 0 && (m & h) == 0 && inv == "")
  {
    data <- subset(data, data[["Ordering"]] %in% ord)
    
    if(col == "Opt")
    {
      if(vehicles == 2)
      {
        data <- data[, 10]
      }
      
      else if(vehicles == 3)
      {
        data <- data[,  16]
      }
      
      else if(vehicles == 4)
      {
        data <- data[, 22]
      }
      
      else
      {
        data <- data[, 28]
      }
      
    }
    
    else
    {
      if(vehicles == 2)
      {
        data <- data[, 11]
      }
      
      else if(vehicles == 3)
      {
        data <- data[,  17]
      }
      
      else if(vehicles == 4)
      {
        data <- data[, 23]
      }
      
      else
      {
        data <- data[, 29]
      }
    }
    
    resultat <- sum(data, na.rm = T)
  }
  
  else if(inv != "" && (h & m & ord) == 0)
  {
    data <- subset(data,data[["Inventory"]] %in% inv)
    
    data <- data[, -1:-5]
    
    if(vehicles == 2)
    {
      data <- data[, 3:6]
    }
    
    else if(vehicles == 3)
    {
      data <- data[, 3:6 + 6]
    }
    
    else if(vehicles == 4)
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

###
### compare_calculs(liste, sheet, ord)
###
## compare_calculs: Calculs based on inventory High or Low and compare Opt and Unsolved
##
## Argument (file) 
##        liste : list of data frame
##        sheet : the data frame target
##        ord : type of ordering from 1 to 15
##
## Output : numeric vector
##
## Example : importFile("SBC.xlsx")
##

compare_calculs <- function(liste, sheet, ord)
{
  data <- liste[[sheet]]
  
  data <- subset(data, data[["Inventory"]] %in% "high" & 
                   data[["Ordering"]] %in%ord)
  
  data <- data[ ,c(10, 11, 16, 17, 22, 23, 28, 29)]
  
  result_1 <- apply(data, 2, sum, na.rm = T)
  
  data <- liste[[sheet]]
  
  data <- subset(data, data[["Inventory"]] %in% "low" & 
                   data[["Ordering"]] %in% ord)
  
  data <- data[ ,c(10, 11, 16, 17, 22, 23, 28, 29)]
  
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

###
### make_calculs(arf_sf, sbc, sbc_vc)
###
## make_calculs : Calcul all the cases
##
## Argument (arf_sf, sbc, sbc_vc) : All the lists of data from excel file
##
## Output : list of data fram 
##
## Example : make_calculs(arf_sf, sbc, sbc_vc)
##

make_calculs <- function(arf_sf, sbc, sbc_vc)
{
  data_list <- list()
  
  temp <- vector()
  
  # Standard_formulation
  
  temp <- standard_calculs(arf_sf, "SF")
  
  temp <- rbind(temp, standard_calculs(sbc, "vc"))
  
  temp <- rbind(temp, standard_calculs(sbc, "vr"))
  
  temp <- rbind(temp, standard_calculs(sbc_vc, "vc+vr"))
  
  temp <- as.data.frame(temp)
  
  data_list[["Standard_formulation"]]<- temp
  
  
  # Symetry_breaking
  
  temp <- vector()
  
  for(i in c("hc1_full", "hc2", "hc3", "cos", "qua", "cus", "lex_full"))
  {
    temp <- rbind(temp, standard_calculs(sbc, i))
  }
  
  temp <- as.data.frame(temp)
  
  data_list[["Symetry_breaking"]]<- temp
  
  
  # Comparison_AFR_SF
  
  temp <- vector()
  
  temp <- rbind(standard_calculs(arf_sf, "SF", TRUE),
                standard_calculs(arf_sf, "ARF", TRUE))
  
  temp <- as.data.frame(temp)
  
  data_list[["Comparison_AFR_SF"]]<- temp
  
  
  # best case vs ARF
  
  temp <- rbind(standard_calculs(sbc, "vc"),
                standard_calculs(arf_sf, "ARF"))
  
  temp <- as.data.frame(temp)
  
  data_list[["ARF_vs_Best"]] <- temp
  
  
  # # Combined_results
  
  temp <- vector()
  name_1 <- names(sbc)[str_detect(names(sbc), "vc")]

  for(i in name_1)
  {
    temp <- rbind(temp, standard_calculs(sbc, i))
  }

  name_2 <- names(sbc_vc)

  for(i in name_2)
  {
    temp <- rbind(temp, standard_calculs(sbc_vc, i))
  }

  temp <- as.data.frame(temp)

  data_list[["Combined_results"]] <- temp

  
  # Sub_ARF_SF
  
  temp_1 <- vector()
  temp <- vector()
  for(i in 2:5)
  {
    for(j in c("ARF", "SF"))
    {
      for(k in c(3, 6))
      {
        for(l in c("high", "low"))
        {
          if(k == 3)
          {
            for(s in seq(from = 5, to = 50, by = 5))
            {
              temp <- rbind(temp, subset_calculs(arf_sf, j, k, l, s, vehicles = i))
            }
          }
          
          else
          {
            for(s in seq(from = 5, to = 30, by = 5))
            {
              temp <- rbind(temp, subset_calculs(arf_sf, j, k, l, s, vehicles = i))
            }
          }
        }
      }
      
      temp_1 <- cbind(temp_1, temp)
      temp <- vector()
    }
  }
  
  temp_1 <- as.data.frame(temp_1)
  
  data_list[["Sub_ARF_SF"]] <- temp_1
  
  
  # ARF_SF_Opt
  
  temp <- c("SF", "vr", "hc1_full", "hc2", "hc3", "cos", "qua",
            "cus", "lex_full" )
  
  temp_1 <- vector()
  temp_2 <- rep(0, 15)
  
  for(i in 2:5)
  {
    for(j in temp)
    {
      for(k in 1:15)
      {
        if(j == "SF")
          temp_2[k] <- subset_calculs(arf_sf, "SF", ord = k, vehicles = i,
                                   col = "Opt")
        else
          temp_2[k] <- subset_calculs(sbc, j, ord = k, vehicles = i,
                                   col = "Opt")
      }
      
      temp_1 <- cbind(temp_1, temp_2)
      temp_2 <- vector()
    }
  }
  
  temp_1 <- as.data.frame(temp_1)
  
  data_list[["ARF_SF_Opt"]] <- temp_1
  
  
  # ARF_SF_Unsolved
  
  temp_2 <- rep(0, 15)
  temp_1 <- vector()
  
  for(i in 2:5)
  {
    for(j in temp)
    {
      for(k in 1:15)
      {
        if(j == "SF")
          temp_2[k] <- subset_calculs(arf_sf, "SF", ord = k, vehicles = i,
                                   col = "Unsolved")
        else
          temp_2[k] <- subset_calculs(sbc, j, ord = k, vehicles = i,
                                   col = "Unsolved")
      }
      
      temp_1 <- cbind(temp_1, temp_2)
      temp_2 <- vector()
    }
  }
  
  temp_1 <- as.data.frame(temp_1)
  
  data_list[["ARF_SF_Unsolved"]] <- temp_1
  
  
  # Average_High_Low
  
  temp_1 <- vector()
  
  temp_2 <- vector()
  
  for(i in temp)
  {
    for(j in 2:5)
    {
      for(k in c("high", "low"))
      {
        if(i == "SF")
          temp_2 <- append(temp_2, subset_calculs(arf_sf, "SF", inv = k,
                                               vehicles = j))
        else
          temp_2 <- append(temp_2, subset_calculs(sbc, i, inv = k,
                                               vehicles = j))
      }
    }
    
    temp_1 <- rbind(temp_1, temp_2)
    temp_2 <- vector()
  }
  
  temp_1 <- as.data.frame(temp_1)
  
  data_list[["Average_High_Low"]] <- temp_1
  
  
  # Best_vs_ARF_Inventory
  
  temp <- vector()
  temp_1 <- vector()
  
  for(i in 1:15)
  {
    temp <- rbind(temp, compare_calculs(sbc, "vr", i))
    temp_1 <- rbind(temp_1, compare_calculs(arf_sf, "ARF", i))
  }
  
  temp <- cbind(temp, temp_1)
  
  temp <- as.data.frame(temp)
  
  data_list[["Best_vs_ARF_Inventory"]] <- temp
  
  data_list
}

### make_tables(liste, arf_sf, sbc, sbc_vc)
###
## make_tables: Create excel file with all calclus and titles in diffent sheet
##
## Argument (liste) : List of all calculs we made
##
## Output : Excel file
##
## Example : make_tables(my_calculs, arf, sbc, sbc_hc1)
##

make_tables <- function(liste, arf_sf, sbc, sbc_vc)
{
  
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
  
  temp <- rbind(title_2, title_3 ,liste[["Standard_formulation"]])
  
  temp <- cbind(c(rep("", 2), title_4), temp)
  
  names(temp) <- title_1
  
  data_list[["Standard_formulation"]] <- temp
  
  # SBC
  
  temp <- rbind(title_2, title_3 ,liste[["Symetry_breaking"]])
  
  temp <- cbind(c(rep("", 2), title_5), temp)
  
  names(temp) <- title_1
  
  data_list[["Symetry_breaking"]]<- temp
  
  
  # Conparaison of the reults obtained with ARF vs SF
  
  temp <- rbind(rep(c("#Opt", "#Unsolved"), 4), liste[["Comparison_AFR_SF"]])
  
  temp <- cbind(c("", "SF", "ARF"), temp)
  
  names(temp) <- c("", "2 Vehicles", "", "3 Vehicles","", "4 Vehicles","",
                   "5 Vehicles", "")
  
  data_list[["Comparison_AFR_SF"]] <- temp
  
  # Combined VC result
  
  temp <- vector()
  
  name_1 <- names(sbc)[str_detect(names(sbc), "vc")]
  
  name_2 <- names(sbc_vc)
  
  temp <- rbind(title_2, title_3, liste[["Combined_results"]])
  
  temp <- cbind(c("", "", name_1, name_2), temp)
  
  names(temp) <- title_1
  
  data_list[["Combined_results"]] <- temp
  
  
  ## ARF vs VC
  
  temp <- vector()
  
  temp <- rbind(title_2, title_3, liste[["ARF_vs_Best"]])
  
  temp <- cbind(c("", "", "VC", "ARF"), temp)
  
  names(temp) <- title_1
  
  data_list[["ARF vs VC"]] <- temp
  
  # Comparison Sub_ARF_SF
  
  title_6 <- c("H", 3, rep("", 9), 3, rep("", 9), 6, rep("", 5), 6, rep("",5))
  
  title_7 <- c("Inventory", "High", rep("", 9), "Low", rep("", 9),
               "High", rep("", 5), "Low", rep("",5))
  
  title_8 <- c("n", rep(seq(from = 5, to = 50, by = 5), 2),
               rep(seq(from = 5, to = 30, by = 5), 2))
  
  title_9 <- c(rep("", 3), rep(c("ARF", rep("", 3), "SF", rep("", 3)), 4))
  
  temp <- rbind(rep(c("Gap", "%Opt", "%Unsolved", "time"), 8),
                liste[["Sub_ARF_SF"]])
  
  temp <- cbind(title_6, title_7, title_8, temp)
  
  names(temp) <- title_9
  
  data_list[["Sub_ARF_SF"]] <- temp
  
  
  # Number Optimal solution
  
  
  title_10 <- c("", "2 Vehicles", rep("", 8), "3 Vehicles", rep("", 8),
                "4 Vehicles", rep("", 8), "5 Vehicles", rep("", 8))
  
  
  temp_1 <- cbind(1:15, liste[["ARF_SF_Opt"]])
  
  temp_1 <- rbind(c("Ordering", rep(c("SF", "vr", title_5), 4)), temp_1)
  
  names(temp_1) <- title_10
  
  data_list[["ARF_SF_Opt"]] <- temp_1
  
  
  # Number of Unsolved
  
  temp_1 <- cbind(1:15, liste[["ARF_SF_Unsolved"]])
  
  temp_1 <- rbind(c("Ordering", rep(c("SF", "vr", title_5), 4)), temp_1)
  
  names(temp_1) <- title_10
  
  data_list[["ARF_SF_Unsolved"]] <- temp_1
  
  
  # High and Low : Average and sum
  
  title_11 <- c("", rep(c("Gap", "Time", "#Opt", "#Unsolver"), 8))
  
  title_12 <- c("", rep(c("Avrage", "", "Sum", ""), 8))
  
  title_13 <- c("", rep(c("High inventory cost", rep("", 3),
                          "Low inventory cost", rep("", 3)), 4))
  
  title_14 <- c("", "2 Vehicles", rep("", 7), "3 Vehicles", rep("", 7),
                "4 Vehicles", rep("", 7), "5 Vehicles", rep("", 7))
  
  temp_1 <- cbind(c("SF", "vr",title_5), liste[["Average_High_Low"]])
  
  temp_1 <- rbind(title_13, title_12, title_11, temp_1)
  
  names(temp_1) <- title_14
  
  data_list[["Average_High_Low"]] <- temp_1
  
  
  # Best case and ARF compare High and Low
  
  title_15 <- c("Ordering", rep(rep(c("High", "Low"), 8), 2))
  
  title_16 <- c("", rep(rep(c("#Opt", "", "#Unsolved", ""), 4), 2))
  
  title_17 <- c("", rep(c("2 Vehicles", rep("", 3), "3 Vehicles", rep("", 3),
                          "4 Vehicles", rep("", 3), "5 Vehicles", rep("", 3)),2))
  
  title_19 <- c("", "VC", rep("", 15), "ARF", rep("", 15))
  
  temp_1 <- cbind(1:15, liste[["Best_vs_ARF_Inventory"]])
  
  temp_1 <- rbind(title_17, title_16, title_15, temp_1)
  
  names(temp_1) <- title_19
  
  data_list[["Best_vs_ARF_Inventory"]] <- temp_1
  
  return (write_xlsx(data_list, "result.xlsx"))
  
}

# Importing the data, calculs and creating the excel file

sbc <- importFile("SBC.xlsx")
arf_sf <- importFile("ARF - SF.xlsx")
sbc_vc <- importFile("SBC - VC.xlsx")



#my_calculs <- make_calculs(arf_sf, sbc, sbc_vc)
#write_xlsx(my_calculs, "calculs.xlsx")
#write_xlsx(arf_sf, "1.xlsx")
#write_xlsx(sbc, "2.xlsx")
#write_xlsx(sbc_vc, "3.xlsx")
#make_tables(my_calculs, arf_sf, sbc, sbc_vc)

