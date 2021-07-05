library("openxslx")

importData <- function(x)
{
  if(x == "ARF" || x == "SF")
  {
    data <- read_excel("ARF - SF.xlsx", sheet = x)
  }
  
  if(x != "SF" & x!= "ARF")
  {
    if(grepl("vc+", x))
    {
      data <- read_excel("SBC - VC.xlsx", sheet = paste0("vi_" , x))
    }                 
  
    if(!grepl("vc+", x))
    {
      data <- read_excel("SBC.xlsx", sheet = paste0("vi_", x))
    }
  }
  
  data <- data[2:2561, c(1, 2, 4, 6, 7, 8, 9, 16, 17, 18, 19, 
                   20, 21, 28, 29, 30, 31, 32, 33, 40, 41,
                   42, 43, 44, 45, 52, 53)]
  
  names(data)[1:3] <- c("H", "Inventory", "n")
  
  name <- c("UB", "LB", "Gap", "Time", "Opt", "Unsolved")
  
  k <- 2
  
  for(i in c(4, 10, 16, 22))
  {
    for(j in 0:5)
    {
      names(data)[i+j] <- paste0("V", k,"_",name[j+1])
    }
    
    k <- k + 1
    
  }
  
  data
  
}

