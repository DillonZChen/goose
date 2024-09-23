;; source: https://github.com/AI-Planning/pddl-generators/blob/main/sokoban/domain.pddl
;; updates:
;;   - only focus on microban instances, hence 4 constant directions
;;
(define (domain sokoban)
    (:requirements :typing :derived-predicates)
    (:types
        location direction box
    )

    (:constants
        down up left right - direction
    )

    (:predicates
        (at-robot ?l - location)
        (at ?o - box ?l - location)
        (adjacent ?l1 - location ?l2 - location ?d - direction)
        (clear ?l - location)

        ; derived
        (reachable ?l - location)
    )

    (:derived
        (reachable ?l - location)
        (exists
            (?other_l - location ?d - direction)
            (and (clear ?l) (adjacent ?other_l ?l ?d) (reachable ?other_l)))
    )

    (:derived
        (reachable ?l - location)
        (at-robot ?l)
    )

    (:action push
        :parameters (?rloc - location ?bloc - location ?floc - location ?dir - direction ?b - box)
        :precondition (and
            (reachable ?rloc)
            (at ?b ?bloc)
            (clear ?floc)
            (adjacent ?rloc ?bloc ?dir)
            (adjacent ?bloc ?floc ?dir)
        )

        :effect (and
            (at-robot ?bloc)
            (at ?b ?floc)
            (clear ?bloc)
            (not (at-robot ?rloc))
            (not (at ?b ?bloc))
            (not (clear ?floc))
        )
    )
)