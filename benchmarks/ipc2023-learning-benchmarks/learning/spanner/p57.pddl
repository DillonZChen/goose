;; spanners=5, nuts=3, locations=7, out_folder=training/easy, instance_id=57, seed=84

(define (problem spanner-57)
 (:domain spanner)
 (:objects 
    bob - man
    spanner1 spanner2 spanner3 spanner4 spanner5 - spanner
    nut1 nut2 nut3 - nut
    shed location1 location2 location3 location4 location5 location6 location7 gate - location
 )
 (:init 
    (at bob shed)
    (at spanner1 location6)
    (usable spanner1)
    (at spanner2 location3)
    (usable spanner2)
    (at spanner3 location7)
    (usable spanner3)
    (at spanner4 location1)
    (usable spanner4)
    (at spanner5 location4)
    (usable spanner5)
    (at nut1 gate)
    (loose nut1)
    (at nut2 gate)
    (loose nut2)
    (at nut3 gate)
    (loose nut3)
    (link shed location1)
    (link location7 gate)
    (link location1 location2)
     (link location2 location3)
     (link location3 location4)
     (link location4 location5)
     (link location5 location6)
     (link location6 location7)
 )
 (:goal  (and (tightened nut1)
   (tightened nut2)
   (tightened nut3))))
