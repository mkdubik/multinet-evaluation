mkdir -p telemetry/t1

for N in 1000 2000 3000 4000
do
	stap telemetry/telemetry.stp -c "./synthetic t1 lart $N 3 03 9 1 1" ../multinet/multinet/libmultinet.so telemetry/t1 -o telemetry/t1/lart_${N}_3

	stap telemetry/telemetry.stp -c "./synthetic t1 glouvain $N 3 03 1 1 4000" ../multinet/multinet/libmultinet.so telemetry/t1 -o telemetry/t1/glouvain_${N}_3

	stap telemetry/telemetry.stp -c "./synthetic t1 pmm $N 3 03 40 140" ../multinet/multinet/libmultinet.so telemetry/t1 -o telemetry/t1/pmm_${N}_3
done

