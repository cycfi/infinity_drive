/*=============================================================================
   Copyright (c) 2014-2017 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_INFINITY_CONFIG_HPP_SEPTEMBER_11_2017)
#define CYCFI_INFINITY_CONFIG_HPP_SEPTEMBER_11_2017

#include <type_traits>
#include <inf/support.hpp>

namespace cycfi { namespace infinity
{
   ///////////////////////////////////////////////////////////////////////////////
   // Application configuration makes sure there are no peripheral conflicts 
   // and attaches IRQ tasks, if there are any. The config variable should
   // be a global variable in global scope.
   //
   // Example:
   //
   //    auto config = inf::config(
   //       tmr = timer_task
   //    );
   //
   ///////////////////////////////////////////////////////////////////////////////

   struct irq_not_handled {};
   struct peripheral_base_id {};
   
   struct peripheral_base
   {
      using id = peripheral_base_id;
      irq_not_handled operator()(...)
      {
         return irq_not_handled{};
      }
   };

   template <typename T>
   constexpr bool has_unique_id()
   {
      return has_unique_id<typename T::base_type>() && 
         !std::is_same<typename T::id, typename T::base_type::id>::value
         ;
   }

   template <>
   constexpr bool has_unique_id<peripheral_base>()
   {
      return true;
   }

   template <typename ID, typename Base>
   struct basic_config : Base
   {
      using id = ID;
      using base_type = Base;
      using Base::operator();

      basic_config(Base base)
       : Base(base)
      {}
   };

   template <typename ID, typename Base, typename F>
   struct task_config : Base
   {
      using id = ID;
      using base_type = Base;
      using Base::operator();

      task_config(Base base, F task)
       : Base(base)
       , task(task)
      {}

      void operator()(identity<ID> const&)
      {
         task();
      }

      F task;
   };

   template <typename T>
   inline auto config(T cfg)
   {
      return cfg(peripheral_base{});
   }

   template <typename T, typename... Rest>
   inline auto config(T cfg, Rest... rest)
   {
      auto r = cfg(config(rest...));
      static_assert(has_unique_id<decltype(r)>(), "Error. Duplicate peripheral IDs.");
      return r;
   }
}} 

#endif
