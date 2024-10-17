; ferry can now board up to 4 cars (see testing files)

; source: https://github.com/AI-Planning/pddl-generators/blob/main/ferry/domain.pddl
; updates: typing car and locations; sail precs have now (not (at-ferry ?to)) so (not-eq ?from ?to) can be removed
(define (domain ferry)
   (:requirements :typing :strips :negative-preconditions :numeric-fluents)
   (:types
        car - object
        location - object )

   (:predicates
		(at-ferry ?l - location)
		(at ?c - car ?l - location)
		(empty-ferry)  ; keep this to make conversion from classical easier
		(on ?c - car))
   
   (:functions
        (ferry-capacity))

   (:action sail
       :parameters  (?from - location ?to - location)
       :precondition (and (at-ferry ?from) (not (at-ferry ?to)))
       :effect (and  (at-ferry ?to) (not (at-ferry ?from))))


   (:action board
       :parameters (?car - car ?loc - location)
       :precondition  (and 
            (>= (ferry-capacity) 1)
            (at ?car ?loc) 
            (at-ferry ?loc) )
       :effect (and
		    (decrease (ferry-capacity) 1)
            (on ?car)
		    (not (at ?car ?loc)) ))

   (:action debark
       :parameters  (?car - car  ?loc - location)
       :precondition  (and (on ?car) (at-ferry ?loc))
       :effect (and
		    (increase (ferry-capacity) 1)
            (at ?car ?loc)
		    (not (on ?car)))))
