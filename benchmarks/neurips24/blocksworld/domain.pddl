(define (domain blocksworld_numeric-domain)
 (:requirements :strips :typing :numeric-fluents)
 (:types
    obj - object
    block cylinder - obj
 )
 (:predicates (base ?block - block) (on ?block - block ?support - obj) (in ?block - block ?cylinder - cylinder) (clear ?obj - obj) (holding ?block - block) (arm_empty))
 (:functions (capacity ?cylinder - cylinder))
 (:action pickup
  :parameters ( ?block - block ?cylinder - cylinder)
  :precondition (and (base ?block) (on ?block ?cylinder) (in ?block ?cylinder) (clear ?block) (arm_empty))
  :effect (and (not (base ?block)) (not (on ?block ?cylinder)) (not (in ?block ?cylinder)) (not (clear ?block)) (clear ?cylinder) (holding ?block) (not (arm_empty)) (increase (capacity ?cylinder) 1)))
 (:action putdown
  :parameters ( ?block - block ?cylinder - cylinder)
  :precondition (and (holding ?block) (clear ?cylinder) (<= 1 (capacity ?cylinder)))
  :effect (and (base ?block) (not (holding ?block)) (not (clear ?cylinder)) (on ?block ?cylinder) (in ?block ?cylinder) (clear ?block) (arm_empty) (decrease (capacity ?cylinder) 1)))
 (:action unstack
  :parameters ( ?block_a - block ?block_b - block ?cylinder - cylinder)
  :precondition (and (on ?block_a ?block_b) (in ?block_a ?cylinder) (clear ?block_a) (arm_empty))
  :effect (and (not (on ?block_a ?block_b)) (not (in ?block_a ?cylinder)) (not (clear ?block_a)) (clear ?block_b) (holding ?block_a) (not (arm_empty)) (increase (capacity ?cylinder) 1)))
 (:action stack
  :parameters ( ?block_a - block ?block_b - block ?cylinder - cylinder)
  :precondition (and (holding ?block_a) (clear ?block_b) (in ?block_b ?cylinder) (<= 1 (capacity ?cylinder)))
  :effect (and (not (holding ?block_a)) (not (clear ?block_b)) (on ?block_a ?block_b) (in ?block_a ?cylinder) (clear ?block_a) (arm_empty) (decrease (capacity ?cylinder) 1)))
)
