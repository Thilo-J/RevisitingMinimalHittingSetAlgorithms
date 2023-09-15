#include "zdd.h"
#include <set>
#include <tuple>

/**
 * @brief Returns the zdd node that represents the empty family Ø.
 * 
 * @return Ø
 */
zdd* empty()
{
    static zdd bottom {
        -1,
        false,
        nullptr,
        nullptr,
    };
    return &bottom;
}


/**
 * @brief Returns the zdd node that represents the unit family {Ø}.
 * 
 * @return {Ø}
 */
zdd* base()
{
    static zdd top {
        1,
        false,
        nullptr,
        nullptr,
    };
    return &top;
}


/**
 * @brief Returns the node zdd{value, left, right, 1} if it does not exists already in unique_nodes.
 * Increases the node counter and returns that node if it already exists in unique_nodes. 
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param value value of the node.
 * @param left left child of the node.
 * @param right right child of the node.
 * @return zdd{value, left, right}.
 */
zdd* get_node(ZddManager* m, int value, zdd* left, zdd* right)
{
    if (right == empty())
        return left;
    std::tuple<int,zdd*,zdd*> key = std::make_tuple(value, left, right);
    auto iter = m->unique_nodes.find(key);
    if (iter == m->unique_nodes.end())
    {
        zdd* new_node = new zdd({value, false, left, right});
        m->unique_nodes.insert({key, new_node});
        return new_node;
    }
    else
    {
        return iter->second;
    }
}

void mark_zdd_tree_rec(ZddManager* m, zdd* P)
{
    if(P == empty() || P == base())
        return;
    if(m->mark_tree_visited.find(P) != m->mark_tree_visited.end())
        return;
    m->mark_tree_visited.insert(P);
    P->marked = true;
    mark_zdd_tree_rec(m, P->left);
    mark_zdd_tree_rec(m, P->right);
}

/**
 * @brief marks all the nodes of a zdd tree.
 * 
 * @param m The ZddManager with the unique_nodes list.
 * @param P Root node of the zdd tree.
 */
void mark_zdd_tree(ZddManager* m, zdd* P)
{
    mark_zdd_tree_rec(m, P);
    m->mark_tree_visited = {};
}


/**
 * @brief Deletes all unmarked nodes in the ZddManager.
 * 
 * @param m
 */
void delete_unmarked_nodes(ZddManager* m)
{
    auto it = m->unique_nodes.begin();

    while(it != m->unique_nodes.end())
    {
        if(it->second->marked) {
            it->second->marked = false;
            it++;
        }
        else {
            delete(it->second);
            it = m->unique_nodes.erase(it);
        }
    }
}


/**
 * @brief Returns the set of subsets of P containing the element var
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param P Set of sets of ints in the form of a zdd node.
 * @param var Element that may or may not be in the sets of P
 * @return The subset of P containing the element var
 */
zdd* zdd_subset1(ZddManager* m, zdd* P, int var)
{
    if (P == empty())   return empty();
    if (P == base())    return empty();
    if (P->value < var) return empty();

    std::tuple<intptr_t, intptr_t, cache_function> cache_key = std::make_tuple((intptr_t)P, (intptr_t)var, sub1_cache);
    if(m->cache.contains(cache_key) && m->cache.get(cache_key).has_value()){
        return m->cache.get(cache_key).value();
    }

    zdd* result;
    if (P->value == var)
        result = get_node(m, var, empty(), P->right);
    else if (P->value > var)
        result = get_node(m, P->value, zdd_subset1(m, P->left, var), zdd_subset1(m, P->right, var));
    m->cache.insert(cache_key, result);
    return result;
}


/**
 * @brief Returns the set of subsets of P not containing the element var.
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param P Set of sets of ints in the form of an zdd node.
 * @param var Element that may or may not be in the sets of P.
 * @return The subset of P not containing the element var.
 */
zdd* zdd_subset0(ZddManager* m, zdd* P, int var)
{
    if (P == empty())    return empty();
    if (P == base())     return base();
    if (P->value < var)  return P;
    if (P->value == var) return P->left;

    std::tuple<intptr_t, intptr_t, cache_function> cache_key = std::make_tuple((intptr_t)P, (intptr_t)var, sub0_cache);
    if(m->cache.contains(cache_key) && m->cache.get(cache_key).has_value()){
        return m->cache.get(cache_key).value();
    }

    zdd* result = get_node(m, P->value, zdd_subset0(m, P->left, var), zdd_subset0(m, P->right, var));
    m->cache.insert(cache_key, result);
    return result;
}


