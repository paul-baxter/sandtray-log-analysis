# sandtray-log-analysis
Simple utility to analyse automatically generated log files from the EU FP7 DREAM project in relation to interacting user behaviour (touchscreen oriented).

Currently assumes input file formats as determined by Sandtray logfile structure.

Uses cmake:
$ mkdir build && cd build
$ cmake .. && cd ..

Usage (don't forget trailing forward slash on path!):
./SandtrayLogAnalysis "path/to/data/directory/" "output-data-filename.dat"

Context, motivation and sample output described in:
* P. Baxter, S. Matu, E. Senft, C. Costescu, J. Kennedy, D. David, T. Belpaeme (2015), [Touchscreen-Mediated Child-Robot Interactions Applied to ASD Therapy](http://newfriends2015.org/Proceedings/Contents/oral_sessions.html), 1st Int. Conf. on Social Robots in Therapy and Education, Almere, The Netherlands, October 22-23
