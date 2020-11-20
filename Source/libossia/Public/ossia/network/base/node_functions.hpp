#pragma once
#include <ossia/network/base/address_scope.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/value/destination.hpp>

/**
 * \file node_functions.hpp
 *
 * Various utilities to work with a node hierarchy.
 */
namespace ossia
{
namespace net
{

/**
 * @brief Find a node in a device.
 *
 * The address is in OSC format, e.g. /foo/bar.
 *
 * @return null if the node was not found.
 */
OSSIA_EXPORT ossia::net::node_base*
find_node(node_base& dev, ossia::string_view parameter_base);

/**
 * @brief Find all nodes matching a pattern in a device
 *
 * The address is in OSC pattern match format, e.g. /foo.[0-5]/bar*
 *
 * @note If the pattern is known beforehand and may be reused,
 * prefer storing a traversal::path and using the functions in path.hpp.
 */
OSSIA_EXPORT std::vector<ossia::net::node_base*>
find_nodes(node_base& dev, ossia::string_view pattern);

/**
 * @brief Create a node in a device.
 *
 * If the node already exists, a new instance will be created.
 * Hence there is no guarantee that the created node name is the same
 * than the one requested; the output should be checked.
 */
OSSIA_EXPORT node_base&
create_node(node_base& dev, ossia::string_view parameter_base);

/**
 * @brief Create nodes according to a brace-expansion-like mechanism
 *
 * Only [] and { } are possible. e.g.
 * /foo/{bar,baz}.[0-9][0-9].
 *
 * \see ossia::net::is_brace_expansion
 */
OSSIA_EXPORT std::vector<ossia::net::node_base*>
create_nodes(node_base& dev, ossia::string_view pattern);

/**
 * @brief Find a node and create it if it does not exist.
 *
 * If the node exists, it will be returned, else a new node will be created.
 */
OSSIA_EXPORT node_base&
find_or_create_node(node_base& dev, ossia::string_view parameter_base);

/**
 * @brief Find a parameter and create it if it does not exist.
 * @details Find a node matching the address, if it already has a parameter
 * create a new one (with name incremented by 1), if not create parameter
 * It some matching nodes does not exist, they will be created with a parameter
 * @param node
 * @param address
 * @param type of the parameter
 * @return vector of created parameters
 */
OSSIA_EXPORT std::vector<parameter_base*> find_or_create_parameter(
    node_base& node, const std::string& address, const std::string& type);

/**
 * @brief Calls find_node or create_node according to the value `create`
 */
OSSIA_EXPORT node_base* find_or_create_node(
    node_base& dev, ossia::string_view parameter_base, bool create);

//! Get a valid name for a given node
void sanitize_name(std::string& name, const node_base::children_t& brethren);

/*!
 * \brief Get the textual address of a node : aDevice:/an/address
 */
OSSIA_EXPORT std::string
address_string_from_node(const ossia::net::node_base&);
OSSIA_EXPORT std::string
address_string_from_node(const ossia::net::parameter_base&);

/**
 * @brief Get the address of a node relative to parent
 */
OSSIA_EXPORT std::string relative_address_string_from_nodes(
    const ossia::net::node_base& node, const ossia::net::node_base& parent);

/**
 * @brief Get the OSC address of a node: /an/address
 */
OSSIA_EXPORT std::string
osc_parameter_string(const ossia::net::parameter_base&);
OSSIA_EXPORT std::string osc_parameter_string(const ossia::net::node_base&);

/**
 * @brief Get the OSC address of a node with the leading device
 *
 * e.g. /aDevice/an/address
 */
OSSIA_EXPORT std::string
osc_parameter_string_with_device(const ossia::net::parameter_base&);
OSSIA_EXPORT std::string
osc_parameter_string_with_device(const ossia::net::node_base&);

/**
 * @brief Get the value associated with a destination
 */
OSSIA_EXPORT ossia::value_with_unit get_value(const ossia::destination& addr);

/**
 * @brief Send a value to a given destination
 */
OSSIA_EXPORT void
push_value(const ossia::destination& addr, const ossia::value_with_unit&);

template <typename Address>
auto create_parameter(ossia::net::node_base& root, std::string name)
{
  auto& node = ossia::net::create_node(root, std::move(name));
  auto addr = new Address(node);
  node.set_parameter(std::unique_ptr<Address>(addr));
  return addr;
}

template <typename Address>
auto find_or_create_parameter(
    ossia::net::node_base& root, ossia::string_view name)
{
  auto& node = ossia::net::find_or_create_node(root, std::move(name));
  if (auto p = dynamic_cast<Address*>(node.get_parameter()))
  {
    return p;
  }
  else
  {
    auto addr = new Address{node};
    node.set_parameter(std::unique_ptr<Address>(addr));
    return addr;
  }
}
OSSIA_EXPORT std::ostream&
operator<<(std::ostream&, const ossia::net::parameter_base&);

void expand_ranges(std::string& str);

/**
 * @brief list_all_child : list all child nodes recursively
 * @param node : starting point
 * @return list : vector of child sorted by name then by priority
 */
OSSIA_EXPORT
std::vector<ossia::net::node_base*>
list_all_child(ossia::net::node_base* node);
}
}
