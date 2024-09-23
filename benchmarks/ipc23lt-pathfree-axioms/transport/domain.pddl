; source: https://github.com/AI-Planning/pddl-generators/blob/main/transport/domain.pddl
; updates:
;  - removed :action-costs and :functions
;  - capacity type now is size
;  - capacity-number predicate now is capacity
(define (domain transport)
    (:requirements :typing :derived-predicates)
    (:types
        size location locatable - object
        vehicle package - locatable
    )

    (:predicates
        (road ?l1 ?l2 - location)
        (at ?x - locatable ?v - location)
        (in ?x - package ?v - vehicle)
        (capacity ?v - vehicle ?s1 - size)
        (capacity-predecessor ?s1 ?s2 - size)

        ; derived
        (reachable ?v - vehicle ?l1 - location)
    )

    (:derived
        (reachable ?v - vehicle ?l1 - location)
        (exists (?l2 - location) (and (road ?l2 ?l1) (reachable ?v ?l2)))
    )

    (:derived
        (reachable ?v - vehicle ?l1 - location)
        (at ?v ?l1)
    )

    (:action pick-up
        :parameters (?v - vehicle ?l - location ?p - package ?s1 ?s2 - size)
        :precondition (and
            (reachable ?v ?l)
            (at ?p ?l)
            (capacity-predecessor ?s1 ?s2)
            (capacity ?v ?s2)
        )
        :effect (and
            (at ?v ?l)
            (not (at ?p ?l))
            (in ?p ?v)
            (capacity ?v ?s1)
            (not (capacity ?v ?s2))
        )
    )

    (:action drop
        :parameters (?v - vehicle ?l - location ?p - package ?s1 ?s2 - size)
        :precondition (and
            (reachable ?v ?l)
            (in ?p ?v)
            (capacity-predecessor ?s1 ?s2)
            (capacity ?v ?s1)
        )
        :effect (and
            (at ?v ?l)
            (not (in ?p ?v))
            (at ?p ?l)
            (capacity ?v ?s2)
            (not (capacity ?v ?s1))
        )
    )
)
