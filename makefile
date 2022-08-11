redirect_only_once: redirect_only_once.cpp
	g++ -g -Wall -std=c++11 redirect_only_once.cpp -o redirect_only_once

redirect_many_times: redirect_many_times.cpp
	g++ -g -Wall -std=c++11 redirect_many_times.cpp -o redirect_many_times

clean:
	rm -rf redirect_many_times redirect_only_once
