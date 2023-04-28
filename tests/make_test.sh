
# generate data for test

cd tests
rm *fastq*

for bc in AA AA AC AG
do
    for i in $(seq 1 10)
    do
        echo "@${bc}$i" >> R1.fastq
        echo "AATTAA" >> R1.fastq
        echo "+" >> R1.fastq
        echo "!!!!!!" >> R1.fastq
        echo "@${bc}$i" >> R2.fastq
        echo "NN${bc}AA" >> R2.fastq
        echo "+" >> R2.fastq
        echo "!!!!!!" >> R2.fastq
    done
done
gzip R1.fastq
gzip R2.fastq

../demuxator --bc-start 3 --bc-length 2 R1.fastq.gz R2.fastq.gz
