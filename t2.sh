mkdir -p telemetry/t2

for N in 4000
do
	for L in 3 4 5
	do
		#stap telemetry/telemetry.stp -c "./synthetic t2 lart $N $L 01 9 1 1" ../multinet/multinet/libmultinet.so telemetry/t2 -o telemetry/t2/lart_${N}_${L}
		stap telemetry/telemetry.stp -c "./synthetic t2 glouvain $N $L 01 1 1 4000" ../multinet/multinet/libmultinet.so telemetry/t2 -o telemetry/t2/glouvain_${N}_${L}
		stap telemetry/telemetry.stp -c "./synthetic t2 pmm $N $L 01 40 140" ../multinet/multinet/libmultinet.so telemetry/t2 -o telemetry/t2/pmm_${N}_${L}
	done
done
