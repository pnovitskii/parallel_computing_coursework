#pragma once

class ProgressBar {
public:
	ProgressBar(int total, int interval) : total(total){
		updateInterval = total / interval;
	}
	void update() {
		++processed;

		// Update progress every updateInterval files
		if (processed % updateInterval == 0 || processed == total) {
			float progress = static_cast<float>(processed) / total * 100.0;
			std::cout << "Progress: " << progress << "% (" << processed << "/" << total << " files)\n";
		}
	}
private:
	size_t total;
	size_t processed = 0;
	size_t updateInterval;
};
