(define (domain blocksworld)
  (:requirements :strips :equality)
  (:predicates (on-table ?x) (on ?x ?y) (clear ?x))
  (:functions (moves-to-table) (moves-to-block) (total-moves))

  (:action movetotable
    :parameters (?x ?y)
    :precondition (and (clear ?x) (on ?x ?y) (not (= ?x ?y)))
    :effect (and (clear ?y) (on-table ?x) (not (on ?x ?y))
		 (increase (moves-to-table) 1)
		 (increase (total-moves) 1))
    )
  
  (:action movetoblock
    :parameters (?x ?y ?z)
    :precondition (and (clear ?x) (clear ?z) (on ?x ?y) (not (= ?x ?z))
		       (not (= ?y ?z)))
    :effect (and (clear ?y) (on ?x ?z) (not (clear ?z)) (not (on ?x ?y))
		 (increase (moves-to-block) 1)
		 (increase (total-moves) 1))
    )
  
  (:action movefromtable
   :parameters (?x ?y)
    :precondition (and (clear ?x) (clear ?y) (on-table ?x) (not (= ?x ?y)))
    :effect (and (on ?x ?y) (not (clear ?y)) (not (on-table ?x))
		 (increase (moves-to-block) 1)
		 (increase (total-moves) 1))
    )
  )