/**
 * @brief Returns the set of subsets derived from P by adding element var to those subsets that
 *  did not contain it and removing element var from those subsets that contain it. 
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param P Set of sets of ints in the form of an zdd node.
 * @param var Element that may or may not be in the sets of P.
 * @return The set of subsets derived from P by adding element var to those subsets that did not contain it 
 *  and removing element var from those subsets that contain it. 
 */
zdd* zdd_change(ZddManager* m, zdd* P, int var)
{
    if (P == empty()) return empty();
    if (P == base())  return get_node(m, var, empty(), base());

    std::tuple<intptr_t, intptr_t, cache_function> cache_key = std::make_tuple((intptr_t)P, (intptr_t)var, change_cache);
    if(m->cache.contains(cache_key) && m->cache.get(cache_key).has_value()){
        return m->cache.get(cache_key).value();
    }

    zdd* result;
    if (P->value < var)
        result = get_node (m, var, empty(), P);
    else if (P->value == var)
        result =  get_node (m, var, P->right, P->left);
    else if (P->value > var)
        result =  get_node(m, P->value, zdd_change(m, P->left, var), zdd_change(m, P->right, var));
    m->cache.insert(cache_key, result);
    return result;
}

/**
 * @brief Union between two Zdd nodes that each represent a set of sets.
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param P First zdd nodes that represents a set of sets.
 * @param Q Second zdd nodes that represents a set of sets.
 * @return P U Q 
 */
zdd* zdd_union(ZddManager* m, zdd* P, zdd* Q)
{
    if (P == empty()) return Q; 
    if (Q == empty()) return P;
    if (P == Q)       return P;
    
    std::tuple<intptr_t, intptr_t, cache_function> cache_key = std::make_tuple((intptr_t)P, (intptr_t)Q, union_cache);
    if(m->cache.contains(cache_key) && m->cache.get(cache_key).has_value()){
        return m->cache.get(cache_key).value();
    }


    zdd* result;
    if (P == base())
        result = get_node(m, Q->value, zdd_union(m, P, Q->left), Q->right);
    else if (Q == base())
        result = get_node(m, P->value, zdd_union(m, P->left, Q), P->right);
    else if (P->value < Q->value)
        result = get_node(m, Q->value, zdd_union(m, P, Q->left), Q->right);
    else if (P->value > Q->value)
        result = get_node(m, P->value, zdd_union(m, P->left, Q), P->right);
    else if (P->value == Q->value)
        result = get_node (m, P->value, zdd_union(m, P->left, Q->left), zdd_union(m, P->right, Q->right));
    m->cache.insert(cache_key, result);
    return result;
}


/**
 * @brief Intersection between two Zdd nodes that each represent a set of sets.
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param P First zdd nodes that represents a set of sets.
 * @param Q Second zdd nodes that represents a set of sets.
 * @return P ⋂ Q 
 */
zdd* zdd_intersection(ZddManager* m, zdd* P, zdd* Q)
{
    if (P == empty()) return empty(); 
    if (Q == empty()) return empty();
    if (P == Q)       return P;
    
    std::tuple<intptr_t, intptr_t, cache_function> cache_key = std::make_tuple((intptr_t)P, (intptr_t)Q, inter_cache);
    if(m->cache.contains(cache_key) && m->cache.get(cache_key).has_value()){
        return m->cache.get(cache_key).value();
    }

    zdd* result;
    if (P == base())
        result = zdd_intersection(m, P, Q->left);
    else if (Q == base())
        result = zdd_intersection(m, P->left, Q);
    else if (P->value > Q->value)
        result = zdd_intersection(m, P->left, Q);
    else if (P->value < Q->value)
        result = zdd_intersection (m, P, Q->left);
    else if (P->value == Q->value) 
        result = get_node(m, P->value, zdd_intersection(m, P->left, Q->left), zdd_intersection(m, P->right, Q->right));
    m->cache.insert(cache_key, result);
    return result;
}


/**
 * @brief Difference between the first zdd node and the second zdd node.
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param P First zdd nodes that represents a set of sets.
 * @param Q Second zdd nodes that represents a set of sets.
 * @return P - Q 
 */
zdd* zdd_difference(ZddManager* m, zdd* P, zdd* Q)
{
    if (P == empty()) return empty(); 
    if (Q == empty()) return P;
    if (P == Q)       return empty();
    
    std::tuple<intptr_t, intptr_t, cache_function> cache_key = std::make_tuple((intptr_t)P, (intptr_t)Q, diff_cache);
    if(m->cache.contains(cache_key) && m->cache.get(cache_key).has_value()){
        return m->cache.get(cache_key).value();
    }
    
    zdd* result;
    if (P == base())
        result = zdd_difference(m, P, Q->left); 
    else if (Q == base()) 
        result = get_node(m, P->value, zdd_difference(m, P->left, Q), P->right);
    else if (P->value > Q->value)
        result = get_node(m, P->value, zdd_difference(m, P->left, Q), P->right);
    else if (P->value < Q->value)
        result = zdd_difference(m, P, Q->left);
    else if (P->value == Q->value)
        result = get_node(m, P->value, zdd_difference(m, P->left, Q->left), zdd_difference(m, P->right, Q->right));

    m->cache.insert(cache_key, result);
    return result;
}


