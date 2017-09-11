/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_CONFIG_HPP_SEPTEMBER_11_2017)
#define CYCFI_INFINITY_CONFIG_HPP_SEPTEMBER_11_2017

namespace cycfi { namespace infinity
{
   struct irq_not_handled {};

   struct peripheral_base
   {
      irq_not_handled operator()(...)
      {
         return irq_not_handled{};
      }
   };

   template <typename... T>
   struct peripheral : T...
   {
      peripheral(T... cfg)
       : T(cfg)...
      {}
   };

   template <typename... T>
   inline peripheral<T...> config(T... cfg)
   {
      return peripheral<T...>(cfg.config()...);
   }
}} 

#endif
