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
      using mode_button_type = input_pin<portc + 10, pull_up>;

   public:

      using param_type = encoder_param<encoder_type>;

      enum class mode_enum : char
      {
         level,
         phase,
         factor
      };

      auto              setup();
      void              start();
      void              refresh();
      float             level() const;

   private:

      void              set_mode();
      void              display(char const* str, int val, int frac);

      i2c_type          i2c;
      encoder_type      enc;
      mode_button_type  mode_btn;
      oled_type_ptr     cnv;
      mode_enum         mode = mode_enum::level;

      param_type        level_enc   { enc, 0.5, 0, 1, 0.001 };
      param_type        phase_enc   { enc, 0, -1, 2, 0.0001 };
      param_type        factor_enc  { enc, 0, 0, 8, 0.001 };
   };

   ///////////////////////////////////////////////////////////////////////////////
   // Infinity UI Implementation

   auto ui::setup()
   {
      auto cfg_i2c = i2c.setup();
      auto cfg_enc = enc.setup();
      auto cfg_mode_btn = mode_btn.setup([this]{ set_mode(); } , 10);
      return [cfg_i2c, cfg_enc, cfg_mode_btn](auto base)
      {
         return cfg_i2c(cfg_enc(cfg_mode_btn(base)));
      };
   }

   void ui::start()
   {
      cnv = std::make_unique<oled_type>(i2c);
      enc.start();
      mode_btn.start(rising_edge);  // call button_task on the rising edge

      factor_enc.activate();
      phase_enc.activate();
      level_enc.activate();
   }

   void ui::refresh()
   {
      switch (mode)
      {
         case mode_enum::level:
            display("Level", std::round(level_enc() * 1000.0f), 2);
            break;
         case mode_enum::phase:
            display("Phase", std::round(phase_enc() * 1800.0f), 1);
            break;
         case mode_enum::factor:
            display("---", std::round(factor_enc() * 1000.0f), 2);
            break;
      }
   }

   float ui::level() const
   {
      return level_enc() / 8;
   }

   void ui::set_mode()
   {
      switch (mode)
      {
         default:
         case mode_enum::level:
            level_enc.deactivate();
            phase_enc.activate();
            mode = mode_enum::phase;
            break;
         case mode_enum::phase:
            phase_enc.deactivate();
            factor_enc.activate();
            mode = mode_enum::factor;
            break;
         case mode_enum::factor:
            factor_enc.deactivate();
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