/**
 * @brief Counts the number of sets in the set P.
 * 
 * @param P Set of sets of ints in the form of a zdd node.
 * @return Number of sets in the set P. 
 */
int zdd_count(zdd* P)
{
    if (P == empty()) return 0; 
    if (P == base())  return 1;
    return zdd_count(P->left) + zdd_count(P->right);
}


/**
 * @brief Preorder traversal through a zdd tree. During the traversal it creates the set of sets that the node represents.
 * 
 * @param node Current zdd node.
 * @param current_set Current set of ints that will be added to set_of_sets if node == {Ø}.
 * @param set_of_sets Pointer to where the set of sets the root node represents will be stored.
 */
void preorder_traversal(zdd* node, std::set<int>* current_set, std::set<std::set<int>>* set_of_sets)
{
    if (node == empty())
        return;
    if (node == base())
    {
        set_of_sets->insert(*current_set);
        return;
    }
    preorder_traversal(node->left, current_set, set_of_sets);
    current_set->insert(node->value);
    preorder_traversal(node->right, current_set, set_of_sets);
    current_set->erase(node->value);
}


/**
 * @brief Returns the set of sets the node P represents
 * 
 * @param P Zdd node that represents a set of sets
 * @return Set of sets the node P represents
 */
std::set<std::set<int>> to_set_of_sets(zdd* P)
{
    if(P == empty())
        return std::set<std::set<int>> ({});
    if(P == base())
        return std::set<std::set<int>> ({{}});
    std::set<std::set<int>> set_of_sets ({});
    std::set<int> current_set ({});
    preorder_traversal(P->left, &current_set, &set_of_sets);
    current_set.insert(P->value);
    preorder_traversal(P->right, &current_set, &set_of_sets);
    return set_of_sets;
}


/**
 * @brief Returns a zdd node that represents a set with a single set in it.
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param set_of_ints A set of ints that will be converted to a zdd node.
 * @return Zdd that represents {set_of_ints}.
 */
zdd* single_set_to_zdd(ZddManager* m, std::set<int> set_of_ints)
{
    zdd* root = base();
    for (int i : set_of_ints)
    {
        root = get_node(m, i, empty(), root);
    }
    return root;
}


/**
 * @brief Returns a zdd node that represent the setof_sets. Creates unecessary nodes. Marking trees and deleting unmarked nodes may be recommened.
 * 
 * @param m ZddManager that handles unique_nodes.
 * @param set_of_sets A set of sets that will be converted to a zdd node.
 * @return Zdd that represents set_of_sets. 
 */
zdd* to_zdd(ZddManager* m, std::set<std::set<int>> set_of_sets)
{   
    zdd* result = empty();
    for (auto s : set_of_sets)
    {
        zdd* tmp_zdd = single_set_to_zdd(m, s);
        result = zdd_union(m, result, tmp_zdd);
    }
    return result;
}

void preorder_traversal_smallest_set(ZddManager* m, zdd* node,std::set<int>* current_set, std::set<int> * current_smallest_set)
{
    if (node == empty())
        return ;
    if (node == base())
    {
        if(current_set->size() < current_smallest_set->size())
            *current_smallest_set =  *current_set;
        return;
    }
    if (current_smallest_set->size() < current_set->size()){
        current_set->erase(node->value);
        return;
    }

    preorder_traversal_smallest_set(m, node->left, current_set, current_smallest_set);
    current_set->insert(node->value);
    preorder_traversal_smallest_set(m, node->right, current_set, current_smallest_set);
    current_set->erase(node->value);
}


/**
 * @brief Returns the smallest set from the set of sets the zdd represents.
 * 
 * @param P Set of sets of ints in the form of a zdd node.
 * @return Smallest set from the set of sets the zdd represents.
 */
std::set<int> smallest_set(ZddManager* m, zdd* P)
{
    if(P == empty())
        throw std::invalid_argument("P has no sets");
    if(P == base())
        return {};
    std::set<int> current_smallest_set = {};
    zdd* node = P;
    while(node != base()){
        current_smallest_set.insert(node->value);
        node = node->right;
    }

    std::set<int> current_set = {};
    preorder_traversal_smallest_set(m, P->left, &current_set, &current_smallest_set);
    current_set.insert(P->value);
    preorder_traversal_smallest_set(m, P->right, &current_set, &current_smallest_set);
    return current_smallest_set;
}



