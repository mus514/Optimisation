library("openxslx")

importData <- function(x)
{
  if(x == "ARF" || x == "SF")
  {
    data <- read_excel("ARF - SF.xlsx", sheet = x)
  }
  
  if(grep("vc", x) == 1)
  {
    data <- read_excel("SBC - VC.xlsx", sheet = paste0("vi_" , x))
  }
  
  else
  {
    data <- read_excel("SBC.xlsx", sheet = paste0("vi_", x))
  }
  
  data
 
}

