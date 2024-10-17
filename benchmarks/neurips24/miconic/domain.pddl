; Source: https://github.com/AI-Planning/pddl-generators/tree/main/miconic
; Fix: prevent the elevator from boarding served passengers => (not (origin ?p ?f)) effect added to board action
(define (domain miconic)
    (:requirements :strips :typing :numeric-fluents)
    (:types
        passenger - object
        floor - object
    )

    (:predicates
        (origin ?person - passenger ?floor - floor)
        ;; entry of ?person is ?floor
        ;; inertia

        (destin ?person - passenger ?floor - floor)
        ;; exit of ?person is ?floor
        ;; inertia

        (above ?floor1 - floor ?floor2 - floor)
        ;; ?floor2 is located above of ?floor1

        (boarded ?person - passenger)
        ;; true if ?person has boarded the lift

        (served ?person - passenger)
        ;; true if ?person has alighted as her destination

        (lift-at ?floor - floor)
        ;; current position of the lift is at ?floor

        (move_slow_half)
    )

    (:functions
        (lift-capacity)
        ;; number of people in the lift

        (weight ?person - passenger)
    )

    (:action board
        :parameters (?f - floor ?p - passenger)
        :precondition (and
            (>= (lift-capacity) (weight ?p))
            (lift-at ?f)
            (origin ?p ?f)
        )
        :effect (and
            (decrease (lift-capacity) (weight ?p))
            (boarded ?p)
            (not (origin ?p ?f))
        )
    )

    (:action depart
        :parameters (?f - floor ?p - passenger)
        :precondition (and
            (lift-at ?f)
            (destin ?p ?f)
            (boarded ?p)
        )
        :effect (and
            (increase (lift-capacity) (weight ?p))
            (not (boarded ?p))
            (served ?p)
        )
    )

    (:action up_fast
        :parameters (?f1 - floor ?f2 - floor)
        :precondition (and (lift-at ?f1) (above ?f1 ?f2) (>= (lift-capacity) 1))
        :effect (and (lift-at ?f2) (not (lift-at ?f1)))
    )

    (:action down_fast
        :parameters (?f1 - floor ?f2 - floor)
        :precondition (and (lift-at ?f1) (above ?f2 ?f1) (>= (lift-capacity) 1))
        :effect (and (lift-at ?f2) (not (lift-at ?f1)))
    )

    (:action up_slow_part_1
        :parameters (?f1 - floor ?f2 - floor)
        :precondition (and (lift-at ?f1) (above ?f1 ?f2))
        :effect (and (move_slow_half))
    )

    (:action up_slow_part_2
        :parameters (?f1 - floor ?f2 - floor)
        :precondition (and (lift-at ?f1) (above ?f1 ?f2) (move_slow_half))
        :effect (and (lift-at ?f2) (not (lift-at ?f1)) (not (move_slow_half)))
    )

    (:action down_slow_part_1
        :parameters (?f1 - floor ?f2 - floor)
        :precondition (and (lift-at ?f1) (above ?f2 ?f1))
        :effect (and (move_slow_half))
    )

    (:action down_slow_part_2
        :parameters (?f1 - floor ?f2 - floor)
        :precondition (and (lift-at ?f1) (above ?f2 ?f1) (move_slow_half))
        :effect (and (lift-at ?f2) (not (lift-at ?f1)) (not (move_slow_half)))
    )
)