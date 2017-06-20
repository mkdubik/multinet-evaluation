# multinet-evaluation

This is the repository that contains the code  and data for the experiments that are included in the paper "A comparative evaluation of state-of-the-art community detection algorithms for multiplex networks". The multinet library fork that is compatible with this project is here[https://github.com/mkdubik/multinet].

To aggregate cpu and memory statistics, DTrace or SystemTap is required. An example systemtap script is included in telemetry/telemetry.stp

Clone and build the multinet library:

```
git clone https://github.com/mkdubik/multinet
cmake . -Ddtrace=on
make
```

Clone and build this project:

```
git clone https://github.com/mkdubik/multinet-evaluation
cmake .
make
```
Run the experiments:

```
./performance
./accuracy
./dk
```
