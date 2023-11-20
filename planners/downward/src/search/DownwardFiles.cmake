# See https://www.fast-downward.org/ForDevelopers/AddingSourceFiles
# for general information on adding source files and CMake plugins.
#
# All plugins are enabled by default and users can disable them by specifying
#    -DPLUGIN_FOO_ENABLED=FALSE
# The default behavior can be changed so all non-essential plugins are
# disabled by default by specifying
#    -DDISABLE_PLUGINS_BY_DEFAULT=TRUE
# In that case, individual plugins can be enabled with
#    -DPLUGIN_FOO_ENABLED=TRUE
#
# Defining a new plugin:
#    fast_downward_plugin(
#        NAME <NAME>
#        [ DISPLAY_NAME <DISPLAY_NAME> ]
#        [ HELP <HELP> ]
#        SOURCES
#            <FILE_1> [ <FILE_2> ... ]
#        [ DEPENDS <PLUGIN_NAME_1> [ <PLUGIN_NAME_2> ... ] ]
#        [ DEPENDENCY_ONLY ]
#        [ CORE_PLUGIN ]
#    )
#
# <DISPLAY_NAME> defaults to lower case <NAME> and is used to group files
#   in IDEs and for messages.
# <HELP> defaults to <DISPLAY_NAME> and is used to describe the cmake option.
# SOURCES lists the source files that are part of the plugin. Entries are
#   listed without extension. For an entry <file>, both <file>.h and <file>.cc
#   are added if the files exist.
# DEPENDS lists plugins that will be automatically enabled if this plugin is
#   enabled. If the dependency was not enabled before, this will be logged.
# DEPENDENCY_ONLY disables the plugin unless it is needed as a dependency and
#   hides the option to enable the plugin in cmake GUIs like ccmake.
# CORE_PLUGIN always enables the plugin (even if DISABLE_PLUGINS_BY_DEFAULT
#   is used) and hides the option to disable it in CMake GUIs like ccmake.

option(
    DISABLE_PLUGINS_BY_DEFAULT
    "If set to YES only plugins that are specifically enabled will be compiled"
    NO)
# This option should not show up in CMake GUIs like ccmake where all
# plugins are enabled or disabled manually.
mark_as_advanced(DISABLE_PLUGINS_BY_DEFAULT)

fast_downward_plugin(
    NAME CORE_SOURCES
    HELP "Core source files"
    SOURCES
        planner

        abstract_task
        axioms
        command_line
        evaluation_context
        evaluation_result
        evaluator
        evaluator_cache
        heuristic
        open_list
        open_list_factory
        operator_cost
        operator_id
        per_state_array
        per_state_bitset
        per_state_information
        per_task_information
        plan_manager
        pruning_method
        search_engine
        search_node_info
        search_progress
        search_space
        search_statistics
        state_id
        state_registry
        task_id
        task_proxy

    DEPENDS CAUSAL_GRAPH INT_HASH_SET INT_PACKER ORDERED_SET SEGMENTED_VECTOR SUBSCRIBER SUCCESSOR_GENERATOR TASK_PROPERTIES
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME PLUGINS
    HELP "Plugin definition"
    SOURCES
        plugins/any
        plugins/bounds
        plugins/doc_printer
        plugins/options
        plugins/plugin
        plugins/plugin_info
        plugins/raw_registry
        plugins/registry
        plugins/registry_types
        plugins/types
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME PARSER
    HELP "Option parsing"
    SOURCES
        parser/abstract_syntax_tree
        parser/decorated_abstract_syntax_tree
        parser/lexical_analyzer
        parser/syntax_analyzer
        parser/token_stream
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME UTILS
    HELP "System utilities"
    SOURCES
        utils/collections
        utils/countdown_timer
        utils/exceptions
        utils/hash
        utils/language
        utils/logging
        utils/markup
        utils/math
        utils/memory
        utils/rng
        utils/rng_options
        utils/strings
        utils/system
        utils/system_unix
        utils/system_windows
        utils/timer
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME ALTERNATION_OPEN_LIST
    HELP "Open list that alternates between underlying open lists in a round-robin manner"
    SOURCES
        open_lists/alternation_open_list
)

