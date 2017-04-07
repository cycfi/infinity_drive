/*=============================================================================
  Copyright (c) Cycfi Research, Inc.
=============================================================================*/
#if !defined(CYCFI_INFINITY_PERIOD_DETECTOR_HPP_FEBRUARY_11_2016)
#define CYCFI_INFINITY_PERIOD_DETECTOR_HPP_FEBRUARY_11_2016

#include <inf/fx.hpp>

namespace cycfi { namespace infinity
{
   template <uint32_t sps>
   struct period_trigger
   {
      static constexpr float threshold = 0.1;

      period_trigger(float cutoff = 1500.0f)
       : lp1(cutoff, sps)
       , lp2(cutoff, sps)
       , dc_blk(20.0f, sps)
      {}

      float operator()(float s)
      {
         // low-pass filter
         auto val = lp2(lp1(s));

         // dc-block
         val = dc_blk(val);

         // agc
         val = agc_(val);
         if (agc_.peak.y < threshold)
            return 0.0f;

         // peak triggers
         auto valp = peak_pos(val);    // positive peaks
         auto valn = peak_neg(-val);   // negative peaks

         // trigger conditioning
         val = (valp > 0)? valp : (valn > 0)? -valn : 0;
         val = wc(val);
         return val;
      }

      low_pass lp1;
      low_pass lp2;

      peak_trigger peak_pos;
      peak_trigger peak_neg;

      agc agc_;
      dc_block dc_blk;

      window_comparator wc;
   };

   template <uint32_t sps>
   class period_detector
   {
      enum mode_enum
      {
         setup_state,
         init1_state,
         init2_state,
         run_state,
         sleep_state
      };

      struct state_
      {
         void next_mode()
         {
            mode = static_cast<mode_enum>(mode+1);
         }

         bool val;
         mode_enum mode;
      };

   public:

      period_detector(float cutoff = 1500.0f)
       : trigger(cutoff)
       , gate(0.1, 0.2)
       , freq_lp(0.1)
       , state({0, setup_state})
       , time_base(0)
      {}

      void run()                 { state.mode = setup_state; }

      void do_setup(float s)
      {
         state.val = trigger(s) > 0;
         state.mode = init1_state;              // move to init state
      }

      void do_sleep(float s)
      {
         state.val = trigger(s) > 0;
         if (gate(trigger.agc_.peak.y) > 0.0f)  // gate is above our threshold
            state.mode = init1_state;           // wake up! move to init state
      }

      void do_init(float s)
      {
         bool val = trigger(s) > 0;
         if (val != state.val)
         {
            if (val)
            {
               rising = time_base;              // rising edge
               state.next_mode();               // move to next state
            }
            else
            {
               falling = time_base;             // falling edge
               state.next_mode();               // move to next state
            }
            state.val = val;
         }
      }

      void do_run(float s)
      {
         bool val = trigger(s) > 0;
         if (gate(trigger.agc_.peak.y) < 0.0f)
         {                                      // gate is below our threshold
            state.mode = sleep_state;           // move to sleep state
         }
         else if (val != state.val)
         {
            std::size_t diff;
            if (val)
            {
               diff = time_base-rising;         // rising edge
               rising = time_base;
            }
            else
            {
               diff = time_base-falling;        // falling edge
               falling = time_base;
            }
            state.val = val;
            freq_lp(diff);
         }
      }

      float operator()(float s)
      {
         switch (state.mode)
         {
            case setup_state:
               do_setup(s);
               break;

            case init1_state:
            case init2_state:
               do_init(s);
               break;

            case run_state:
               do_run(s);
               break;

            case sleep_state:
               do_sleep(s);
               break;
         }
         ++time_base;
         return freq_lp.y;
      }

      period_trigger<sps> trigger;
      window_comparator gate;

   private:

      low_pass freq_lp;
      state_ state;

      uint32_t time_base;
      uint32_t rising;
      uint32_t falling;
   };
}}

#endif
