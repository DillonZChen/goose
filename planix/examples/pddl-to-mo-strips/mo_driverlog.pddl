(define (domain driverlog)
  (:requirements :typing) 
  (:types location driver truck obj)

  (:predicates
   (at-truck ?t - truck ?loc - location)
   (at-driver ?d - driver ?loc - location)
   (at-pkg ?p - obj ?loc - location)
   (in ?obj1 - obj ?obj - truck)
   (driving ?d - driver ?v - truck)
   (link ?x ?y - location) (path ?x ?y - location)
   (empty ?v - truck)
   )
  
  (:functions
   (time-to-walk ?l1 ?l2 - location)
   (time-to-drive ?l1 ?l2 - location)
   (driven ?truck - truck)
   (walked ?driver - driver)
   )

  (:action LOAD-TRUCK
   :parameters (?obj - obj ?truck - truck ?loc - location)
   :precondition (and (at-truck ?truck ?loc) (at-pkg ?obj ?loc))
   :effect (and (not (at-pkg ?obj ?loc)) (in ?obj ?truck))
   )

  (:action UNLOAD-TRUCK
   :parameters (?obj - obj ?truck - truck ?loc - location)
   :precondition (and (at-truck ?truck ?loc) (in ?obj ?truck))
   :effect (and (not (in ?obj ?truck)) (at-pkg ?obj ?loc))
   )

  (:action BOARD-TRUCK
   :parameters (?driver - driver ?truck - truck ?loc - location)
   :precondition (and (at-truck ?truck ?loc)
		      (at-driver ?driver ?loc)
		      (empty ?truck))
   :effect (and (not (at-driver ?driver ?loc))
		(driving ?driver ?truck)
		(not (empty ?truck)))
   )

  (:action DISEMBARK-TRUCK
   :parameters (?driver - driver ?truck - truck ?loc - location)
   :precondition (and (at-truck ?truck ?loc) (driving ?driver ?truck))
   :effect (and (not (driving ?driver ?truck))
		(at-driver ?driver ?loc)
		(empty ?truck))
   )

  (:action DRIVE-TRUCK
   :parameters (?truck - truck ?loc-from - location ?loc-to - location
		       ?driver - driver)
   :precondition (and (at-truck ?truck ?loc-from)
		      (driving ?driver ?truck)
		      (link ?loc-from ?loc-to))
   :effect (and (not (at-truck ?truck ?loc-from))
		(at-truck ?truck ?loc-to)
		(increase (driven ?truck) (time-to-drive ?loc-from ?loc-to)))
   )

  (:action WALK
   :parameters (?driver - driver ?loc-from - location ?loc-to - location)
   :precondition (and (at-driver ?driver ?loc-from) (path ?loc-from ?loc-to))
   :effect (and (not (at-driver ?driver ?loc-from)) (at-driver ?driver ?loc-to)
		(increase (walked ?driver) (time-to-walk ?loc-from ?loc-to)))
   )
  
  )
