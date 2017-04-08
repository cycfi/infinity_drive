/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_DRIVER_JULY_17_2016)
#define CYCFI_INFINITY_DRIVER_JULY_17_2016

#include <inf/freq_locked_synth.hpp>

namespace cycfi { namespace infinity
{
   template <typename Synth, std::uint32_t sps, std::uint32_t buffer_size>
   class infinity_driver : public freq_locked_synth<Synth, sps, buffer_size>
   {
   public:

      using base_type = freq_locked_synth<Synth, sps, buffer_size>;

      infinity_driver(Synth& synth_)
       : base_type(synth_)
       , cycles(0)
      {}

      float operator()(float s)
      {
         auto read_cycle = ((cycles & 0x8) == 0) || (this->_pd.gate.state < 0.0f);

         // Update and sync phase if we are in read cycle
         if (read_cycle)
         {
            this->update(s);
            this->sync_phase();
         }

         auto prev_ref_phase = this->ref_synth().phase();
         auto val = this->synthesize();
         auto ref_phase = this->ref_synth().phase();

         // Detect overflow (start of next cycle)
         if (ref_phase < prev_ref_phase)
            ++cycles;

         return read_cycle ? 20 : val;
      }

   private:

      std::size_t cycles; // we increase this every start of cycle
   };
}}

#endif
