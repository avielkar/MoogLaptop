#include <time.h>


class Timer {
private:
	unsigned long begTime;
public:
	void start() {
		begTime = clock();
	}

	unsigned long elapsedTime() {
		return ((unsigned long)clock() - begTime) * 1000 / CLOCKS_PER_SEC;
	}

	bool isTimeout(unsigned long milliSeconds) {
		return milliSeconds >= elapsedTime();
	}
};