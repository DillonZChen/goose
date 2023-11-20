( define ( domain Depot ) ( :requirements :typing ) ( :types place locatable - object depot distributor - place truck hoist surface - locatable pallet crate - surface ) ( :predicates ( at ?x - locatable ?y - place ) ( on ?x - crate ?y - surface ) ( in ?x - crate ?y - truck ) ( lifting ?x - hoist ?y - crate ) ( available ?x - hoist ) ( clear ?x - surface ) ) ( :action Drive :parameters ( ?x - truck ?y - place ?z - place ) :precondition ( and ( at ?x ?y ) ) :effect ( and ( at ?x ?z ) ) ) ( :action Lift :parameters ( ?x - hoist ?y - crate ?z - surface ?p - place ) :precondition ( and ( at ?x ?p ) ( available ?x ) ( at ?y ?p ) ( on ?y ?z ) ( clear ?y ) ) :effect ( and ( lifting ?x ?y ) ( clear ?z ) ) ) ( :action Drop :parameters ( ?x - hoist ?y - crate ?z - surface ?p - place ) :precondition ( and ( at ?x ?p ) ( at ?z ?p ) ( clear ?z ) ( lifting ?x ?y ) ) :effect ( and ( available ?x ) ( at ?y ?p ) ( clear ?y ) ( on ?y ?z ) ) ) ( :action Load :parameters ( ?x - hoist ?y - crate ?z - truck ?p - place ) :precondition ( and ( at ?x ?p ) ( at ?z ?p ) ( lifting ?x ?y ) ) :effect ( and ( in ?y ?z ) ( available ?x ) ) ) ( :action Unload :parameters ( ?x - hoist ?y - crate ?z - truck ?p - place ) :precondition ( and ( at ?x ?p ) ( at ?z ?p ) ( available ?x ) ( in ?y ?z ) ) :effect ( and ( lifting ?x ?y ) ) ) )