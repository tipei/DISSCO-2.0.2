#include "ProbabilityEnvelope.h"
#include <cmath>

m_value_type lerp(m_value_type a, m_value_type b, double c) {
  return (1-c) * a + c * b;
}

ProbabilityEnvelope::ProbabilityEnvelope() {
  setDuration(1.0);
}

ProbabilityEnvelope::ProbabilityEnvelope(const Envelope& other): Envelope(other) {
  setDuration(1.0);
}

m_value_type ProbabilityEnvelope::estimateAreaUnderCurve(int num_steps) {
  // set the length to 1 second and sample rate to num_steps
  // will force the iterator to sample num_steps times
  setSamplingRate(num_steps);
  Iterator<m_value_type> it = valueIterator();
  m_value_type auc = 0;
  while (it.hasNext()) {
    auc += it.next();
  }
  return auc;
}

void ProbabilityEnvelope::generateCountTable(int num_steps) {
  stepCounts.resize(0);
  stepTimes.resize(0);

  setSamplingRate(num_steps);
  m_value_type x_step_size = 1.0 / num_steps;
  Iterator<m_value_type> it = valueIterator();

  totalCounts = 0;
  for (size_t i = 0; it.hasNext(); i++) {
    stepTimes.push_back(i * x_step_size);
    m_value_type count = it.next();
    stepCounts.push_back(count);
    totalCounts += count;
    cumulativeStepCounts.push_back(totalCounts);
  }
}

// TODO: throw uninitialized error
m_value_type ProbabilityEnvelope::sample(double x) {
  m_value_type target = totalCounts * x;
  // now search for target in cumulativeStepCounts to find the time index
  size_t upper_idx = std::upper_bound(cumulativeStepCounts.begin(), cumulativeStepCounts.end(), target)
    - cumulativeStepCounts.begin();

  // the value passed in is close to or over 1
  if (upper_idx == stepCounts.size())
    return 1.0;

  // the value passed in is close to or under 0
  if (upper_idx == 0)
    return 0.0;

  double ratio = (target - cumulativeStepCounts[upper_idx-1]) /
    (cumulativeStepCounts[upper_idx] - cumulativeStepCounts[upper_idx-1]);
  return lerp(stepTimes[upper_idx-1], stepTimes[upper_idx], ratio);
}


void ProbabilityEnvelope::print() {
  for (size_t i = 0; i < stepCounts.size(); i++) {
    cout << stepTimes[i] << " " << stepCounts[i] << " " << cumulativeStepCounts[i] << endl;
  }
}
