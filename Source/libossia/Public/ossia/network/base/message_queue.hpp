#pragma once
#include <ossia/detail/ptr_set.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/parameter.hpp>

#include <concurrentqueue.h>

namespace ossia
{
struct received_value
{
  ossia::net::parameter_base* address{};
  ossia::value value;
};

class message_queue final : public Nano::Observer
{
public:
  ossia::net::device_base& device;
  message_queue(ossia::net::device_base& dev) : device{dev}
  {
    dev.on_parameter_removing.connect<&message_queue::on_param_removed>(*this);
  }

  ~message_queue()
  {
    try
    {
      for (auto reg : m_reg)
      {
        reg.first->remove_callback(reg.second.second);
      }
    }
    catch (...)
    {
    }
  }

  bool try_dequeue(ossia::received_value& v)
  {
    return m_queue.try_dequeue(v);
  }

  void reg(ossia::net::parameter_base& p)
  {
    auto ptr = &p;
    auto reg_it = m_reg.find(&p);
    if (reg_it == m_reg.end())
    {
      auto it = p.add_callback([this, ptr](const ossia::value& val) {
        m_queue.enqueue({ptr, val});
      });
      m_reg.insert({&p, {0, it}});
    }
    else
    {
      reg_it.value().first++;
    }
  }

  void unreg(ossia::net::parameter_base& p)
  {
    auto it = m_reg.find(&p);
    if (it != m_reg.end())
    {
      it.value().first--;
      if(it.value().first <= 0)
      {
        p.remove_callback(it->second.second);
        m_reg.erase(it);
      }
    }
  }

private:
  void on_param_removed(const ossia::net::parameter_base& p)
  {
    auto it = m_reg.find(const_cast<ossia::net::parameter_base*>(&p));
    if (it != m_reg.end())
      m_reg.erase(it);
  }

  moodycamel::ConcurrentQueue<received_value> m_queue;

  ossia::ptr_map<
      ossia::net::parameter_base*,
        std::pair<int, ossia::net::parameter_base::callback_index>>
      m_reg;
};

class global_message_queue final : public Nano::Observer
{
public:
  global_message_queue(ossia::net::device_base& dev)
  {
    dev.on_message.connect<&global_message_queue::on_message>(*this);
  }

  void on_message(const ossia::net::parameter_base& p)
  {
    m_queue.enqueue({const_cast<ossia::net::parameter_base*>(&p), p.value()});
  }

  bool try_dequeue(ossia::received_value& v)
  {
    return m_queue.try_dequeue(v);
  }

private:
  moodycamel::ConcurrentQueue<received_value> m_queue;
};
}
