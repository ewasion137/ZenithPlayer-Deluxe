#pragma once

#include <QString>
#include <vector>

namespace Audio {

std::vector<float> generateWaveformPeaks(const QString& filePath, int totalBars = 120);

} // namespace Audio