fast_downward_plugin(
    NAME BEST_FIRST_OPEN_LIST
    HELP "Open list that selects the best element according to a single evaluation function"
    SOURCES
        open_lists/best_first_open_list
)

fast_downward_plugin(
    NAME EPSILON_GREEDY_OPEN_LIST
    HELP "Open list that chooses an entry randomly with probability epsilon"
    SOURCES
        open_lists/epsilon_greedy_open_list
)

fast_downward_plugin(
    NAME PARETO_OPEN_LIST
    HELP "Pareto open list"
    SOURCES
        open_lists/pareto_open_list
)

fast_downward_plugin(
    NAME TIEBREAKING_OPEN_LIST
    HELP "Tiebreaking open list"
    SOURCES
        open_lists/tiebreaking_open_list
)

fast_downward_plugin(
    NAME TYPE_BASED_OPEN_LIST
    HELP "Type-based open list"
    SOURCES
        open_lists/type_based_open_list
)

fast_downward_plugin(
    NAME DYNAMIC_BITSET
    HELP "Poor man's version of boost::dynamic_bitset"
    SOURCES
        algorithms/dynamic_bitset
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME NAMED_VECTOR
    HELP "Generic vector with associated name for each element"
    SOURCES
        algorithms/named_vector
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME EQUIVALENCE_RELATION
    HELP "Equivalence relation over [1, ..., n] that can be iteratively refined"
    SOURCES
        algorithms/equivalence_relation
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME INT_HASH_SET
    HELP "Hash set storing non-negative integers"
    SOURCES
        algorithms/int_hash_set
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME INT_PACKER
    HELP "Greedy bin packing algorithm to pack integer variables with small domains tightly into memory"
    SOURCES
        algorithms/int_packer
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME MAX_CLIQUES
    HELP "Implementation of the Max Cliques algorithm by Tomita et al."
    SOURCES
        algorithms/max_cliques
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PRIORITY_QUEUES
    HELP "Three implementations of priority queue: HeapQueue, BucketQueue and AdaptiveQueue"
    SOURCES
        algorithms/priority_queues
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME ORDERED_SET
    HELP "Set of elements ordered by insertion time"
    SOURCES
        algorithms/ordered_set
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SEGMENTED_VECTOR
    HELP "Memory-friendly and vector-like data structure"
    SOURCES
        algorithms/segmented_vector
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SUBSCRIBER
    HELP "Allows object to subscribe to the destructor of other objects"
    SOURCES
        algorithms/subscriber
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME EVALUATORS_SUBCATEGORY
    HELP "Subcategory plugin for basic evaluators"
    SOURCES
        evaluators/subcategory
)

fast_downward_plugin(
    NAME NULL_PRUNING_METHOD
    HELP "Pruning method that does nothing"
    SOURCES
        pruning/null_pruning_method
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME LIMITED_PRUNING
    HELP "Method for limiting another pruning method"
    SOURCES
        pruning/limited_pruning
)

fast_downward_plugin(
    NAME STUBBORN_SETS
    HELP "Base class for all stubborn set partial order reduction methods"
    SOURCES
        pruning/stubborn_sets
    DEPENDS TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME STUBBORN_SETS_ACTION_CENTRIC
    HELP "Base class for all action-centric stubborn set partial order reduction methods"
    SOURCES
        pruning/stubborn_sets_action_centric
    DEPENDS STUBBORN_SETS
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME STUBBORN_SETS_ATOM_CENTRIC
    HELP "Atom-centric stubborn sets"
    SOURCES
        pruning/stubborn_sets_atom_centric
    DEPENDS STUBBORN_SETS
)

fast_downward_plugin(
    NAME STUBBORN_SETS_SIMPLE
    HELP "Stubborn sets simple"
    SOURCES
        pruning/stubborn_sets_simple
    DEPENDS STUBBORN_SETS_ACTION_CENTRIC
)

fast_downward_plugin(
    NAME STUBBORN_SETS_EC
    HELP "Stubborn set method that dominates expansion core"
    SOURCES
        pruning/stubborn_sets_ec
    DEPENDS STUBBORN_SETS_ACTION_CENTRIC TASK_PROPERTIES
)

