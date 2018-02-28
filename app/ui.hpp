/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_UI_HPP_NOVEMBER_11_2017)
#define CYCFI_INFINITY_UI_HPP_NOVEMBER_11_2017

#include <inf/i2c.hpp>
#include <inf/canvas.hpp>
#include <inf/ssd1306.hpp>
#include <inf/rotary_encoder.hpp>
#include <memory>

namespace cycfi { namespace infinity
{
   ////////////////////////////////////////////////////////////////////////////
   // Infinity UI
   ////////////////////////////////////////////////////////////////////////////
   class ui
   {
      static constexpr auto portb = port::portb;
      static constexpr auto portc = port::portc;
      static constexpr auto pull_up = port::pull_up;
      static constexpr auto rising_edge = port::rising_edge;
      static constexpr auto medium_font = monochrome::font::medium;

      using canvas_type = mono_canvas<128, 32>;
      using i2c_type = i2c_master<portb+10, portb+3>;
      using oled_type = ssd1306<i2c_type, canvas_type>;
      using oled_type_ptr = std::unique_ptr<oled_type>;
      using encoder_type = rotary_encoder<portc+8, portc+9, 24 /*steps*/>;
      using mode_button_type = input_pin<portc + 10, pull_up>; //main_button_type;

   public:

      using param_type = encoder_param<encoder_type>;

      enum class mode_enum : char
      {
         level,
         cutoff,
         delay
      };

      auto              setup();
      void              start();
      void              refresh();
      float             level() const;
      float             cutoff() const;
      float             delay() const;

   private:

      void              set_mode();
      void              display(char const* str, int val, int frac);

      i2c_type          i2c;
      encoder_type      enc;
      mode_button_type  mode_btn;
      oled_type_ptr     cnv;
      mode_enum         mode = mode_enum::level;

      param_type        level_enc   { enc, 0.1, 0, 1, 0.001 };
      param_type        cutoff_enc  { enc, 500, 100, 3900, 0.1 };
      param_type        delay_enc  { enc, 0, 0, 1024, 1 };

      std::uint32_t     time = 0;
   };

   ///////////////////////////////////////////////////////////////////////////////
   // Infinity UI Implementation

   auto ui::setup()
   {
      auto cfg_i2c = i2c.setup();
      auto cfg_enc = enc.setup(2);
      auto cfg_mode_btn = mode_btn.setup([this]{ set_mode(); }, 3);
      return [cfg_i2c, cfg_enc, cfg_mode_btn](auto base)
      {
         return cfg_i2c(cfg_enc(cfg_mode_btn(base)));
      };
   }

   void ui::start()
   {
      cnv = std::make_unique<oled_type>(i2c);
      enc.start();
      mode_btn.start(port::falling_edge);  // call button_task on the rising edge

      delay_enc.activate();
      cutoff_enc.activate();
      level_enc.activate();
   }

   void ui::refresh()
   {
      switch (mode)
      {
         case mode_enum::level:
            display("Level", std::round(level_enc() * 1000.0f), 2);
            break;
         case mode_enum::cutoff:
            display("Cutoff", std::round(cutoff_enc() * 10.0f), 1);
            break;
         case mode_enum::delay:
            display("Delay", std::round(delay_enc() * 10.0f), 1);
            break;
      }
   }

   float ui::level() const
   {
      return level_enc();
   }

   float ui::cutoff() const
   {
      return cutoff_enc();
   }

   float ui::delay() const
   {
      return delay_enc();
   }

   void ui::set_mode()
   {
      if (!debounce(time, 50))
         return;

      switch (mode)
      {
         default:
         case mode_enum::level:
            level_enc.deactivate();
            cutoff_enc.activate();
            mode = mode_enum::cutoff;
            break;
         case mode_enum::cutoff:
            cutoff_enc.deactivate();
            delay_enc.activate();
            mode = mode_enum::delay;
            break;
         case mode_enum::delay:
            delay_enc.deactivate();
            level_enc.activate();
            mode = mode_enum::level;
            break;
      }
   }

   void ui::display(char const* str, int val, int frac)
   {
      cnv->clear();
      cnv->draw_string(str, 15, 8, medium_font);

      if (val < 99999)
      {
         char out[8];
         to_string(val, out, frac);
         cnv->draw_string(out, 70, 8, medium_font);
      }
      else
      {
         cnv->draw_string("over", 70, 8, medium_font);
      }

      cnv->refresh();
   }
}}

#endif
