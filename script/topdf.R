# Creates a set of png files that histograms
# from a text ring dump called "all-rois-merged.txt"

doall <- function (d) {
  for (i in seq(1,dim(d)[2])) {
    t <- paste ("Ring", i, sep=" ");
    s <- paste ("[ Mean =", signif(mean(d[,i],digits=),4),
                "Stddev =", signif(sqrt(var(d[,i])),digits=4), 
                "Entries = ", length(d[,i]), "]", sep=" ");
    hist(d[,i], breaks=length(d[,i])/10,
         xlab="Energy", ylab="Occurrences", main=t, sub=s);
  }
}

d <- read.table("all-rois-merged.merged-data.txt");
pdf("all-rois-merged.pdf", onefile=TRUE);
doall(d);
dev.off();
pdf("ring-%03d.pdf", onefile=FALSE);
doall(d);
dev.off();
