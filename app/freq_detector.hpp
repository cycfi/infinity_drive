

struct freq_detector
{
   static constexpr auto samples = 32;
   static constexpr auto max_deviation = 1.059463094f;  // one semi-tone
   static constexpr auto max_outliers = 32;

   static bool near(float a, float b)
   {
      return ((a > b)? q::fast_div(a, b) : q::fast_div(b, a)) < max_deviation;
   }

   float update(float f);
   {
      _acc = 0.0f;
      _oc = 0;
      return _lp(f);
   }

   float operator()(float f)
   {
      // Check if the current freq is near the median. If so, we got a hit.
      if (near(f, _lp()))
         return update();

      // Check if the current freq + _acc is near the median.
      // If so, we got a hit.
      if (near(f + _acc, _lp()))
         return update();

      // We probably got an outlier, accumulate it into _acc, but count all
      // outliers into _oc. If this goes beyond a certain threshold, then
      // the accumulated outliers divided by the count (the average) is probably
      // the new frequency.

      // Handle the case where we have too many outliers. In that case, they
      // wouldn't be outliers anymore, eh? ;)
      if (_oc++ > max_outliers)
      {
         _lp = _acc / _oc; // average
         return update();
      }

      // Accumulate f
      _acc += f;

      // Now return the previous stable frequency
      return _lp();
   }

   freq_detector& operator=(float f)
   {
      _lp = f;
      _acc = 0.0f;
      _oc = 0;
      return *this;
   }

   float _acc;
   uint32_t _oc = 0;
   q::exp_moving_average<samples> _lp;
};

