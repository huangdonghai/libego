#ifndef _STAT_H_
#define _STAT_H_

const bool stat_ac = false;

class Stat {
public:

  static const float prior_update_count = 1.0;

  Stat () {
    reset ();
  }
  
  void reset (float prior_sample_count = Stat::prior_update_count) {
    sample_count       = prior_sample_count; // TODO 
    sample_sum         = 0.0; // TODO
    square_sample_sum  = 0.0; // TODO
  }

  void update (float sample) {
    sample_count       += 1.0;
    sample_sum         += sample;
    square_sample_sum  += sample * sample;
  }

  float update_count () const {
    return sample_count;
  }

  float mean () const { 
    return sample_sum / sample_count; 
  }

  float variance () const {
    // VX = E(X^2) - EX ^ 2
    float m = mean ();
    return square_sample_sum / sample_count - m * m;
  }

  float std_dev () const { 
    return sqrt (variance ());
  }

  float std_err () const {
    // TODO assert sample_count
    return sqrt (variance () / sample_count);
  } 

  float precision (float bias = 0.0) const {
    return 1.0 / (variance() / update_count () + bias);
  }

  static float SlowMix (const Stat& stat1, float b1, const Stat& stat2, float b2) {
    return
      (stat1.precision(b1) * stat1.mean() + stat2.precision(b2) * stat2.mean()) /
      (stat1.precision(b1) + stat2.precision(b2));
  }

  // Optimized SlowMix
  static float Mix (const Stat& stat1, float b1, const Stat& stat2, float b2) {

    float n1 = stat1.sample_count;
    float n2 = stat2.sample_count;

    float s1 = stat1.sample_sum;
    float s2 = stat2.sample_sum;

    float v1 = stat1.square_sample_sum;
    float v2 = stat2.square_sample_sum;

    float nn1 = n1 * n1;
    float nn2 = n2 * n2;

    float x1 = (v1 + b1*nn1) * n1  -  s1 * s1;
    float x2 = (v2 + b2*nn2) * n2  -  s2 * s2;

    float t1 = nn1 * x2;
    float t2 = nn2 * x1;

    float mix = (t1*s1 + t2*s2) / (t1*n1 + t2*n2);

    assertc (stat_ac, fabs(mix - SlowMix(stat1, b1, stat2 ,b2)) < 0.00001);
    return mix;
  }


  string to_string (float minimal_update_count = 0.0) const {
    if (sample_count < minimal_update_count) return "           ";

    ostringstream out;
    char buf [100];
    sprintf (buf, "%+3.3f(%5.0f)", mean(), update_count());
    out << buf;
    return out.str ();
  }

private:
  float sample_count;
  float sample_sum;
  float square_sample_sum;
};

#endif
