; Source: https://github.com/AI-Planning/pddl-generators/tree/main/miconic
; Fix: prevent the elevator from boarding served passengers => (not (origin ?p ?f)) effect added to board action
(define (domain miconic)
  (:requirements :strips :typing)
  (:types passenger - object
          floor - object
         )

(:predicates 
(origin ?person - passenger ?floor - floor)
;; entry of ?person is ?floor
;; inertia

(destin ?person - passenger ?floor - floor)
;; exit of ?person is ?floor
;; inertia

(above ?floor1 - floor  ?floor2 - floor)
;; ?floor2 is located above of ?floor1

(boarded ?person - passenger)
;; true if ?person has boarded the lift

(served ?person - passenger)
;; true if ?person has alighted as her destination

(lift-at ?floor - floor)
;; current position of the lift is at ?floor
)


;;stop and allow boarding

(:action board
  :parameters (?f - floor ?p - passenger)
  :precondition (and (lift-at ?f) (origin ?p ?f))
  :effect (and (boarded ?p) (not (origin ?p ?f))))

(:action depart
  :parameters (?f - floor ?p - passenger)
  :precondition (and (lift-at ?f) (destin ?p ?f)
		     (boarded ?p))
  :effect (and (not (boarded ?p))
	       (served ?p)))
;;drive up

(:action up
  :parameters (?f1 - floor ?f2 - floor)
  :precondition (and (lift-at ?f1) (above ?f1 ?f2))
  :effect (and (lift-at ?f2) (not (lift-at ?f1))))


;;drive down

(:action down
  :parameters (?f1 - floor ?f2 - floor)
  :precondition (and (lift-at ?f1) (above ?f2 ?f1))
  :effect (and (lift-at ?f2) (not (lift-at ?f1))))
)



