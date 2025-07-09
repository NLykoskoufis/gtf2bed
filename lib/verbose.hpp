#ifndef _VERBOSE_H
#define _VERBOSE_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

class verbose {
protected:
	ofstream log;
	bool verbose_on_screen;
	bool verbose_on_log;
	int prev_percent;

public:
	verbose() {
		verbose_on_screen = true;
		verbose_on_log = false;
		prev_percent = 0;
	}

	~verbose() {
		close_log();
	}

	bool open_log(string fname) {
		log.open(fname.c_str());
		if (log.fail()) return false;
		else return (verbose_on_log = true);
	}

	void close_log() {
		log.close();
	}

	void set_silent() {
		verbose_on_screen = false;
	}

	void print(string s) {
		if (verbose_on_screen) cout << s << endl;
		if (verbose_on_log) log << s << endl;
	}

	void ctitle(string s) {
		if (verbose_on_screen) cout << endl << "\x1B[32m" << s <<  "\033[0m" << endl;
		if (verbose_on_log) log << endl << s << endl;
	}

	void title(string s) {
		if (verbose_on_screen) cout << endl << s << endl;
		if (verbose_on_log) log << endl << s << endl;
	}

	void bullet(string s) {
		if (verbose_on_screen) cout << "  * " << s << endl;
		if (verbose_on_log) log << "  * " << s << endl;
	}

	void warning(string s) {
		if (verbose_on_screen) cout << endl << "\x1B[33m" << "WARNING: " <<  "\033[0m" << s << endl;
		if (verbose_on_log) log << endl << "WARNING: " << s << endl;
	}

	void leave(string s) {
		if (verbose_on_screen) cout << endl << "\x1B[33m" << "EXITED: " <<  "\033[0m" << s << endl;
		if (verbose_on_log) log << endl << "EXITED: " << s << endl;
		exit(EXIT_SUCCESS);
	}

	void error(string s) {
		if (verbose_on_screen) cout << endl << "\x1B[31m" << "ERROR: " <<  "\033[0m" << s << endl;
		if (verbose_on_log) log << endl << "ERROR: " << s << endl;
		exit(EXIT_FAILURE);
	}

	void done(string s) {
		if (verbose_on_screen) cout << endl << "\x1B[32m" << "DONE: " <<  "\033[0m" << s << endl;
		if (verbose_on_log) log << endl << "DONE: " << s << endl;
		exit(EXIT_SUCCESS);
	}

	void progress(float percent, int barWidth = 70) {
		if (verbose_on_screen) {
			int curr_percent = int(percent * 100.0);
			if (prev_percent > curr_percent) prev_percent = -1;
			if (curr_percent > prev_percent) {
				int pos = barWidth * percent;
				cout << "[";
				for (int i = 0; i < barWidth; ++i) {
					if (i < pos) cout << "=";
					else if (i == pos) cout << ">";
					else cout << " ";
				}
				cout << "] " << curr_percent << " %\r";
				if (percent < 1.0) cout.flush();
				else cout << endl;
				prev_percent = curr_percent;
			}
		}
	}
};
#endif