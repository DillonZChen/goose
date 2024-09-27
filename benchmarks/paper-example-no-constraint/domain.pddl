(define (domain ccblocksworld)
    (:requirements :strips :typing :numeric-fluents)
    (:types
        block base - object
    )
    (:predicates
        (on         ?x - block      ?y - object)
        (above      ?x - block      ?y - base  )
        (clear      ?x - object                )
        (holding    ?x - block                 )
        (arm_empty                             )
    )
    (:functions
        (capacity   ?x - base                  )
    )
    (:action pickup
        :parameters (?block - block ?base - base)
        :precondition (and
            (on ?block ?base)
            (above ?block ?base)
            (clear ?block)
            (arm_empty))
        :effect (and
            (not (on ?block ?base))
            (not (above ?block ?base))
            (not (clear ?block))
            (clear ?base)
            (holding ?block)
            (not (arm_empty)))
    )
    (:action putdown
        :parameters (?block - block ?base - base)
        :precondition (and
            (holding ?block)
            (clear ?base)
            (<= 1 (capacity ?base)))
        :effect (and
            (not (holding ?block))
            (not (clear ?base))
            (on ?block ?base)
            (above ?block ?base)
            (clear ?block)
            (arm_empty))
    )
    (:action unstack
        :parameters (?block_a - block ?block_b - block ?base - base)
        :precondition (and
            (on ?block_a ?block_b)
            (above ?block_a ?base)
            (clear ?block_a)
            (arm_empty))
        :effect (and
            (not (on ?block_a ?block_b))
            (not (above ?block_a ?base))
            (not (clear ?block_a))
            (clear ?block_b)
            (holding ?block_a)
            (not (arm_empty)))
    )
    (:action stack
        :parameters (?block_a - block ?block_b - block ?base - base)
        :precondition (and
            (holding ?block_a)
            (clear ?block_b)
            (above ?block_b ?base))
        :effect (and
            (not (holding ?block_a))
            (not (clear ?block_b))
            (on ?block_a ?block_b)
            (above ?block_a ?base)
            (clear ?block_a)
            (arm_empty))
    )
)