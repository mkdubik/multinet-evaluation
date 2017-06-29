mkdir -p telemetry/t4

for N in 1000 4000
do
	for mu in 01 02 03 04 05 06
	do
		./synthetic t4 lart $N 3 $mu 9 1 1
		./synthetic t4 glouvain $N 3 $mu 1 1 4000
		./synthetic t4 pmm $N 3 $mu 40 140
	done
done
