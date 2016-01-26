S-gui: gui for the S benchmark suite

This application allows you to execute, through a graphical interface,
the following two benchmarks of the S benchmark suite:

1) Aggregated throughput 
2) Application start-up time
See below for more details

This package includes also the S benchmark suite itself. If you want
to check the last version of S, then go to:"link"

INSTALLATION
1) Download .zip package or clone repo
2) Unzip if needed
3) Enter S-Gui folder and execute:
   ./install.sh
4) Solve possible issues reported by install.sh

USAGE
As of now it can be executed only from a terminal, as follows:
1) Enter S-Gui folder and execute:
   sudo -E ./S

AGGREGATED THROUGHPUT
	Measures the aggregated throughput with parallel, greedy (i.e.,
	continuously issuing disk requests readers and/or writers. Both
	readers and writers are implemented with fio, and may be sequential
	or random.
	Files are read from and written to the /tmp/test directory (created
	if non existing).
	This type of workload is also used as background in the other tests.
	At the end of the test, the min, max, avg, std deviation,
	confidence interval of the read/write/total aggregated throughput
	values sampled during the run is reported.

APPLICATION START-UP TIME
	Measures the cold-cache startup latency of the given command,
	launched while the same configurable workload used in the agg_thr
	test is running. At the end of the test reports min, max, avg, std
	dev and conf interval of both the sampled latencies and the read/
	write/total aggregated throughput.


TODO:
1)Handle closing of test windows: as of now, when one of these windows is closed,
the related data structure is not deallocated.
2)Check and, if needed, fix build of S.desktop