fast_downward_plugin(
    NAME SEARCH_COMMON
    HELP "Basic classes used for all search engines"
    SOURCES
        search_engines/search_common
    DEPENDS ALTERNATION_OPEN_LIST BEST_FIRST_OPEN_LIST TIEBREAKING_OPEN_LIST
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME EAGER_SEARCH
    HELP "Eager search algorithm"
    SOURCES
        search_engines/eager_search
    DEPENDS NULL_PRUNING_METHOD ORDERED_SET SUCCESSOR_GENERATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME BATCH_EAGER_SEARCH
    HELP "Batch eager search algorithm"
    SOURCES
        search_engines/batch_eager_search
    DEPENDS NULL_PRUNING_METHOD ORDERED_SET SUCCESSOR_GENERATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PLUGIN_EAGER
    HELP "Eager (i.e., normal) best-first search"
    SOURCES
        search_engines/plugin_eager
    DEPENDS EAGER_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_EAGER_GREEDY
    HELP "Eager greedy best-first search"
    SOURCES
        search_engines/plugin_eager_greedy
    DEPENDS EAGER_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_BATCH_EAGER_GREEDY
    HELP "Batch eager greedy best-first search"
    SOURCES
        search_engines/plugin_batch_eager_greedy
    DEPENDS BATCH_EAGER_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_LAZY
    HELP "Best-first search with deferred evaluation (lazy)"
    SOURCES
        search_engines/plugin_lazy
    DEPENDS LAZY_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_LAZY_GREEDY
    HELP "Greedy best-first search with deferred evaluation (lazy)"
    SOURCES
        search_engines/plugin_lazy_greedy
    DEPENDS LAZY_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME LAZY_SEARCH
    HELP "Lazy search algorithm"
    SOURCES
        search_engines/lazy_search
    DEPENDS ORDERED_SET SUCCESSOR_GENERATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PLUGIN_PERFECT
    HELP "Generate states from plan"
    SOURCES
        search_engines/plugin_perfect
    DEPENDS PERFECT_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PERFECT_SEARCH
    HELP "Generate states from plan"
    SOURCES
        search_engines/perfect
    DEPENDS ORDERED_SET SUCCESSOR_GENERATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME BLIND_HEURISTIC
    HELP "blind heuristic"
    SOURCES
        heuristics/blind
)

fast_downward_plugin(
    NAME GOOSE_HEURISTIC
    HELP "The GOOSE heuristic"
    SOURCES
        heuristics/goose_heuristic
)

fast_downward_plugin(
    NAME CORE_TASKS
    HELP "Core task transformations"
    SOURCES
        tasks/cost_adapted_task
        tasks/delegating_task
        tasks/root_task
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME EXTRA_TASKS
    HELP "Non-core task transformations"
    SOURCES
        tasks/domain_abstracted_task
        tasks/domain_abstracted_task_factory
        tasks/modified_goals_task
        tasks/modified_operator_costs_task
    DEPENDS TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME CAUSAL_GRAPH
    HELP "Causal Graph"
    SOURCES
        task_utils/causal_graph
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SAMPLING
    HELP "Sampling"
    SOURCES
        task_utils/sampling
    DEPENDS SUCCESSOR_GENERATOR TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SUCCESSOR_GENERATOR
    HELP "Successor generator"
    SOURCES
        task_utils/successor_generator
        task_utils/successor_generator_factory
        task_utils/successor_generator_internals
    DEPENDS TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME TASK_PROPERTIES
    HELP "Task properties"
    SOURCES
        task_utils/task_properties
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME VARIABLE_ORDER_FINDER
    HELP "Variable order finder"
    SOURCES
        task_utils/variable_order_finder
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SCCS
    HELP "Algorithm to compute the strongly connected components (SCCs) of a "
         "directed graph."
    SOURCES
        algorithms/sccs
    DEPENDENCY_ONLY
)

fast_downward_add_plugin_sources(PLANNER_SOURCES)

# The order in PLANNER_SOURCES influences the order in which object
# files are given to the linker, which can have a significant influence
# on performance (see issue67). The general recommendation seems to be
# to list files that define functions after files that use them.
# We approximate this by reversing the list, which will put the plugins
# first, followed by the core files, followed by the main file.
# This is certainly not optimal, but works well enough in practice.
list(REVERSE PLANNER_SOURCES)
