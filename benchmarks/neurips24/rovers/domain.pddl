;; Modifications to domain.pddl from numeric ipc2023:
;; change predicate name: in(x, y) -> at(x, y)
;; remove redundant available and channel_free predicates and to match classical ver.

;; This domain was motivated by the 2003 Mars Exploration Rover (MER) missions 
;; and the planned 2009 Mars Science Laboratory (MSL) mission, used in the IPC-3.
;;
;; Inspired by planetary rovers problems, this domain requires that a
;; collection of rovers navigate a planet surface, finding samples and
;; communicating them back to a lander.
(define (domain rovers)
    (:requirements :typing :fluents :numeric-fluents)
    (:types
        rover
        waypoint
        store
        camera
        mode
        lander
        objective
    )
    (:predicates
        (at ?x - rover ?y - waypoint)
        (at_lander ?x - lander ?y - waypoint)
        (can_traverse ?r - rover ?x - waypoint ?y - waypoint)
        (equipped_for_soil_analysis ?r - rover)
        (equipped_for_rock_analysis ?r - rover)
        (equipped_for_imaging ?r - rover)
        (empty ?s - store)
        (have_rock_analysis ?r - rover ?w - waypoint)
        (have_soil_analysis ?r - rover ?w - waypoint)
        (full ?s - store)
        (calibrated ?c - camera ?r - rover)
        (supports ?c - camera ?m - mode)
        (visible ?w - waypoint ?p - waypoint)
        (have_image ?r - rover ?o - objective ?m - mode)
        (communicated_soil_data ?w - waypoint)
        (communicated_rock_data ?w - waypoint)
        (communicated_image_data ?o - objective ?m - mode)
        (at_soil_sample ?w - waypoint)
        (at_rock_sample ?w - waypoint)
        (visible_from ?o - objective ?w - waypoint)
        (store_of ?s - store ?r - rover)
        (calibration_target ?i - camera ?o - objective)
        (on_board ?i - camera ?r - rover)
        (in_sun ?w - waypoint)
    )

    (:functions
        (energy ?r - rover)
        (recharges)
    )

    (:action navigate
        :parameters (?x - rover ?y - waypoint ?z - waypoint)
        :precondition (and
            (>= (energy ?x) 8)
            (can_traverse ?x ?y ?z)
            (at ?x ?y)
            (visible ?y ?z))
        :effect (and
            (decrease (energy ?x) 8)
            (not (at ?x ?y))
            (at ?x ?z))
    )

    (:action recharge
        :parameters (?x - rover ?w - waypoint)
        :precondition (and
            (<= (energy ?x) 80)
            (at ?x ?w)
            (in_sun ?w))
        :effect (and
            (increase (energy ?x) 20)
            (increase (recharges) 1))
    )

    (:action sample_soil
        :parameters (?x - rover ?s - store ?p - waypoint)
        :precondition (and
            (>= (energy ?x) 3)
            (at ?x ?p)
            (at_soil_sample ?p)
            (equipped_for_soil_analysis ?x)
            (store_of ?s ?x)
            (empty ?s))
        :effect (and
            (decrease (energy ?x) 3)
            (not (empty ?s))
            (full ?s)
            (have_soil_analysis ?x ?p)
            (not (at_soil_sample ?p)))
    )

    (:action sample_rock
        :parameters (?x - rover ?s - store ?p - waypoint)
        :precondition (and
            (>= (energy ?x) 5)
            (at ?x ?p)
            (at_rock_sample ?p)
            (equipped_for_rock_analysis ?x)
            (store_of ?s ?x)
            (empty ?s))
        :effect (and
            (decrease (energy ?x) 5)
            (not (empty ?s))
            (full ?s)
            (have_rock_analysis ?x ?p)
            (not (at_rock_sample ?p)))
    )

    (:action drop
        :parameters (?x - rover ?y - store)
        :precondition (and
            (store_of ?y ?x)
            (full ?y))
        :effect (and
            (not (full ?y))
            (empty ?y))
    )

    (:action calibrate
        :parameters (?r - rover ?i - camera ?t - objective ?w - waypoint)
        :precondition (and
            (>= (energy ?r) 2)
            (equipped_for_imaging ?r)
            (calibration_target ?i ?t)
            (at ?r ?w)
            (visible_from ?t ?w)
            (on_board ?i ?r))
        :effect (and
            (decrease (energy ?r) 2)
            (calibrated ?i ?r))
    )

    (:action take_image
        :parameters (?r - rover ?p - waypoint ?o - objective ?i - camera ?m - mode)
        :precondition (and
            (>= (energy ?r) 1)
            (calibrated ?i ?r)
            (on_board ?i ?r)
            (equipped_for_imaging ?r)
            (supports ?i ?m)
            (visible_from ?o ?p)
            (at ?r ?p))
        :effect (and
            (decrease (energy ?r) 1)
            (have_image ?r ?o ?m)
            (not (calibrated ?i ?r)))
    )

    (:action communicate_soil_data
        :parameters (?r - rover ?l - lander ?p - waypoint ?x - waypoint ?y - waypoint)
        :precondition (and
            (>= (energy ?r) 4)
            (at ?r ?x)
            (at_lander ?l ?y)
            (have_soil_analysis ?r ?p)
            (visible ?x ?y))
        :effect (and
            (decrease (energy ?r) 4)
            (communicated_soil_data ?p))
    )

    (:action communicate_rock_data
        :parameters (?r - rover ?l - lander ?p - waypoint ?x - waypoint ?y - waypoint)
        :precondition (and
            (>= (energy ?r) 4)
            (at ?r ?x)
            (at_lander ?l ?y)
            (have_rock_analysis ?r ?p)
            (visible ?x ?y))
        :effect (and
            (decrease (energy ?r) 4)
            (communicated_rock_data ?p))
    )

    (:action communicate_image_data
        :parameters (?r - rover ?l - lander ?o - objective ?m - mode ?x - waypoint ?y - waypoint)
        :precondition (and
            (>= (energy ?r) 6)
            (at ?r ?x)
            (at_lander ?l ?y)
            (have_image ?r ?o ?m)
            (visible ?x ?y))
        :effect (and
            (decrease (energy ?r) 6)
            (communicated_image_data ?o ?m))
    )
)
