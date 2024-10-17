(define (domain childsnack)
    (:requirements :strips :typing :numeric-fluents)
    (:types
        tray place gluten
    )
    (:constants
        is_gluten_free is_not_gluten_free - gluten
        kitchen - place
    )
    (:predicates
        (at ?tray - tray ?place - place)
        (gluten_free ?gluten - gluten)
    )
    (:functions
        (at_kitchen_bread ?gluten - gluten)
        (at_kitchen_content ?gluten - gluten)
        (at_kitchen_sandwich ?gluten - gluten)
        (ontray ?tray - tray ?gluten - gluten)
        (hungry ?place - place ?gluten - gluten)
    )
    (:action make_sandwich_no_gluten
        :parameters (?bread_gluten - gluten ?content_gluten - gluten)
        :precondition (and
            (gluten_free ?bread_gluten)
            (gluten_free ?content_gluten)
            (<= 1 (at_kitchen_bread ?bread_gluten))
            (<= 1 (at_kitchen_content ?content_gluten)))
        :effect (and
            (decrease (at_kitchen_bread ?bread_gluten) 1)
            (decrease (at_kitchen_content ?content_gluten) 1)
            (increase (at_kitchen_sandwich is_gluten_free) 1))
    )
    (:action make_sandwich
        :parameters (?bread_gluten - gluten ?content_gluten - gluten)
        :precondition (and
            (<= 1 (at_kitchen_bread ?bread_gluten))
            (<= 1 (at_kitchen_content ?content_gluten)))
        :effect (and
            (decrease (at_kitchen_bread ?bread_gluten) 1)
            (decrease (at_kitchen_content ?content_gluten) 1)
            (increase (at_kitchen_sandwich is_not_gluten_free) 1))
    )
    (:action put_on_tray
        :parameters (?tray - tray ?gluten - gluten)
        :precondition (and
            (<= 1 (at_kitchen_sandwich ?gluten))
            (at ?tray kitchen))
        :effect (and
            (decrease (at_kitchen_sandwich ?gluten) 1)
            (increase (ontray ?tray ?gluten) 1))
    )
    (:action serve_sandwich_no_gluten
        :parameters (?tray - tray ?place - place)
        :precondition (and
            (at ?tray ?place)
            (<= 1 (ontray ?tray is_gluten_free))
            (<= 1 (hungry ?place is_gluten_free)))
        :effect (and
            (decrease (ontray ?tray is_gluten_free) 1)
            (decrease (hungry ?place is_gluten_free) 1))
    )
    (:action serve_sandwich
        :parameters (?tray - tray ?place - place ?gluten - gluten)
        :precondition (and
            (at ?tray ?place)
            (<= 1 (ontray ?tray ?gluten))
            (<= 1 (hungry ?place is_not_gluten_free)))
        :effect (and
            (decrease (ontray ?tray ?gluten) 1)
            (decrease (hungry ?place is_not_gluten_free) 1))
    )
    (:action move_tray
        :parameters (?tray - tray ?place1 - place ?place2 - place)
        :precondition (and
            (at ?tray ?place1))
        :effect (and
            (at ?tray ?place2)
            (not (at ?tray ?place1)))
    )
)