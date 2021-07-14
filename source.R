library("readxl")
setwd("~/Google drive/data_analysis")

importData <- function(x)
{
                        
  if(x == "SBC.xlsx")
    sheet <- tail(excel_sheets(x), 13)
  
  else if(grepl("ARF", x))
    sheet <- excel_sheets(x)
  
  else
    sheet <- tail(excel_sheets(x), 12)
    
  
  data_list <- list()
  
  for(i in 1:length(sheet))
  {
    data <- read_excel(x, sheet = sheet[i])
    
    data <- data[2:2561, c(1, 2, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 
                           20, 21, 28, 29, 30, 31, 32, 33, 40, 41,
                           42, 43, 44, 45, 52, 53)]
    
    names(data)[1:4] <- c("H", "Inventory", "Ordering", "n")
    
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
  
  names(data_list) <- sheet
  data_list
}

