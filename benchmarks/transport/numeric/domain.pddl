; source: https://github.com/AI-Planning/pddl-generators/blob/main/transport/domain.pddl
; updates:
;  - removed :action-costs and :functions
;  - capacity type now is size
;  - capacity-number predicate now is capacity
(define (domain transport)
    (:requirements :typing :numeric-fluents)
    (:types
        size location locatable - object
        vehicle package - locatable
    )

    (:predicates
        (road ?l1 ?l2 - location)
        (at ?x - locatable ?v - location)
        (in ?x - package ?v - vehicle)
    )

    (:functions
        (capacity ?v - vehicle)
    )

    (:action drive
        :parameters (?v - vehicle ?l1 ?l2 - location)
        :precondition (and
            (at ?v ?l1)
            (road ?l1 ?l2)
        )
        :effect (and
            (not (at ?v ?l1))
            (at ?v ?l2)
        )
    )

    (:action pick-up
        :parameters (?v - vehicle ?l - location ?p - package)
        :precondition (and
            (at ?v ?l)
            (at ?p ?l)
            (>= (capacity ?v) 1)
        )
        :effect (and
            (not (at ?p ?l))
            (in ?p ?v)
            (decrease (capacity ?v) 1)
        )
    )

    (:action drop
        :parameters (?v - vehicle ?l - location ?p - package)
        :precondition (and
            (at ?v ?l)
            (in ?p ?v)
        )
        :effect (and
            (not (in ?p ?v))
            (at ?p ?l)
            (increase (capacity ?v) 1)
        )
    )
)
