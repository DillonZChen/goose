; source => https://github.com/AI-Planning/pddl-generators/blob/main/spanner/domain.pddl
(define (domain spanner)
    (:requirements :typing :strips :numeric-fluents)
    (:types
        location locatable - object
        man - locatable
    )

    (:constants gate - location)

    (:predicates
        (at ?m - locatable ?l - location)
        (link ?l1 - location ?l2 - location)
    )

    (:functions
        (spanners_at ?l - location)
        (carrying ?m - man)
        (loose)
    )

    (:action walk
        :parameters (?start - location ?end - location ?m - man)
        :precondition (and
            (at ?m ?start)
            (link ?start ?end)
        )
        :effect (and
            (not (at ?m ?start))
            (at ?m ?end)
        )
    )

    (:action pickup_spanner
        :parameters (?l - location ?m - man)
        :precondition (and
            (at ?m ?l)
            (>= (spanners_at ?l) 1)
        )
        :effect (and
            (decrease (spanners_at ?l) 1)
            (increase (carrying ?m) 1)
        )
    )

    (:action tighten_nut
        :parameters (?m - man)
        :precondition (and
            (at ?m gate)
            (>= (loose) 1)
            (>= (carrying ?m) 1)
        )
        :effect (and
            (decrease (loose) 1)
            (decrease (carrying ?m) 1)
        )
    )
)