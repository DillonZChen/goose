;; spanners=2, nuts=1, locations=5, out_folder=training/easy, instance_id=31, seed=58

(define (problem spanner-31)
 (:domain spanner)
 (:objects 
    bob - man
    spanner1 spanner2 - spanner
    nut1 - nut
    shed location1 location2 location3 location4 location5 gate - location
 )
 (:init 
    (at bob shed)
    (at spanner1 location5)
    (usable spanner1)
    (at spanner2 location2)
    (usable spanner2)
    (at nut1 gate)
    (loose nut1)
    (link shed location1)
    (link location5 gate)
    (link location1 location2)
     (link location2 location3)
     (link location3 location4)
     (link location4 location5)
 )
 (:goal  (and (tightened nut1))))
