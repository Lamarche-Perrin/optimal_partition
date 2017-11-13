size <- 8
prob <- 3/4
output <- paste ("matrix.size=",size,".prob=",round(prob,2),".csv", sep="")

build <- function (matrix) {
    sum <- sum (matrix)
    size <- dim(matrix)[1]

    if (size > 1) {
        matrix[1:(size/2),1:(size/2)] <- sum * prob / (size*size)
        matrix[(size/2+1):size,(size/2+1):size] <- sum * prob / (size*size)
        matrix[1:(size/2),(size/2+1):size] <- sum * (1-prob) / (size*size)
        matrix[(size/2+1):size,1:(size/2)] <- sum * (1-prob) / (size*size)
    }

    if (size > 2) {
        matrix[1:(size/2),1:(size/2)] <- build(matrix[1:(size/2),1:(size/2)])
        matrix[(size/2+1):size,(size/2+1):size] <- build(matrix[(size/2+1):size,(size/2+1):size])
        matrix[1:(size/2),(size/2+1):size] <- build(matrix[1:(size/2),(size/2+1):size])
        matrix[(size/2+1):size,1:(size/2)] <- build(matrix[(size/2+1):size,1:(size/2)])
    }
    return (matrix)
}

matrix <- array (1, dim=list(size,size))
matrix <- build(matrix)
matrix <- matrix / sum(matrix)

write.table (matrix, output, col.names=FALSE, row.names=FALSE)
