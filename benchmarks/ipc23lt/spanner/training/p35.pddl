;; spanners=3, nuts=2, locations=5, out_folder=training/easy, instance_id=35, seed=62

(define (problem spanner-35)
 (:domain spanner)
 (:objects 
    bob - man
    spanner1 spanner2 spanner3 - spanner
    nut1 nut2 - nut
    shed location1 location2 location3 location4 location5 gate - location
 )
 (:init 
    (at bob shed)
    (at spanner1 location5)
    (usable spanner1)
    (at spanner2 location2)
    (usable spanner2)
    (at spanner3 location1)
    (usable spanner3)
    (at nut1 gate)
    (loose nut1)
    (at nut2 gate)
    (loose nut2)
    (link shed location1)
    (link location5 gate)
    (link location1 location2)
     (link location2 location3)
     (link location3 location4)
     (link location4 location5)
 )
 (:goal  (and (tightened nut1)
   (tightened nut2))))
