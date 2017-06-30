mkdir -p telemetry/t3

for N in 7000 8000 9000 10000
do
	stap telemetry/telemetry.stp -c "./synthetic t3 glouvain $N 3 03 1 1 4000" ../multinet/multinet/libmultinet.so telemetry/t3 -o telemetry/t3/glouvain_${N}_3

	stap telemetry/telemetry.stp -c "./synthetic t3 pmm $N 3 03 40 140" ../multinet/multinet/libmultinet.so telemetry/t3 -o telemetry/t3/pmm_${N}_3
done

